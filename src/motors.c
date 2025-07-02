/**
 * Author: Ryan Stevens
 * 
 * implementation of the HASP25 Motor thread. configures and runs the 
 * SpectraSolis Motors
 */

#include <stdio.h>
#include "config.h"
#include "motors.h"
#include "src/drivers/motorDriver.c"
#include "stdlib.h"


typedef struct HASP25Motors_s {
    Motor_t altitude; // altitude motor
    Motor_t azimuth; // azimuth motor
    Motor_t mirror; // mirror motor
    charDeque *rxBuff; // command buffer
    charDeque *txBuff;
}HASP25Motors_t;

HASP25Motors_t *motorInst;
uint16_t rxBuffSize = 128;

/**
 * initializes the HASP 25 motors with the defied values and returns a 
 * pointer to it's command recieve buffer
 * 
 * returns a pointer to a recieve buffer. pointer[0] should always be
 * a uint16_t integer denoting the buffer size excluding the size
 */
charDeque *initMotors() {

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
    motors->rxBuff = initCharDeque(rxBuffSize);

    return motors->rxBuff;
}


void runMotors() {
    uint8_t altRunning = true;
    uint8_t azRunning = true;
    uint8_t mirrorRunning = true;
    uint8_t altStack[1028] = {0};
    uint8_t azStack[1028] = {0};
    uint8_t mirrorStack[1028] = {0};

    Deque *altCommand = initDeque(20,sizeof(motorCommand));
    Deque *azCommand = initDeque(20,sizeof(motorCommand));
    Deque *mirrorCommand = initDeque(20,sizeof(motorCommand));


    __builtin_cogstart(runMotor(&motorInst->altitude,&altRunning, altCommand), altStack);
    __builtin_cogstart(runMotor(&motorInst->azimuth, &azRunning, azCommand), azStack);
    __builtin_cogstart(runMotor(&motorInst->mirror, &mirrorRunning, mirrorCommand), mirrorStack);

    
    motorCommand cmd = {0};

    int count = 0;
    
    while(true) {
        cmd.cmd = -1;
        int id = rxCheck(&cmd);
        if(id != -1) {
            //printf("MOTORID %d\n", id);
        }
        
        switch(id) {
            case ALL_CMD :
                switch (cmd.cmd) {
                case GETPOS:
                    // TODO: implement returning all positions
                    break;
                default:
                    break;
                }
                break;
            case ALT_ID :
                // printf("alt %d, %f\n\n", cmd.cmd, cmd.degree);
                //printf("alt commands: %d\n", ++count);
                dqEnqueue(altCommand, &cmd); // thread lock handled by size var
                break;
            case AZ_ID :
                dqEnqueue(azCommand, &cmd);
                break;
            case MIR_ID :
                dqEnqueue(mirrorCommand, &cmd);
                break;
            case -1: break;
            default : break;
        }
    }
}

/**
 * check the rx buffer for recieved commands and load them into ret.
 * 
 * @return the integer motor ID the command was adressed to.
 */
int rxCheck(motorCommand* ret) {
    if(motorInst->rxBuff->threadLock == true) { //wait for full message to be dispatched
        return -1;
    }
    //motorInst->rxBuff->threadLock = true; //enter buffer to read

    char read[64] = {0};
    int curr = 1;
    read[0] = '$';
    //printf("%d",motorInst->rxBuff->size);
    if(motorInst->rxBuff->size <= 0) {
        return -1;
    }
    //printf("%d", motorInst->rxBuff->size);
    while(motorInst->rxBuff->size > 0) { //read until terminator
        while(motorInst->rxBuff->threadLock == true) {
            _waitus(5); // wait and do nothing
        }
        dqDequeueCharDeque(motorInst->rxBuff, &read[curr]);
        if(read[curr-1] == '\n' && read[curr] == '\0') {
            break;
        }
        curr++;
    }//printf("arrived here\n");
    

    //motorInst->rxBuff->threadLock = false; //done reading

    //printf(read);
    //printf("\n");
    //printf("%d\n", motorInst->rxBuff->size);

    int motoID = atoi(&read[1]);
    //printf("MOTORID %d", motoID);
    ret->cmd = -1;
    int param = 0;
    
    
    for(int i = 0; i < curr; i++) {
        if(read[i] == ',') { // read until we see a delimiter, then read 
                                //  the next parameter (delimiter will always be
                                //  followed by parameter)
            i++;
            
            switch (ret->cmd) {
                case -1: //parse command
                    ret->cmd = atoi(&read[i]);;
                    if( ret->cmd == HOME ||
                        ret->cmd == INIT ||
                        ret->cmd == GETPOS) { //if command has no params return
                        return motoID;
                    }
                    break;
                case DEG: //parse params for DEG command
                    ret->degree = atof(&read[i]);
                    ret->direction = 1;
                    return motoID;
                case MSTEP: //parse params for STEP command
                    ret->step = atoi(&read[i]);
                    ret ->direction = 1;
                    return motoID;
                case STEPDIR: //parse params for STEP DIRECTION command
                    if(param == 0) {
                        ret->step = atoi(&read[i]);
                        param++;
                    }
                    else{
                        ret->direction = atoi(&read[i]);
                        return motoID;
                    }
                    break;
                case DEGDIR: //parse params for DEGREE DIRECTION command
                    if(param == 0) {
                        ret->step = atof(&read[i]);
                        param++;
                    }
                    else{
                        ret->direction = atoi(&read[i]);
                        return motoID;
                    }
                    break;
                case HOME:
                    return motoID;
                case INIT:
                    return motoID;
                default:
                    break;
            }
        }
    }
    return motoID;
}

void txData() {

}

void runMotor(Motor_t *motor, uint8_t *motRunning, Deque *motCommand) {
    
    motorCommand cmd = {0};
    
    while(*motRunning) {
        if(motCommand->queueSize > 0) { // decode and execute motor movement
            cmd.cmd = -1;
            dqDequeue(motCommand, &cmd);
            switch(cmd.cmd) {
                case MSTEP:
                    step(motor, cmd.step);
                    break;
                case DEG:
                    spin_deg(motor, cmd.degree);
                    break;
                case STEPDIR:
                    step(motor, cmd.step * cmd.direction);
                    break;
                case DEGDIR:
                    spin_deg(motor, cmd.degree * cmd.direction);
                    break;
                case HOME:
                    home(motor);
                    break;
                case INIT:
                    rehome(motor);
                    break;
                case -1: break;
                default: printf("unexpected command! %d\n", cmd.cmd);break;
            }
        }
    }
}

void setMotorTx(charDeque *tx) {
    motorInst->txBuff = tx;
}

