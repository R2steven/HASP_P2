/**
 * Author: Ryan Stevens
 * 
 * HASP25 SpectraSolis propeller comms module. Handles 
 */

#ifndef HASP25_COMMS_MODULE
#define HASP25_COMMS_MODULE

#include "config.h"
#include "charDeque.h"
#include "deque.h"
#include "hashMap.h"
#include "CRC.h"


typedef uint8_t (*rxPHandler)(Deque*, char*, uint16_t);

typedef struct HASP25_Header_s {
    uint8_t S;
    uint8_t P;
    int32_t PID;
    uint8_t type;
    uint16_t crc;
    uint8_t length;
    uint8_t not_used;
}HASP25_Header;

typedef struct Comms_s {
    uint8_t RXPin;
    uint8_t TXPin;
    uint32_t baud;
    HashMap rxHandles;
    Deque txBuff;
    HASP25_Header *header;
    uint8_t *buffer;
    CRC16 crc;
}HASP25_Comms;

typedef struct CommsPacketHandler_s {
    uint32_t key;
    rxPHandler packetHandler;
    Deque *buff;
}CommsPacketHandler;

void regRxPacket(   HASP25_Comms* comms,
                    uint32_t key, 
                    Deque *output, 
                    rxPHandler packetHandler
                ) __fromfile("src/comms.c");

void regTxPacket(   HASP25_Comms* comms,
                    uint32_t key, 
                    Deque *input, 
                    rxPHandler CommsPacketHandler
                ) __fromfile("src/comms.c");

int runComms() __fromfile("src/comms.c");

HASP25_Comms *initComms() __fromfile("src/comms.c");

void rxCheck(HASP25_Comms *comms) __fromfile("src/comms.c");

void txCheck() __fromfile("src/comms.c");

#endif
