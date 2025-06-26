#include "config.h"
#include "motors.h"
#include "src/drivers/motorDriver.c"
#include "stdlib.h"

typedef struct HASP25Motors_s {
    Motor altitude; // altitude motor
    Motor azimuth; // azimuth motor
    Motor mirror; // mirror motor
    Deque *rxBuff; // command buffer
    uint8_t rxLock; // recieve buffer threadLock
}HASP25Motors_t;

typedef struct motorCommand {
    int step;
    float degree;
    int direction;
}motorCommand;

HASP25Motors_t *motorInst;
uint16_t rxBuffSize = 128;

/**
 * initializes the HASP 25 motors with the defied values and returns a 
 * pointer to it's command recieve buffer
 * 
 * returns a pointer to a recieve buffer. pointer[0] should always be
 * a uint16_t integer denoting the buffer size excluding the size
 */
Deque *initMotors() {

    HASP25Motors_t *motors = (HASP25Motors_t*) malloc(sizeof(HASP25Motors_t));
    
    /**
     * altitude motor config
     */
    initMotorCstm(
        &motors->altitude, 
        MOTOR_CONSTS.altStep, 
        MOTOR_CONSTS.altDir, 
        MOTOR_CONSTS.altMF, 
        MOTOR_CONSTS.altSwitches, 
        MOTOR_CONSTS.altNumSw,
        MOTOR_CONSTS.altdegmin,
        MOTOR_CONSTS.altdegmax,
        MOTOR_CONSTS.altSPR,
        MOTOR_CONSTS.altRatio
    );

    /**
     * azimuth motor config
     */
    initMotorCstm(
        &motors->azimuth,
        MOTOR_CONSTS.aziStep,
        MOTOR_CONSTS.aziDir,
        MOTOR_CONSTS.aziMF,
        MOTOR_CONSTS.aziSwitches,
        MOTOR_CONSTS.aziNumSw,
        MOTOR_CONSTS.azidegmin,
        MOTOR_CONSTS.azidegmax,
        MOTOR_CONSTS.aziSPR,
        MOTOR_CONSTS.aziRatio
    );

    /**
     *  mirror motor config
     */
    initMotorCstm(
        &motors->mirror,
        MOTOR_CONSTS.mirrorStep,
        MOTOR_CONSTS.mirrorDir,
        MOTOR_CONSTS.mirrorMF,
        MOTOR_CONSTS.mirrorSwitches,
        MOTOR_CONSTS.mirrorNumSw,
        MOTOR_CONSTS.mirrordegmin,
        MOTOR_CONSTS.mirrordegmax,
        MOTOR_CONSTS.mirrorSPR,
        MOTOR_CONSTS.mirrorRatio
    );
    _pinh(MOTOR_CONSTS.mirrorMicroStep1); //set microstep behavior
    _pinh(MOTOR_CONSTS.mirrorMicroStep2); //set microstep behavior

    motorInst = motors;
    motors->rxBuff = initDeque(rxBuffSize, sizeof(char));

    return motors->rxBuff;
}


void runMotors() {

    while(true) {
        (*motorInst->altitude.spin_deg)(&motorInst->altitude, 20);
        (*motorInst->azimuth.spin_deg)(&motorInst->azimuth, 360);
        (*motorInst->altitude.spin_deg)(&motorInst->altitude, -20);
        (*motorInst->mirror.spin_deg)(&motorInst->mirror, 45);
        (*motorInst->mirror.spin_deg)(&motorInst->mirror, -45);
    }
}

/**
 * check the rx buffer for recieved commands and load them into ret.
 */
void rxCheck(motorCommand* ret) {
    if(motorInst->rxLock == true) {
        return;
    }
    motorInst->rxLock = true;

    char read = 0;
    while(motorInst->rxBuff->size > 0) {
        dqDequeue(motorInst->rxBuff, &read);
        //decode read
    }
    motorInst->rxLock = 0;
}
