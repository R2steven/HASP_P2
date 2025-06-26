/**
 * motor module, meant to be run in a separate core
 */

#ifndef HASP25_MOTOR_MODULE
#define HASP25_MOTOR_MODULE

#include "config.h"
#include "deque.h"

struct A {
    /////////////////////
    // Altitude constants M1
    /////////////////////
    const uint8_t altStep; //alt step pin
    const uint8_t altDir; //alt dir pin
    const uint8_t altMF; //alt enable pin (enable on low)
    int altSwitches[2]; //limit switch pins
    const uint8_t altNumSw; //number of limit switches
    const float altdegmin; //degree minimum limit
    const float altdegmax; //degree maximim limit
    const uint8_t altSPR; //alt steps per revolution
    const uint8_t altRatio; //3 to 1 gear Ratio (gear down)

    /////////////////////
    // Azimuth constants M4
    /////////////////////
    const uint8_t aziStep; //azi step pin
    const uint8_t aziDir; //azi dir pin
    const uint8_t aziMF; //azi enable pin (enable on low)
    int aziSwitches[2]; //limit switch pins
    const uint8_t aziNumSw;
    const float azidegmin;
    const float azidegmax;
    const uint8_t aziSPR; //azi steps per revolution
    const uint8_t aziRatio; //6 to 1 gear Ratio (gear down)

    /////////////////////
    // Instrument Mirror constants
    /////////////////////
    const uint8_t mirrorStep; //mirror step pin
    const uint8_t mirrorDir; //mirror direction pin
    const uint8_t mirrorMF;
    const uint8_t mirrorMicroStep1; //mirror microstep1 pin
    const uint8_t mirrorMicroStep2; //mirror microstep2 pin
    int mirrorSwitches[2]; //limit switch pins
    const uint8_t mirrorNumSw;
    const float mirrordegmin;
    const float mirrordegmax;
    const uint8_t mirrorSPR; //mirror steps per revolution
    const uint8_t mirrorRatio; //60 to 1 gear Ratio (gear down)

};

struct A MOTOR_CONSTS = {
    34, //.altStep    alt step pin
    35, //.altDiralt dir pin
    36, //.altMF alt enable pin (enable on low)
    {6,7}, //.altSwitches limit switch pins
    2, //.altNumSw number of limit switches
    0.0, //.altdegmin degree minimum limit
    47.0, //.altdegmax degree maximim limit
    40000, //.altSPR alt steps per revolution
    3, //.altRatio 3 to 1 gear Ratio (gear down)

    /////////////////////
    // Azimuth constants M4
    /////////////////////
    37, //.azi Stepazi step pin
    38, //.azi Dirazi dir pin
    39, //.aziMF azi enable pin (enable on low)
    {30,0}, //.aziSwitches limit switch pins
    1, //.aziNumSw
    -360.0, //.azidegmin
    2*360, //.azidegmax
    40000, //.aziSPR azi steps per revolution
    6, //.aziRatio 6 to 1 gear Ratio (gear down)

    /////////////////////
    // Instrument Mirror constants
    /////////////////////
    16, //.mirrorStep mirror step pin
    17, //.mirrorDir mirror direction pin
    -1, //.mirrorMF
    18, //.mirrorMicroStep1 mirror microstep1 pin
    19, //.mirrorMicroStep2 mirror microstep2 pin
    {31,0}, //.mirrorSwitches limit switch pins
    1, //.mirrorNumSw
    0.0, //.mirrordegmin
    45.0, //.mirrordegmax
    3200, //.mirrorSPR mirror steps per revolution
    60 //.mirrorRatio 60 to 1 gear Ratio (gear down)
};

/**
 * initializes the HASP 25 motors with the defied values and returns a 
 * pointer to it's command recieve buffer
 * 
 * returns a pointer to a recieve buffer. pointer[0] should always be
 * a uint16_t integer denoting the buffer size excluding the size
 */
Deque *initMotors() __fromfile("src/motors.c");


/**
 * enter the motor control loop. intended to be run as a thread
 */
void runMotors() __fromfile("src/motors.c"); 

#endif
