#include <stdio.h>
#include "CRC.h"

#define TEST_CRC16
#ifdef TEST_CRC16

int main() {
    CRC16 crc = {0};
    initCRCNalloc(&crc);
    uint8_t input[2] = {0x51, 0x00};
    uint16_t final = 0x1373;

    CRC16 *test = &crc;

    if(test == &crc) {
        printf("true");
    }
    
    uint16_t out = compute_CRC16(&crc, input, 2);

    printf("Expected: 0x%x, Got: 0x%x", final, out);
}

#endif
