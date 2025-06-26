// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

#include <stdint.h>

#include "config.h"

// SIX_STEP_OVERDRIVE  high 0b1011011011011011 low
// FOUR_STEP_DRIVE 0b1001100110110110

#define STEP                    0b1000000000000000
#define TWO_STEP                0b1000001100000000
#define THREE_STEP              0b1000011000011000
#define FOUR_STEP               0b1000110001100110 
#define FIVE_STEP_OVERDRIVE     0b1001100110010010
#define SIX_STEP_OVERDRIVE      0b1011011011011011


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


    uint16_t base_pin;
    uint16_t tx_pin = base_pin;
    uint16_t dir_pin = base_pin+1;
    uint16_t MF_pin = base_pin+2;


    for(;;) {

        //uart starts high, 0 low, 3.3 high, little endian, start bit low, stop bit high
        _wypin( tx_pin,0b1011011011011011);//uart start bit low, end bit high



        // example does not 0b1111101111

        //_waitus(100);
    }
}