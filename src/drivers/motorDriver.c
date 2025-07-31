/**
 * Authors: Daniel Begel, Ryan Stevens
 * 
 * motor driver for pulse, direction, free stepper motors. contains an internal
 */

#include <stdio.h>
#include "config.h"

#ifndef HASP25_MOTOR_DRIVER
#define HASP25_MOTOR_DRIVER

//UART step signals using little endian format
#define DEF_ONE_STEP            0b1111111000000000
#define STEP                    0b1111111111111110 //only use this one
#define TWO_STEP                0b1111111001111110
#define THREE_STEP              0b1111001111001110
#define FOUR_STEP               0b1110011100110010
#define FIVE_STEP_OVERDRIVE     0b1100110011011010
#define SIX_STEP_OVERDRIVE      0b1101101101101101 

#define NULL_PIN -1
const uint32_t _txmode = 0b00000000000000000000000001111100; //async tx mode, output enabled for smart output

//other constants
const int DELAY = 50;

//TODO: test gear ratio support 
//TODO: test limit switch homing support
//TODO: polish this up and get it working better

typedef struct Motor{
    int PU_PIN; //Pulse pin
    int DIR_PIN; //Direction pin
    int MF_PIN; //Motor_t-Free pin
    int minLimit;
    int maxLimit;
    int limit_min_pin; //pin for limit switch
    int limit_max_pin; //pin for limit switch
    int interrupt;
    int pos; //steps away from home 
    int steps_in_rev; //num steps in one revolution as derived from ratio 
                      //and motor steps per revolution
    int stepBuff; //steps to move
    int homeDir;
    int dirMod;
    // int LOWER_LIMIT;
    // int UPPER_LIMIT;

    //Getters and Setters
    void (*set_home)(struct Motor *);
    int (*get_pos)(struct Motor *);

    //Utility
    void (*step)(struct Motor *, int);
    void (*spin_rev)(struct Motor *, int);
    void (*spin_deg)(struct Motor *, int);
    void (*home)(struct Motor *);
    int (*rehome)(struct Motor *);
}Motor_t;


void initMotor(struct Motor * motor, int Pin1, int sw1, int sw2,int deg_min, int deg_max, int steps_per_rev, float ratio, int homeDir, int dirMod) {
    initMotorCstm(motor, Pin1, Pin1 + 1, Pin1 + 2, sw1, sw2, deg_min, deg_max, steps_per_rev, ratio, homeDir, dirMod);
}

//Constructor. maximum of 2 limit switches supported.
void initMotorCstm(struct Motor * motor, int pinSTP, int pinDIR, int pinMF, int sw1, int sw2, int deg_min, int deg_max, int steps_per_rev, float ratio , int homeDir, int dirMod) {

    motor->PU_PIN = pinSTP;
    motor->DIR_PIN = pinDIR;
    motor->MF_PIN = pinMF;
    motor->minLimit = degToStep(motor, deg_min);
    motor->maxLimit = degToStep(motor, deg_max);
    motor->limit_min_pin = sw1;
    motor->limit_max_pin = sw2;
    motor->interrupt = 0;
    motor->steps_in_rev = (int)(ratio * steps_per_rev);
    set_home(motor);
    motor->stepBuff = 0;
    motor->homeDir = homeDir;
    motor->dirMod = dirMod;

    //printf("ratio %f, spr %d, sir %d\n", ratio, steps_per_rev, motor->steps_in_rev);

    motor->set_home = &set_home;
    motor->get_pos = &get_pos;
    motor->step = &step;
    motor->spin_rev = &spin_rev;
    motor->spin_deg = &spin_deg;
    motor->home = &home;
    motor->rehome = &rehome;


    _pinh(motor->DIR_PIN);

    if(motor->MF_PIN != NULL_PIN ) {
        _pinl(motor->MF_PIN);
    }

    //16 bit, 6 step per word, 18us per word, 1us per bit
    uint32_t bitperiod = (_CLOCKFREQ / 1000000);
    // calculate smartpin mode for 16 bits per character
    uint32_t bit_mode = 15 + (bitperiod << 16);

    _pinstart(motor->PU_PIN,_txmode,bit_mode, 0);
}


//Setter / Getter
void set_home(struct Motor * motor) { //set current position to new 0 position
    motor->pos = 0;
}

int get_pos(struct Motor * motor) { //get position in steps 
    return motor->pos;
}

//Utility
void step(struct Motor * motor, int steps) {
    // printf("arrived! stepBuff: %d\n\n", motor->stepBuff);
    int dir = 1;
    _dirh(motor->DIR_PIN);
    //printf("steps: %d\n", steps);
    steps = steps*motor->dirMod;
    int itrs = steps;
    if(steps < 0) {
        _pinh(motor->DIR_PIN);
        // printf("motor_pin: %d, -1\n", motor->PU_PIN);
        itrs*=-1;
        dir = -1;
    }
    else {
        _pinl(motor->DIR_PIN);
        // printf("motor_pin: %d, 1\n", motor->DIR_PIN);
        dir = 1;
    }

    //printf("Steps: %d\n", steps);
    //printf("stepBuff: %d\n", motor->stepBuff);

    int limit = 0;
    for(int i = 0; i < itrs; i++) {
        limit = pollSwitches(motor);
        if(limit > 0 || // check limit switches
            motor->pos + dir*i < motor->minLimit || //check step limits
            motor->pos + dir*i > motor->maxLimit) 
        { // hit a limit
            //printf("hit limit! Pos = %d, max = %d, min = %d, lastStep = %d\n", motor->pos,motor->maxLimit,motor->minLimit,dir*i);
            if((limit == 1 && dir <= 0) || (limit == 2 && dir >= 0)) {
                motor->pos = (motor->pos + dir*i) % motor->steps_in_rev; //update pos
                if(steps < 0) {
                    motor->stepBuff -= i;
                }
                else {
                    motor->stepBuff += i;
                }
                motor->stepBuff = 0;
                return;
            }
        }
        //printf("arrived\n");
        //printf("step: pin %d\n", motor->PU_PIN);
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
    }

    // printf("stepBuff: %d\n", motor->stepBuff);
    //_pinl(motor->DIR_PIN);
    motor->pos = (motor->pos + dir*steps) % motor->steps_in_rev;
    motor->stepBuff -= steps;
}

void spin_rev(struct Motor * motor, int revs) {
    int steps = revs * motor->steps_in_rev;

    step(motor, steps);
}

void update_target_deg(struct Motor * motor, float degs) {
    int steps = degToStep(motor, degs);
    // printf("degrees: %f, steps: %d", degs, steps);
    motor->stepBuff += steps;
    // printf("steps: %d, target: %d\n", steps, motor->stepBuff);
}

void move_toward_target_deg(struct Motor * motor, float degsMax) {
    int stepsMax = degToStep(motor, degsMax);

    if(stepsMax < 0) {
        stepsMax*=-1;
    }

    //printf("%d, %d, %d, %d\n", motor->target, motor->pos, distance, motor->DIR_PIN);
    if(motor->stepBuff < 0) {
        stepsMax *= -1;
        int steps = (((motor->stepBuff) < (stepsMax)) ? stepsMax : motor->stepBuff);
        //printf("%d, %d, %d, %d, %d\n", motor->target, motor->pos, distance, steps, motor->DIR_PIN);
        step(motor, steps);
    }
    else if(motor->stepBuff > 0) {//printf("arrived");
        int steps = (((motor->stepBuff) > (stepsMax)) ? stepsMax : motor->stepBuff);
        //printf("%d, %d, %d, %d, %d\n", motor->target, motor->pos, distance, steps, motor->DIR_PIN);
        step(motor, steps);
    }
    else{
        //steps == 0 do nothing
    }
}

// void spin_rev(struct Motor * motor, int revs) {
//     int steps = revs*REV_TO_STEP;
//     step(&motor, steps);
// }

void spin_deg(struct Motor * motor, float degs) {
    int steps = degToStep(motor, degs);
    //printf("steps %d, degs %f\n", steps, degs);
    step(motor, steps);
}

int degToStep(struct Motor * motor, float degs) {// degs must be between 0,360
    return (int)((degs/360.00) * motor->steps_in_rev);
}

void home(struct Motor * motor) { //return to home position
    motor->stepBuff = 0;
    int steps = -1*((motor->pos)%motor->steps_in_rev); //calculate the steps needed to get back 
    if(steps > motor->steps_in_rev/2) {
        steps = -1*(motor->steps_in_rev-steps);
    }

    motor->step(motor, steps);
    motor->stepBuff = motor->pos;
}

int rehome(struct Motor * motor) {

    if(motor->dirMod * motor->homeDir < 0) {
        _pinh(motor->DIR_PIN); //set direction
    }
    else {
        _pinl(motor->DIR_PIN);
    }
    
    int sw = 0;

    while( sw <= 0) {
        sw = pollSwitches(motor);
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
        // printf("Motor pin: %d, sw: %d\n", motor->PU_PIN, sw);
    }

    motor->pos = 0;
    motor->stepBuff = motor->pos;
    return 1;
}

#define enable 0
#define min 1
#define max 2
int pollSwitches(Motor_t *motor) {
    //printf("poll switches!\n");
    if(motor->limit_min_pin != -1) {// if min pin enabled
        if(motor->limit_max_pin != -1) { //if max pin also enabled
            if(_pinr(motor->limit_min_pin) != enable) {
                return min;
            }
            if(_pinr(motor->limit_max_pin) != enable) {
                return max;
            }
        }    
        else {// if only min pin enabled
            if(_pinr(motor->limit_min_pin) != enable) {
                return min;
            }
        }
    }
    else if(motor->limit_max_pin != -1) {//if only max pin enabled
        if(_pinr(motor->limit_max_pin) != enable) {
            return max;
        }
    }
    else if(motor->limit_min_pin == -1 && motor->limit_max_pin == -1) {//if no pin enabled
        return 0;
    }
    return 0;//no switch triggered
}

/*
int main() {
    
    Motor_t *alt = {0};

    int numSwitch = 1;
    int swPin = 1;

    initMotor(alt, 0, &swPin, numSwitch, 0, 360, 400000, 1);

    rehome(alt);

    spin_deg(alt, 360);
}*/
#endif
