/**
 * Authors: Daniel Begel, Ryan Stevens
 * 
 * motor driver for pulse, direction, free stepper motors. contains an internal
 */

#include <stdio.h>
#include "config.h"

//UART step signals using little endian format
#define STEP                    0b1111111111111110 //only use this one
#define TWO_STEP                0b1111111001111110
#define THREE_STEP              0b1111001111001110
#define FOUR_STEP               0b1110011100110010
#define FIVE_STEP_OVERDRIVE     0b1100110011011010
#define SIX_STEP_OVERDRIVE      0b1101101101101101 

const uint32_t _txmode = 0b00000000000000000000000001111100; //async tx mode, output enabled for smart output

//other constants
const int DELAY = 50;

//TODO: test gear ratio support 
//TODO: test limit switch homing support

typedef struct Motor{
    int PU_PIN; //Pulse pin
    int DIR_PIN; //Direction pin
    int MF_PIN; //Motor-Free pin
    int LIMIT_SW_PIN; //pin for limit switch
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


//Constructor
void initMotor(struct Motor * motor, int Pin1, int steps_per_rev, float ratio) {

    motor->PU_PIN = Pin1;
    motor->DIR_PIN = Pin1+1;
    motor->MF_PIN = Pin1+2;
    motor->steps_in_rev = (int)(ratio * steps_per_rev);

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

    _pinl(motor->MF_PIN);

    motor->home;
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
    if(steps < 0) {
        _pinh(motor->DIR_PIN);
        steps*=-1;
    }
    else {
        _pinl(motor->DIR_PIN);
    }

    for(int i = 0; i < steps; i++) {
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
    }
    _pinl(motor->DIR_PIN);
    motor->pos = (motor->pos + steps) % motor->steps_in_rev;
}

void spin_rev(struct Motor * motor, int revs) {
    int steps = revs * motor->steps_in_rev;

    motor->step(motor, steps);
}

// void spin_rev(struct Motor * motor, int revs) {
//     int steps = revs*REV_TO_STEP;
//     step(&motor, steps);
// }

void spin_deg(struct Motor * motor, int degs) {
    int steps = (int)((degs/360) * motor->steps_in_rev);
    motor->step(motor, steps);
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

    while(getpin(motor->LIMIT_SW_PIN) == 0) {
        _wypin(motor->PU_PIN,STEP);
        _waitus(DELAY);
    }

    motor->pos = 0;
    return 1;
}


/**int main() {
    
    Motor *alt = {0};

    init(alt,0);

    spin_deg(alt, 360);
}*/

