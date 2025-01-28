#include <stdio.h>


#include "HASP_25_P2.h"

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

/**
 * flexprop specific goofyness that c syntax doesn't like.
 * 
 * This line will import the SmartSerial.spin library for use in this file
 */  
struct __using("spin/SmartSerial") ser; 

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    ser.start(63, 62, 0, _BAUD);
    
    for(;;) {
        printf("Hello World!\n");
        if(ser.rxcheck()!=-1) {
            printf(ser.rx());
        }
        _waitx(40000000);
    }
    
}
