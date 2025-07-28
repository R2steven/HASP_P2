
#include <propeller.h>
#include <stdio.h>
#include "config.h"

int main() {
    _dirh(35);
    _pinl(35);

    while(true) {

        printf("donothing");
        _waitms(1000);
    }
}
