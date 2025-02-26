#include <stdio.h>
#include <propeller.h>

#include "HASP_25_P2.h"

#include "test.h"

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

uint32_t _txmode = 0b00000000000000000000000001111100; //async tx mode, output enabled for smart output

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    if(square(2)==4)printf("%d", 4);

    //8 bit, 1 Step Per word, 3 us per word, .3 us per bit
    /**uint32_t bitperiod = (_CLOCKFREQ / 3333333);
      // calculate smartpin mode for 8 bits per character
    uint32_t bit_mode = 7 + (bitperiod << 16);

    _pinstart(0,_txmode,bit_mode, 0);

    _pinh(1);

    _pinl(2);

    for(;;) {

        //uart starts high, 0 low, 3.3 high, little endian, start bit low, stop bit high
        _wypin(0,0b11110000);//uart start bit low, end bit high
        //_waitus(100);
    }*/

    //16 bit, 6 step per word, 18us per word, 1us per bit
    uint32_t bitperiod = (_CLOCKFREQ / 1000000);
      // calculate smartpin mode for 16 bits per character
    uint32_t bit_mode = 15 + (bitperiod << 16);

    _pinstart(0,_txmode,bit_mode, 0);

    _pinh(1);

    _pinl(2);

    for(;;) {

        //uart starts high, 0 low, 3.3 high, little endian, start bit low, stop bit high
        _wypin(0,0b1011011011011011);//uart start bit low, end bit high

        #define SIX_STEP_OVERDRIVE  high 0b1011011011011011 low
        #define FOUR_STEP_DRIVE 0b1001100110110110

        // example does not 0b1111101111

        //_waitus(100);
    }
}

void something(){
  main();
}
