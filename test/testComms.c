#include <string.h>

//#include "comms.h"
#include "src/comms.c"
#include "stdio.h"

uint8_t *message = "Hello World From Prop";

int main() {
    HASP25_Comms *comms = initComms();
    
    Deque * notUsed = {0};

    //p2comms.fstr0("Hello from propeller!");

    //while(p2comms.available() > 0) {
    //    printf(p2comms.rxCheck());
    //}
    //printf("\n");

    regRxPacket(comms, 1, notUsed, &prxhandler);

    
    HASP25_Header_buff stdhead = {0};

    stdhead.hdr.S = 'S';
    stdhead.hdr.P = 'P';
    stdhead.hdr.PID = (uint32_t) 0xffffffff;
    stdhead.hdr.type = (uint8_t) 1;
    stdhead.hdr.length = (uint8_t) 2;
    stdhead.hdr.not_used = (uint8_t) 0;
    stdhead.hdr.buff[0] = (uint8_t) 0x51;

    stdhead.hdr.crc = compute_CRC16(&comms->crc, message, stdhead.hdr.length);

    printf("sent:\n");
    printf("indexed:\nstart: %c%c, PID: 0x%x%x%x%x, type: 0x%x, CRC: 0x%x%x, length: 0x%x\n",
        stdhead.hdrBuff[0],
        stdhead.hdrBuff[1],
        stdhead.hdrBuff[4],
        stdhead.hdrBuff[5],
        stdhead.hdrBuff[6],
        stdhead.hdrBuff[7],
        stdhead.hdrBuff[8],
        stdhead.hdrBuff[10],
        stdhead.hdrBuff[11],
        stdhead.hdrBuff[12]
    );
    printf("stdheader:\nstart: %c%c, PID: %u, type: %u, CRC: 0x%x, length: %u\n",
        stdhead.hdr.S,
        stdhead.hdr.P,
        stdhead.hdr.PID,
        stdhead.hdr.type,
        stdhead.hdr.crc,
        stdhead.hdr.length
    );

    Deque notused = {0};

    regRxPacket(comms, 1, notUsed, &prxhandler);

    
    while(true) {
        enqueTXPacket(comms, 21, 1, &ptxhandler);
        printf("\n\n");
        txCheck(comms);

        _waitms(500);
        
        rxCheck(comms);
        printf("\nin waiting: %d\n", p2comms->availablep2());
    }
}

uint8_t prxhandler(Deque *notused, char *input, int length) {
    printf("Decoded: %s\n", &input[0]);
}

uint16_t ptxhandler(uint8_t *txbuff) {
    memcpy(txbuff, message, 22);
    return 22;
}
