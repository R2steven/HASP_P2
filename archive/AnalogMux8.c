#include <propeller.h>

#include "HASP_25_P2.h"

#define DEBUG

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

#ifdef DEBUG
    #include <stdio.h>
#endif

/**
 * flexprop specific goofyness that c syntax doesn't like.
 * 
 * This line will import the ez_analog library for use in this file
 */  
typedef struct __using("spin/jm_ez_analog.spin2") ez_analog;

typedef struct rdAnalogMux8 {
    int8_t pin_A;
    int8_t pin_B;
    int8_t pin_C;
    int8_t pin_signal;
    int8_t pin_INH;
    int readings[8];
    ez_analog *analog;

    void select(uint8_t inputSel) {
        _pinw(pin_A, (inputSel & 0x01)); //mask off bit and write to pin
        _pinw(pin_B, (inputSel & 0x02) >> 1);
        _pinw(pin_C, (inputSel & 0x04) >> 2);
        if(pin_INH > 3) {
            _pinw(pin_INH, 0);
        }
        _waitus(250); //let opAmp output stablize
    }

    /**
     * creates and starts a 
     */
    void init(uint8_t npin_A, uint8_t useINH) {
        pin_A = npin_A; 
        pin_B = npin_A + 1;
        pin_C = npin_A + 2;
        pin_signal = npin_A + 3;
        //if inhibit pin is in use, set it. Else set it to 0 as boolean flag.
        if(useINH > 0) {
            pin_INH = pin_A + 4;
        }
        else {
            pin_INH = 0;
        }
        //start the analog pin.
        analog->start(pin_signal, 0, 3300);
    }

    int read(uint8_t inputSel) {
        if(inputSel > 7) {
            #ifdef DEBUG
                printf("rdAnalogMux8.read input out of range! got %d > 7",inputSel);
            #endif
            return 0;
        }
        select(inputSel);
        readings[inputSel] = analog->read();
        return readings[inputSel];
    }

    void poll() {
        for(int i = 0; i < 8; i++) {
            //digital write to A,B,C pins
            select(i);
            readings[i] = analog->read();
        }
    }

    void Mux8print() {
        printf("diodeArray:\n\tpin_A: %d,\n\tpin_B: %d,\n\tpin_C: %d,\n\tpin_signal: %d,\n\tpin_INH: %d,\n",pin_A,pin_B,pin_C,pin_signal,pin_INH);
    }

    int* getReadBuff() {
        return readings;
    }
}rdAnalogMux8;



/**
 * creates and starts a 
 *
rdAnalogMux8 *init(uint8_t pin_A, uint8_t useINH) {
    rdAnalogMux8 *ret = (rdAnalogMux8*) malloc(sizeof(rdAnalogMux8));
    ret->pin_A = pin_A;
    ret->pin_B = pin_A + 1;
    ret->pin_C = pin_A + 2;
    ret->pin_signal = pin_A +3;
    //if inhibit pin is in use, set it. Else set it to 0 as boolean flag.
    if(useINH > 0) {
        ret->pin_INH = pin_A + 4;
    }
    else {
        ret->pin_INH = 0;
    }
    //start the analog pin.
    ret->analog.start(pin_signal, 0, 3300);
    return ret;
}**/

int main() {

    int pin_A = 0;

    rdAnalogMux8 diodeArray;

    printf("preInit:\n");
    diodeArray.Mux8print();
    printf("\n");

    diodeArray.init(pin_A, 1);

    printf("postInit:\n");
    diodeArray.Mux8print();
    

    /**Counter x;

    printf("Counter x: %d\n", x.val);
    printf("Counter x: %d\n", x.getval());

    x.setval(10);

    printf("Counter x: %d\n", x.val);
    printf("Counter x: %d\n", x.getval());*/

    _pinl(3);
    _pinl(5);
    _pinh(6);



    int *readings = diodeArray.getReadBuff();
    for(;;) {
        _pinnot(3);
        diodeArray.poll();
        printf("measured voltages:\n");
        for(int i = 0; i < 8; i++) {
            printf("Diode %d: %d mV\n", i, readings[i]);
        }
        printf("\n\n");
        _waitms(1000);
    }
}
