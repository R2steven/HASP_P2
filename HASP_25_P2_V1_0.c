#include <propeller.h>
#include <stdio.h>

#include "config.h"

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

//TODO: split these into headers to hide common method names hopefully
#include "src/drivers/photodiodes.c" //TODO: make a header with a runSteering() method or something

#include "src/drivers/thermistors.c" //TODO: make a header with a runTemp() method or simething

#include "motors.h"//TODO: modify home() function to use limit switches. add gear ration functionality
#include "src/motors.c"

#include "src/comms.c"
//struct __using("spin/jm_i2c.spin2") solar1;

//struct __using("spin/jm_i2c.spin2") solar2;

//struct __using("spin/jm_i2c.spin2") MPU;



int main() {
    //set propeller clock
    _clkset(_SETFREQ, _CLOCKFREQ);

    //initialize Comms
    HASP25_Comms *comms = initComms();

    //initialize and run Motors
    HASP25Motors_t *motors = initMotors(comms);

    photoDiode_start();
    therm_start();


    //run modules
    uint8_t motoStack[8192];
    __builtin_cogstart(runMotors(), motoStack);

    uint8_t sensorStack[8192];

    //this cog becomes comms thread
    runComms();
}

#define sensorLen 210
int runSensors(HASP25_Comms *comms) {
    while(true) {
        enqueTXPacket(comms, sensorLen, SENSORS, &SensorTXPHandler);
        _waitms(50);
    }
    return 1;
}

uint16_t SensorTXPHandler(uint8_t *txBuff) {
    getTempMessage(txBuff, 100);
    //getSteeringMessage(txBuff+101, 100);
    return sensorLen;
}
