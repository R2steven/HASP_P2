#include "config.h"
#include "motors.h"
#include "src/motors.c"

#define DEBUG
#define TEST_RUNMOTORS

#ifdef TEST_RXCHECK
int main() {
    Motor_consts *motors = initMotors();

    char commandALT[45] = {0};
    sprintf(commandALT,"%d, %d, %.4f\n\0",
            ALT_ID, DEG, 90.0
        );

    char commandAZ[45] = {0};
    sprintf(commandAZ,"%d, %d, %.4f\n\0",
            AZ_ID, DEG, 45.0
        );
    char commandMIR[45] = {0};
    sprintf(commandMIR,"%d, %d, %.4f\n\0",
            MIR_ID, DEG, 180.0
        );

    
    int itrs = 1;

    unsigned char motoStack[8192];
    __builtin_cogstart(runRxCheck(), motoStack);

    while(true) {
        _pinnot(56);
        //printf("%d\n",itrs);
        motors->threadLock = true;
        for(int i = 0; i < 17; i++) {
            dqEnqueueCharDeque(motors, commandALT[i]);
        }
        for(int i = 0; i < 17; i++) {
            dqEnqueueCharDeque(motors, commandAZ[i]);
        }
        for(int i = 0; i < 18; i++) {
            dqEnqueueCharDeque(motors, commandMIR[i]);
        }
        motors->threadLock = false;
        _waitms(1000);
        itrs++;
    }
}

void runRxCheck() {
    motorCommand cmd = {0};
    while (true)
    {
        rxCheck(rxCheck(&cmd));
    }
    
}
#endif

#ifdef TEST_RUNMOTORS
int main() {
    
    HASP25Motors_t *motors = initMotors();

    unsigned char motoStack[16384];

    __builtin_cogstart(runMotors(), motoStack);

    char commandALT[45] = {0};
    sprintf(commandALT,"%d, %d, %.4f\n\0",
            ALT_ID, DEG, 90.0
        );

    char commandAZ[45] = {0};
    sprintf(commandAZ,"%d, %d, %.4f\n\0",
            AZ_ID, DEG, 45.0
        );
    char commandMIR[45] = {0};
    sprintf(commandMIR,"%d, %d, %.4f\n\0",
            MIR_ID, DEG, 180.0
        );
    
    _pinh(56);
    _waitms(1000);

    int itrs = 1;
    while(true) {
        _pinnot(56);
        printf("%d\n",itrs);
        motors->rxBuff->threadLock = true;
        for(int i = 0; i < 17; i++) {
            dqEnqueueCharDeque(motors, commandALT[i]);
        }
        for(int i = 0; i < 17; i++) {
            dqEnqueueCharDeque(motors, commandAZ[i]);
        }
        for(int i = 0; i < 18; i++) {
            dqEnqueueCharDeque(motors, commandMIR[i]);
        }
        motors->rxBuff->threadLock = false;
        _waitms(1000);
        itrs++;
    }
}
#endif
