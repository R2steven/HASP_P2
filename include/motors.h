/**
 * Author: Ryan Stevens
 * 
 * HASP25 Motor thread. configures and runs the SpectraSolis Motors, as well as 
 * provides a place to tune the motor parameters.
 */

#ifndef HASP25_MOTOR_MODULE
#define HASP25_MOTOR_MODULE

#include "config.h"
#include "deque.h"
#include "charDeque.h"

// motor IDs
#define ALL_CMD 0x57
#define ALT_ID 0xB5
#define AZ_ID 0xBD
#define MIR_ID 0xD5

typedef struct motorCommand {
    int cmd; //command to execute
    //   command table:
    //   ID:        DESC:
    #define MSTEP 1
    //    1           execute the step command utilizing only step value for step
    //                    and direction
    #define DEG  2
    //    2           execute the spin degree command utilizing only Deg value 
    //                    for step and direction
    #define STEPDIR 3
    //    3           execute step command utilizing step and dir values
    #define DEGDIR 4
    //    4           execute sspin degree command utilizing ged and dir values
    #define HOME 5
    //    5           execute the homing subroutine
    #define INIT 6
    //    6           execute initialization of the motor
    #define GETPOS 7 //only defined for ALL_CMD
    //    7           prompt the motors to transmit their current position

    int step; //steps parameter (can be negative)
    float degree; //degree parameter (can be negative)
    int direction; //direction parameter (must be 1 or -1)
}motorCommand;


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
    const int altSPR; //alt steps per revolution
    const float altRatio; //3 to 1 gear Ratio (gear down)

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
    const int aziSPR; //azi steps per revolution
    const float aziRatio; //6 to 1 gear Ratio (gear down)

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
    const int mirrorSPR; //mirror steps per revolution
    const float mirrorRatio; //60 to 1 gear Ratio (gear down)

};

struct A MOTOR_CONSTS = {
    36, //.altStep    alt step pin
    35, //.altDir alt dir pin
    34, //.altMF alt enable pin (enable on low)
    {6,7}, //.altSwitches limit switch pins
    2, //.altNumSw number of limit switches
    0.0, //.altdegmin degree minimum limit
    47.0, //.altdegmax degree maximim limit
    40000, //.altSPR alt steps per revolution
    3, //.altRatio 3 to 1 gear Ratio (gear down)

    /////////////////////
    // Azimuth constants M4
    /////////////////////
    39, //.azi Stepazi step pin
    38, //.azi Dirazi dir pin
    37, //.aziMF azi enable pin (enable on low)
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
charDeque *initMotors() __fromfile("src/motors.c");

void setMotorTx(charDeque *tx) __fromfile("src/sensors.c");

/**
 * enter the motor control loop. intended to be run as a thread
 */
void runMotors() __fromfile("src/motors.c"); 

#endif
