/**
 * Authors: Daniel Begel, Ryan Stevens
 * 
 * motor driver for pulse, direction, free stepper motors. contains an internal
 */

#include <stdio.h>
#include "config.h"

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

typedef struct Motor{
    int PU_PIN; //Pulse pin
    int DIR_PIN; //Direction pin
    int MF_PIN; //Motor-Free pin
    int minLimit;
    int maxLimit;
    int LIMIT_SW_PIN[2]; //pin for limit switch
    int numSW;
    int interrupt;
    int pos; //steps away from home 
    int steps_in_rev; //num steps in one revolution as derived from ratio 
                      //and motor steps per revolution
    // int LOWER_LIMIT;
    // int UPPER_LIMIT;


    //Constructor
    void (*init)(struct Motor *, int);
    void (*initCstm)(struct Motor *, int);


    //Getters and Setters
    void (*set_home)(struct Motor *);
    int (*get_pos)(struct Motor *);

    //Utility
    void (*step)(struct Motor *, int);
    void (*spin_rev)(struct Motor *, int);
    void (*spin_deg)(struct Motor *, int);
    void (*home)(struct Motor *);
    int (*rehome)(struct Motor *);
}Motor;


void initMotor(struct Motor * motor, int Pin1, int* pinSW, int numSW,int deg_min, int deg_max, int steps_per_rev, float ratio) {
    initMotorCstm(motor, Pin1, Pin1 + 1, Pin1 + 2, pinSW, numSW, deg_min, deg_max, steps_per_rev, ratio);
}

//Constructor. maximum of 2 limit switches supported.
void initMotorCstm(struct Motor * motor, int pinSTP, int pinDIR, int pinMF, int* pinSW, int numSW, int deg_min, int deg_max, int steps_per_rev, float ratio) {

    motor->PU_PIN = pinSTP;
    motor->DIR_PIN = pinDIR;
    motor->MF_PIN = pinMF;
    motor->steps_in_rev = (int)(ratio * steps_per_rev);
    motor->minLimit = degToStep(motor, deg_min);
    motor->maxLimit = degToStep(motor, deg_max);
    motor->numSW = numSW; // max 2
    motor->interrupt = 0;

    for(int i = 0; i < numSW; i++) {
        motor->LIMIT_SW_PIN[i] = pinSW[i];
    }

    motor->set_home = &set_home;
    motor->get_pos = &get_pos;
    motor->step = &step;
    motor->spin_rev = &spin_rev;
    motor->spin_deg = &spin_deg;
    motor->home = &home;
    motor->rehome = &rehome;

    //16 bit, 6 step per word, 18us per word, 1us per bit
    uint32_t bitperiod = (_CLOCKFREQ / 1000000);
    // calculate smartpin mode for 16 bits per character
    uint32_t bit_mode = 15 + (bitperiod << 16);

    _pinstart(motor->PU_PIN,_txmode,bit_mode, 0);

    _pinh(motor->DIR_PIN);

    if(motor->MF_PIN != NULL_PIN ) {
        _pinl(motor->MF_PIN);
    }
    

    //(*motor->rehome)(motor);
}


//Setter / Getter
void set_home(struct Motor * motor) { //set current position to new 0 position
    motor->pos = 0;
}

int get_pos(struct Motor * motor) { //get position in steps 
    return (motor->pos);
}



//Utility
void step(struct Motor * motor, int steps) {
    int dir = 1;
    if(steps < 0) {
        _pinh(motor->DIR_PIN);
        steps*=-1;
        dir = -1;
    }
    else {
        _pinl(motor->DIR_PIN);
        dir = 1;
    }

    printf("Steps: %d\n", steps);

    for(int i = 0; i < steps; i++) {
        if(pollSwitches(motor) == 1 || // check limit switches
            motor->pos + dir*i < motor->minLimit || //check step limits
            motor->pos + dir*i > motor->maxLimit) 
        { // hit a limit
            printf("hit limit! Pos = %d, max = %d, min = %d, lastStep = %d\n", motor->pos,motor->maxLimit,motor->minLimit,dir*i);
        interrupt();
        motor->pos = (motor->pos + dir*i) % motor->steps_in_rev; //update pos
        return;
        }
    
        
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
    }

    _pinl(motor->DIR_PIN);
    motor->pos = (motor->pos + dir*steps) % motor->steps_in_rev;
}

void spin_rev(struct Motor * motor, int revs) {
    int steps = revs * motor->steps_in_rev;

    motor->step(motor, steps);
}

// void spin_rev(struct Motor * motor, int revs) {
//     int steps = revs*REV_TO_STEP;
//     step(&motor, steps);
// }

void spin_deg(struct Motor * motor, float degs) {
    int steps = degToStep(motor, degs);
    motor->step(motor, steps);
}

int degToStep(struct Motor * motor, float degs) {// degs must be between 0,360
    return (int)((degs/360.00) * motor->steps_in_rev);
}

void home(struct Motor * motor) { //return to home position
    int steps = -1*((motor->pos)%motor->steps_in_rev); //calculate the steps needed to get back 
    if(steps > motor->steps_in_rev/2) {
        steps = -1*(motor->steps_in_rev-steps);
    }

    motor->step(motor, steps);
}

int rehome(struct Motor * motor) {

    _pinh(motor->DIR_PIN); //set direction

    while(pollSwitches(motor) != 1) {
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
    }

    motor->pos = 0;
    return 1;
}

int pollSwitches(Motor *motor) {
    //printf("poll switches!\n");
    /*for(int i = 0; i < motor->numSW; i++) {
        //printf("pin %d reading: %d\n",motor->LIMIT_SW_PIN[i],_pinr(motor->LIMIT_SW_PIN[i]));
        if(_pinr(motor->LIMIT_SW_PIN[i])==1) {
            return 1;
        }
        //_waitms(100);
    }*/
    return 0;
}

// TODO: define what motor does when hits limit switch and interrupts. probably 
// will have to send a message to the pi.
void interrupt() {

}

/*
int main() {
    
    Motor *alt = {0};

    int numSwitch = 1;
    int swPin = 1;

    initMotor(alt, 0, &swPin, numSwitch, 0, 360, 400000, 1);

    rehome(alt);

    spin_deg(alt, 360);
}*/

