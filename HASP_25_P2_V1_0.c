#include <propeller.h>
#include <stdio.h>

#include "HASP_25_P2.h"

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

struct __using("spin/jm_fullduplexserial.spin2") Pi_uart; //TODO: break out into comms module

//TODO: split these into headers to hide common method names hopefully
#include "src/drivers/photodiodes.c" //TODO: make a header with a runSteering() method or something

#include "src/drivers/thermistors.c" //TODO: make a header with a runTemp() method or simething

#include "motors.h"//TODO: modify home() function to use limit switches. add gear ration functionality

//struct __using("spin/jm_i2c.spin2") solar1;

//struct __using("spin/jm_i2c.spin2") solar2;

//struct __using("spin/jm_i2c.spin2") MPU;

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    Pi_uart.start(1,0,0,230400);

    therm_start();
    photoDiode_start();

    char str[100] = {0};
    char recieved[100] = {0};
    int buff[20] = {0};

    _pinh(0);

    unsigned char stack[128];
    
    __builtin_cogstart(runMotors(), stack);

    for(int i = 0; i < 10000; i++) {

        _pinnot(56);
        Pi_uart.fstr0("Hello World From Propeller!\n");
        getTempMessage(str, 200);
        pollDiode(buff);
        readuart(recieved,Pi_uart.available());
        printf(recieved);
        _waitms(500);
        printf("prop main loop!\n");
    }
}

void readuart(char *buff, int len) {
    for(int i = 0; i < len; i++) {
        buff[i] = Pi_uart.rxcheck();
    }
}
