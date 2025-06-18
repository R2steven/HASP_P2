#include <stdio.h>
#include <propeller.h>

#include "HASP_25_P2.h"

#include "test.h"

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

#define STEP                    0b1111111111111110
#define TWO_STEP                0b1111110011111110
#define THREE_STEP              0b1110011110011110
#define FOUR_STEP               0b1100111001100110
#define SIX_STEP_OVERDRIVE      0b1011011011011011

uint32_t _txmode = 0b00000000000000000000000001111100; //async tx mode, output enabled for smart output

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);

    //ser.start();
    //ser.tx("Hi");

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

    _pinstart(0,_txmode,bit_mode,0);

    _pinh(1);

    _pinl(2); 

    _pinstart(4,_txmode,bit_mode,0);

    _pinh(5);

    _pinl(6);

    _pinstart(8,_txmode,bit_mode,0);

    _pinh(9);

    _pinl(10);

    _pinstart(12,_txmode,bit_mode,0);

    _pinh(13);

    _pinl(14);

    int blink = 0;

    for(;;) {
        //uart starts high, 0 low, 3.3 high, little endian, start bit low, stop bit high
        _wypin(0,FOUR_STEP);//uart start bit low, end bit high

        _wypin(4,0b1000110001100110);//uart start bit low, end bit high

        _wypin(8,0b0111001110011001);//uart start bit low, end bit high

        _wypin(12,0b0111001110011001);//uart start bit low, end bit high

        blink++;

        if(blink == 1000000) {
            printf("Running!");
            _pinnot(56);
            blink = 0;
        }

        // example does not 0b1111101111

        //_waitus(100);
        //printf("Running!");
    }
}
