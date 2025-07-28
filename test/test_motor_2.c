#include "config.h"
#include "motors.h"
#include "src/comms.c"
#include "src/motors.c"

int main() {
    //set propeller clock
    _clkset(_SETFREQ, _CLOCKFREQ); 

    HASP25_Comms *comms = initComms();

    HASP25Motors_t *motors = initMotors(comms);

    // runMotors();
    uint8_t CommsStack[65536];
    __builtin_cogstart(runComms(), CommsStack);
    // while(true) {
    //     _waitms(500);
    // }
}

int testRunMotors(HASP25Motors_t *motors) {
    while(true) {
        step(&(motors->altitude), -40000);
        step(&(motors->altitude), 40000);
    }
}


// int main() {
//     while(true) {
//         _pinl(36);
//         _pinh(35);
//         //_waitms(1);

//         _pinh(36);
//         _pinl(35);
//         //_waitms(1);
//     }
// }
