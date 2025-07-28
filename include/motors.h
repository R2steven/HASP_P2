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
#include "src/comms.c"
#include "src/drivers/motorDriver.c"

// motor IDs
#define ALL_CMD 0x57
#define ALT_ID 0xB5
#define AZ_ID 0xBD
#define MIR_ID 0xD5

typedef struct motorCommand {
    int motoId;
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
    //    4           execute sspin degree command utilizing deg and dir values
    //                  $motorID, $commandID, $param1, $param2
    #define HOME 5
    //    5           execute the homing subroutine
    //                  $motorID, $commandID
    #define INIT 6
    //    6           execute initialization of the motor
    #define GETPOS 7 //only defined for ALL_CMD
    //    7           prompt the motors to transmit their current position

    int step; //steps parameter (can be negative)
    float degree; //degree parameter (can be negative)
    int direction; //direction parameter (must be 1 or -1)
}motorCommand;

typedef struct HASP25Motors_s {
    Motor_t altitude; // altitude motor
    Motor_t azimuth; // azimuth motor
    Motor_t mirror; // mirror motor
    Deque *commands; // motor command queue
    HASP25_Comms *comms; // comms module
    uint8_t *altStack;
    uint8_t *azStack;
    uint8_t *mirrorStack;
    uint8_t altRunning;
    uint8_t azRunning;
    uint8_t mirrorRunning;
}HASP25Motors_t;

typedef struct Motor_consts_s {
    /////////////////////
    // Altitude constants M1
    /////////////////////
    const uint8_t altStep; //alt step pin
    const uint8_t altDir; //alt dir pin
    const uint8_t altMF; //alt enable pin (enable on low)
    const int altminsw1; //limit switch pins
    const int altmaxsw2; //number of limit switches
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
    int aziminsw1; //limit switch pins
    int azimaxsw2;
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
    int mirrorminsw1; //limit switch pins
    int mirrormaxsw2;
    const float mirrordegmin;
    const float mirrordegmax;
    const int mirrorSPR; //mirror steps per revolution
    const float mirrorRatio; //60 to 1 gear Ratio (gear down)

}Motor_consts_t;

struct Motor_consts_s MOTOR_CONSTS = {
    36, //.altStep    alt step pin
    35, //.altDir alt dir pin
    34, //.altMF alt enable pin (enable on low)
    10, //.altminsw1
    9, //.altmaxsw2 limit switch pins
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
    8,
    -1, //.aziSwitches limit switch pins
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
    7,
    -1, //.mirrorSwitches limit switch pins
    0.0, //.mirrordegmin
    45.0, //.mirrordegmax
    3200, //.mirrorSPR mirror steps per revolution
    60 //.mirrorRatio 60 to 1 gear Ratio (gear down)
};

/**
 * initializes the HASP 25 motors with the defied values and returns a 
 * pointer to it's command recieve buffer
 */
HASP25Motors_t *initMotors(HASP25_Comms *comms) __fromfile("src/motors.c");

/**
 * enter the motor control loop. intended to be run as a thread
 */
int runMotors() __fromfile("src/motors.c"); 

uint8_t motorRxHandler(Deque* notUsed, uint8_t* input, uint16_t length)__fromfile("src/motors.c");
#endif
