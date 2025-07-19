#include <propeller.h>

#include "HASP_25_P2.h"

// we only need to support P2, 
#ifndef __P2__
#error this demo is for P2 only 
#endif

struct __using("spin/jm_fullduplexserial.spin2") Pi_uart;

typedef int (*operation_func)(int, int);

int main() {
    Pi_uart.start(0,1,0,9600);

    char str[] = "Hello World From Propeller!";
    long args[] = {0};

    while(1) {
        _pinnot(56);
        Pi_uart.fstr0(str);
        _waitms(500);
    }
}
