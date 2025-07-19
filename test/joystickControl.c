#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "motors.h"
#include "src/motors.c"


struct __using("spin/joystick.spin2")jystick;

int main() {
    HASP25Motors_t *motors = initMotors();

    uint8_t altRunning = true;
    uint8_t azRunning = true;
    uint8_t mirrorRunning = true;
    uint8_t altStack[1028] = {0};
    uint8_t azStack[1028] = {0};
    uint8_t mirrorStack[1028] = {0};

    Deque *altCommand = initDeque(20,sizeof(motorCommand));
    Deque *azCommand = initDeque(20,sizeof(motorCommand));
    Deque *mirrorCommand = initDeque(20,sizeof(motorCommand));


    __builtin_cogstart(runMotor(&motors->altitude,&altRunning, altCommand), altStack);
    __builtin_cogstart(runMotor(&motors->azimuth, &azRunning, azCommand), azStack);
    __builtin_cogstart(runMotor(&motors->mirror, &mirrorRunning, mirrorCommand), mirrorStack);

    int rate = 100;
    int timeout = (int)(1.0/rate * 1000000);
    //printf("timeout = %d\n", timeout);

    jystick.start(8, timeout, 0, 2000);

    float AzDeg = 0;
    float AltDeg = 0;
    int8_t sw = 0;
    float mirDeg = 0;

    float AzDegPerPoll = 2/100.0; // 2 deg a second
    float AltDegPerPoll = 2/100.00; // 2 deg a second
    float mirDegPerPoll = 2/500.00; // 1 deg a second

    motorCommand cmd = {0};

    int swTimeout = 0;
    int rawalt = 0;
    int rawaz = 0;
    int rawSw = 0;

    int azPrint = 0;
    int altPrint = 0;
    int mirPrint = 0;
    int degPrint = 0;

    uint32_t start_time_ms;
    uint32_t end_time_ms;

    while(1) {
        jystick.poll();

        degPrint++;

        //printf("jystick X: %d\njystick y: %d\njystick sw: %d\n\n",jystick.getX(), jystick.getY(), jystick.getSW());

        //printf("poll time = %d\n", end_time_ms - start_time_ms);

        if(sw == 0) {
            rawalt = jystick.getY();
            rawaz = jystick.getX();

            AzDeg = ((rawaz - jystick.getXMed()) / 1000.0) * AzDegPerPoll;
            AltDeg = ((rawalt - jystick.getYMed()) / 1000.0) * AltDegPerPoll;

            if(degPrint >= 99) {
                //printf("jystick X: %d\njystick y: %d\njystick sw: %d\n\n",jystick.getX(), jystick.getY(), jystick.getSW());
                //printf("azDeg: %f\naltDeg: %f\n\n",AzDeg, AltDeg);
                degPrint = 0;
            }

            if(swTimeout >= 99 && jystick.getSW() != 0) {
                sw = 1;
                swTimeout = 0;
            }
            else {
                if(swTimeout < 99) {
                    swTimeout++;
                }
            }

            if(AzDeg >= AzDegPerPoll*.001 || AzDeg <= -AzDegPerPoll*.001) {
                cmd.cmd = DEG;
                cmd.degree = AzDeg;
                dqEnqueue(azCommand, &cmd);
                if(azPrint >= 99) {
                    azPrint = 0;
                    printf("Az step: %f\n", AzDeg);
                }
                cmd.degree = 0;
                azPrint++;
            }

            if(AltDeg >= AltDegPerPoll*.001 || AltDeg <= -AltDegPerPoll*.001) {
                cmd.cmd = DEG;
                cmd.degree = AltDeg;
                dqEnqueue(altCommand, &cmd);
                if(altPrint >= 99) {
                    altPrint = 0;
                    printf("Alt Step: %f\n", AltDeg);
                }
                cmd.degree = 0;
                altPrint++;
            }
        }
        else {
            mirDeg = ((jystick.getY()) / 1000.0) * mirDegPerPoll;
            
            if(swTimeout >= 99 && jystick.getSW() != 0) {
                sw = 0;
                swTimeout = 0;
            }
            else {
                if(swTimeout < 99) {
                    swTimeout++;
                }
            }

            if(mirDeg >= mirDegPerPoll*.001 || mirDeg <= -mirDegPerPoll*.001) {
                cmd.cmd = DEG;
                cmd.degree = mirDeg;
                dqEnqueue(altCommand, &cmd);
                if(mirPrint >= 99) {
                    mirPrint = 0;
                    printf("Alt Step: %f\n", mirDeg);
                }
                cmd.degree = 0;
                mirPrint++;
            }
        }
    }

}
