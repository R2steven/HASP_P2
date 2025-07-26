#include "config.h"
#include "motors.h"
#include "src/comms.c"
#include "src/motors.c"

int main() {
    //set propeller clock
    _clkset(_SETFREQ, _CLOCKFREQ); 

    HASP25_Comms *comms = initComms();

    HASP25Motors_t *motors = initMotors(comms);

    uint8_t motoStack[8192];
    __builtin_cogstart(runMotors(), motoStack);
}
