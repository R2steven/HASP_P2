/**
 * Author: Ryan Stevens
 * 
 * implementation of the HASP25 Motor thread. configures and runs the 
 * SpectraSolis Motors
 */

#include <stdio.h>
#include "motors.h"
#include "stdlib.h"

HASP25Motors_t *motorInst;
uint16_t rxBuffSize = 128;

/**
 * initializes the HASP 25 motors with the defied values and returns a 
 * pointer to it's command recieve buffer
 * 
 * returns a pointer to a recieve buffer. pointer[0] should always be
 * a uint16_t integer denoting the buffer size excluding the size
 */
HASP25Motors_t *initMotors(HASP25_Comms *comms) {

    motorInst = (HASP25Motors_t*) malloc(sizeof(HASP25Motors_t));
    
    /**
     * altitude motor config
     */
    initMotorCstm(
        &(motorInst->altitude), 
        MOTOR_CONSTS.altStep,   //these include errors are because the compiler is
                                //motordrivers.c is included in motors.h and 
                                //linked into this file. stupid compiler
        MOTOR_CONSTS.altDir, 
        MOTOR_CONSTS.altMF, 
        MOTOR_CONSTS.altminsw1, 
        MOTOR_CONSTS.altmaxsw2,
        MOTOR_CONSTS.altdegmin,
        MOTOR_CONSTS.altdegmax,
        MOTOR_CONSTS.altSPR,
        MOTOR_CONSTS.altRatio
    );

    /**
     * azimuth motor config
     */
    initMotorCstm(
        &(motorInst->azimuth),
        MOTOR_CONSTS.aziStep,
        MOTOR_CONSTS.aziDir,
        MOTOR_CONSTS.aziMF,
        MOTOR_CONSTS.aziminsw1,
        MOTOR_CONSTS.azimaxsw2,
        MOTOR_CONSTS.azidegmin,
        MOTOR_CONSTS.azidegmax,
        MOTOR_CONSTS.aziSPR,
        MOTOR_CONSTS.aziRatio
    );

    /**
     *  mirror motor config
     */
    initMotorCstm(
        &(motorInst->mirror),
        MOTOR_CONSTS.mirrorStep,
        MOTOR_CONSTS.mirrorDir,
        MOTOR_CONSTS.mirrorMF,
        MOTOR_CONSTS.mirrorminsw1,
        MOTOR_CONSTS.mirrormaxsw2,
        MOTOR_CONSTS.mirrordegmin,
        MOTOR_CONSTS.mirrordegmax,
        MOTOR_CONSTS.mirrorSPR,
        MOTOR_CONSTS.mirrorRatio
    );
    _pinh(MOTOR_CONSTS.mirrorMicroStep1); //set microstep behavior
    _pinh(MOTOR_CONSTS.mirrorMicroStep2); //set microstep behavior

    motorInst->comms = comms;

    motorInst->altStack = (uint8_t *) calloc(1028, sizeof(uint8_t));
    motorInst->azStack = (uint8_t *) calloc(1028, sizeof(uint8_t));
    motorInst->mirrorStack = (uint8_t *) calloc(1028, sizeof(uint8_t));

    regRxPacket(comms, MOTO_ID, motorInst->commands, &motorRxHandler);

    return motorInst;
}


// int runMotors() {
//     motorInst->altRunning = true;
//     motorInst->azRunning = true;
//     motorInst->mirrorRunning = true;

//     while(true) {
//         //if motocmd
//         //dequeue motor command off queue
//         //parse motor ID 
//         //update motor target
//         //else
//         //move some steps toward target
//     }
// }
    
//     motorCommand cmd = {0};

//     int count = 0;
    
//     while(true) {
//         cmd.cmd = -1;
//         int id = rxCheck(&cmd);
//         if(id != -1) {
//             //printf("MOTORID %d\n", id);
//         }
        
//         switch(id) {
//             case ALL_CMD :
//                 switch (cmd.cmd) {
//                 case GETPOS:
//                     // TODO: implement returning all positions
//                     break;
//                 default:
//                     break;
//                 }
//                 break;
//             case ALT_ID :
//                 printf("alt %d, %f\n\n", cmd.cmd, cmd.degree);
//                 //printf("alt commands: %d\n", ++count);
//                 dqEnqueue(altCommand, &cmd); // thread lock handled by size var
//                 break;
//             case AZ_ID :
//                 dqEnqueue(azCommand, &cmd);
//                 break;
//             case MIR_ID :
//                 dqEnqueue(mirrorCommand, &cmd);
//                 break;
//             case -1: break;
//             default : break;
//         }
//     }
// }

/**
 * check the rx buffer for recieved commands and load them into ret.
 * 
 * @return the integer motor ID the command was adressed to.
 */
// int rxCheck(motorCommand* ret) {
//     if(motorInst->rxBuff->threadLock == true) { //wait for full message to be dispatched
//         return -1;
//     }
//     //motorInst->rxBuff->threadLock = true; //enter buffer to read

//     char read[64] = {0};
//     int curr = 1;
//     read[0] = '$';
//     //printf("%d",motorInst->rxBuff->size);
//     if(motorInst->rxBuff->size <= 0) {
//         return -1;
//     }
//     //printf("%d", motorInst->rxBuff->size);
//     while(motorInst->rxBuff->size > 0) { //read until terminator
//         while(motorInst->rxBuff->threadLock == true) {
//             _waitus(5); // wait and do nothing
//         }
//         dqDequeueCharDeque(motorInst->rxBuff, &read[curr]);
//         if(read[curr-1] == '\n' && read[curr] == '\0') {
//             break;
//         }
//         curr++;
//     }//printf("arrived here\n");
    

//     //motorInst->rxBuff->threadLock = false; //done reading

//     printf(read);
//     printf("\n");
//     //printf("%d\n", motorInst->rxBuff->size);

//     int motoID = atoi(&read[1]);
//     //printf("MOTORID %d", motoID);
//     ret->cmd = -1;
//     int param = 0;
    
    
//     for(int i = 0; i < curr; i++) {
//         if(read[i] == ',') { // read until we see a delimiter, then read 
//                                 //  the next parameter (delimiter will always be
//                                 //  followed by parameter)
//             i++;
            
//             switch (ret->cmd) {
//                 case -1: //parse command
//                     ret->cmd = atoi(&read[i]);
//                     if( ret->cmd == HOME ||
//                         ret->cmd == INIT ||
//                         ret->cmd == GETPOS) { //if command has no params return
//                         return motoID;
//                     }
//                     break;
//                 case DEG: //parse params for DEG command
//                     ret->degree = atof(&read[i]);
//                     ret->direction = 1;
//                     return motoID;
//                 case MSTEP: //parse params for STEP command
//                     ret->step = atoi(&read[i]);
//                     ret ->direction = 1;
//                     return motoID;
//                 case STEPDIR: //parse params for STEP DIRECTION command
//                     if(param == 0) {
//                         ret->step = atoi(&read[i]);
//                         param++;
//                     }
//                     else{
//                         ret->direction = atoi(&read[i]);
//                         return motoID;
//                     }
//                     break;
//                 case DEGDIR: //parse params for DEGREE DIRECTION command
//                     if(param == 0) {
//                         ret->step = atof(&read[i]);
//                         param++;
//                     }
//                     else{
//                         ret->direction = atoi(&read[i]);
//                         return motoID;
//                     }
//                     break;
//                 case HOME:
//                     return motoID;
//                 case INIT:
//                     return motoID;
//                 default:
//                     break;
//             }
//         }
//     }
//     return motoID;
// }

int runMotorold(Motor_t *motor, uint8_t *motRunning, Deque *motCommand) {
    
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

int runMotors() {
    motorCommand cmd = {0};

    // uint8_t altStack[8192];
    // uint8_t azStack[8192];
    // uint8_t mirStack[8192];

    // __builtin_cogstart(runMotor(&(motorInst->altitude)), altStack);
    // __builtin_cogstart(runMotor(&(motorInst->azimuth)), azStack);
    // __builtin_cogstart(runMotor(&(motorInst->mirror)), mirStack);

    
    int itrs = 0;
    while(true) {
        cmd.motoId = -1;

        if(motorInst->commands->size > 0) {
            dqDequeue(motorInst->commands, &cmd);
        }


        //#define all_test
        #ifdef all_test
            if(itrs == 0) {
                cmd.motoId = ALT_ID;
                cmd.cmd = DEGDIR;
                cmd.degree = 3.0;
                cmd.direction = 1;
                itrs++;
                //_waitms(1000);
            }
            
        #endif
        
        switch(cmd.motoId) {
            case ALL_CMD:   executeMotocmd(&motorInst->azimuth, &cmd);
                            executeMotocmd(&motorInst->altitude, &cmd);
                            executeMotocmd(&motorInst->mirror, &cmd);
            break;
            case AZ_ID: executeMotocmd(&motorInst->azimuth, &cmd);
            break;
            case ALT_ID: executeMotocmd(&motorInst->altitude, &cmd);
            break;
            case MIR_ID: executeMotocmd(&motorInst->mirror, &cmd);
            break;
            case -1://do nothing, no command enqueued;
            default:;
            break;
        }

        move_toward_target_deg(&(motorInst->altitude), 1);
        move_toward_target_deg(&(motorInst->azimuth), 1);
        move_toward_target_deg(&(motorInst->mirror), 1);

    }
}

int runMotor(Motor_t *motor) {
    while(true) {
        move_toward_target_deg(motor, 20);
    }
}

void executeMotocmd(Motor_t *motor, motorCommand *cmd) {
    switch(cmd->cmd) {
        // case DEG: readDEGstr(&cmd, input, &curr, length);
        // break;
        // case MSTEP: readMSTEPstr(&cmd, input, &curr, length);
        // break;
        // case STEPDIR: readSTEPDIRstr(&cmd, input, &curr, length);
        // break;
        case HOME: rehome(motor);
        break;
        case DEGDIR: update_target_deg(motor, cmd->degree*cmd->direction);
        break;
        case -1: printf("no command sent!");
        default:
        break;
    }
}

uint8_t motorRxHandler(Deque* notUsed, uint8_t* input, uint16_t length) {
    motorCommand cmd = {0};
    cmd.cmd = -1;

    uint16_t curr = 0;

    curr = scanForParam(input, curr, length);
    cmd.motoId = atoi(&input[++curr]);

    curr = scanForParam(input, curr, length);
    cmd.cmd = atoi(&input[++curr]);

    switch(cmd.cmd) {
        // case DEG: readDEGstr(&cmd, input, &curr, length);
        // break;
        // case MSTEP: readMSTEPstr(&cmd, input, &curr, length);
        // break;
        // case STEPDIR: readSTEPDIRstr(&cmd, input, &curr, length);
        // break;
        case HOME: //do nothing;
        break;
        case DEGDIR: readDEGDIRstr(&cmd, input, &curr, length);
        break;
        case -1: printf("no command sent!");
        default:
        break;
    }

    dqEnqueue(motorInst->commands, &cmd);
}

void readDEGstr(motorCommand *ret, uint8_t* input, uint16_t *curr, uint16_t length) {
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->degree = atof(&input[*curr]);
    ret->direction = 1;
}

void readMSTEPstr(motorCommand *ret, uint8_t* input, uint16_t *curr, uint16_t length) {
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->step = atoi(&input[*curr]);
    ret->direction = 1;
}

void readSTEPDIRstr(motorCommand *ret, uint8_t* input, uint16_t *curr, uint16_t length) {
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->step = atoi(&input[*curr]);
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->direction = atoi(&input[*curr]);
}

void readDEGDIRstr(motorCommand *ret, uint8_t* input, uint16_t *curr, uint16_t length) {
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->step = atof(&input[*curr]);
    *curr = scanForParam(input, *curr, length);
    *curr++;
    ret->direction = atoi(&input[*curr]);
}

uint16_t scanForParam(uint8_t* input, uint16_t curr, uint16_t length) {
    for(uint16_t i = curr; i < length; i++) {
        if (input[i] == '$') {
            return i;
        }
    }
}
