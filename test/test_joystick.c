#include <stdio.h>
#include <propeller.h>

#include "HASP_25_P2.h"
#include "Joystick.c"


// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

int main() {

    uint32_t start_time_ms;
    uint32_t end_time_ms;
    printf("setting up Joystick!");
    
    start_time_ms = _getms();
    _waitms(1000);
    end_time_ms = _getms();
    printf("wait %d ms\n", end_time_ms-start_time_ms);
    

    Joystick *controller = {0};

    initJoystick(controller, 4, 0, 1000);

    printf("setup Joystick!\n");


    uint32_t wait_small = 200;
    uint32_t wait_large = 300;

    for(;;) {
        //printf("starting!\n");

        start_time_ms = _getms();
        pollJoystick(controller);
        end_time_ms = _getms();
        /*printf("(x,y,sw) -> (%d,%d,%d)\n in %d ms\n",
                controller->x,  
                controller->y, 
                controller->sw,
                end_time_ms-start_time_ms 
            );*/
        start_time_ms = _getms();
        _pinl(57);
        _waitms(wait_small);
        _pinh(57);
        _waitms(wait_large);
        end_time_ms = _getms();
        //printf("wait %d ms\n", end_time_ms-start_time_ms);
        if(controller->sw == 0) {
        break;
        }
    }

    start_time_ms = _getms();
    _waitms(1000);
    end_time_ms = _getms();
    printf("wait %d ms after break!\n", end_time_ms-start_time_ms);
}
