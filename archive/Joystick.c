#include <propeller.h>
#include <stdio.h>

#include "HASP_25_P2.h"

/**
 * flexprop specific goofyness that c syntax doesn't like.
 * 
 * This line will import the ez_analog library for use in this file
 */  
struct __using("spin/jm_ez_analog.spin2") pin_x;
struct __using("spin/jm_ez_analog.spin2") pin_y;
struct __using("spin/jm_ez_analog.spin2") pin_sw;


typedef struct joystick {
    uint8_t pin_start;
    int xmed;
    int ymed;
    int x;
    int y;
    int sw;
} Joystick;

/**
 * initializes a pollable joystic object starting at pin_start
 * and ending at pin_start+2
 * 
 * pin_x = pin_start
 * pin_y = pin_start+1
 * pin_sw = pin_start+2
 * */
void initJoystick(Joystick *joystick, uint8_t pin_first);

/**
 * poll the joystick position and update the given struct. Joystic valuse will
 * be centered on 0 and range between -500 and 500
 */
void pollJoystick(Joystick *joystick);

void initJoystick(Joystick *joystick, uint8_t pin_first, int lo, int hi) {
    joystick->pin_start = pin_first;

    pin_x.start(pin_first, lo, hi);
    pin_y.start(pin_first+1, lo, hi);
    pin_sw.start(pin_first+2, lo, hi);

    uint32_t start_time_ms;
    uint32_t end_time_ms;
    start_time_ms = _getms();
    _waitms(1000);
    end_time_ms = _getms();
    printf("wait %d ms\n", end_time_ms-start_time_ms);

    for(int i = 0; i < 10; i++) {
        joystick->x = pin_x.read();
        joystick->y = pin_y.read();

        start_time_ms = _getms();
        _waitx(160000000);
        end_time_ms = _getms();
        printf("wait %d ms start\n", end_time_ms-start_time_ms);

        joystick->sw = pin_sw.read();
        
        joystick->xmed = (joystick->xmed + joystick->x)/2;
        joystick->ymed = (joystick->ymed + joystick->y)/2;
        start_time_ms = _getms();
        _waitms(1000);
        end_time_ms = _getms();
        printf("wait %d ms\n", end_time_ms-start_time_ms);
    }
    start_time_ms = _getms();
    _waitx(100000000);
    end_time_ms = _getms();
    printf("wait %d x after setup\n", end_time_ms-start_time_ms);
    start_time_ms = _getms();
    _waitms(1000);
    end_time_ms = _getms();
    printf("wait %d ms\n", end_time_ms-start_time_ms);
}

void pollJoystick(Joystick *joystick) {
    //printf("pin x");
    joystick->x = pin_x.read() - joystick->xmed;
    //printf("pin y");
    joystick->y = pin_y.read() - joystick->ymed;
    //printf("pin sw");
    joystick->sw = pin_sw.read() > 0;
    //printf("(x,y,sw) -> (%d,%d,%d)\n",joystick->x, joystick->y, joystick->sw);
}

