
/**
 * Author: Ryan Stevens 
 * restevens52@gmail.com
 * 
 * HASP25 SpectraSolis propeller comms module. Handles packetizing data strings
 * and all recieve and transmit functions.
 */

#ifndef HASP25_COMMS_MODULE
#define HASP25_COMMS_MODULE

#include "config.h"
#include "charDeque.h"
#include "deque.h"
#include "hashMap.h"
#include "CRC.h"


typedef uint8_t (*rxPHandler)(Deque*, uint8_t*, uint16_t); //returns sucess flag
typedef uint16_t (*txPHandler)(uint8_t*); //returns data length

#define HEADER_SIZE_BYTES 11
typedef struct HASP25_Header_s {
    uint8_t S;
    uint8_t P;
    uint32_t PID;
    uint8_t type;
    uint16_t crc;
    uint8_t length;
    uint8_t not_used;
    uint8_t buff[8]; //needed? not used just a buffer idk 
}HASP25_Header;

typedef union HASP25_Header_buff_u {
    HASP25_Header hdr;
    uint8_t hdrBuff[20];
}HASP25_Header_buff;

typedef struct Comms_s {
    uint8_t RXPin;
    uint8_t TXPin;
    uint8_t initialized;
    uint32_t baud;
    uint32_t currID;
    HashMap rxHandles;
    Deque txBuff;
    CRC16 crc;
    HASP25_Header_buff *header;
    uint8_t *buffer;
}HASP25_Comms;

typedef struct CommsRxPacketHandler_s {
    uint32_t key;
    rxPHandler packetHandler;
    Deque *buff;
}CommsRxPacketHandler;

typedef struct CommsTXPacketHandler_s {
    txPHandler phandler;
    int dataLength;
    uint8_t type;
}CommsTXPacketHandler;

void regRxPacket(   HASP25_Comms* comms,
                    uint32_t key, 
                    Deque *output, 
                    rxPHandler packetHandler
                );

void enqueTXPacket( HASP25_Comms* comms,
                    int dataLength,
                    uint8_t type,
                    txPHandler handler
                  );

int runComms();

HASP25_Comms *initComms();

void destroyComms();

int8_t rxCheck(HASP25_Comms *comms);

int8_t txCheck(HASP25_Comms *comms);

////////////////////////////////////////////////////////////////////////////////
// END comms.h HEADER
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// BEGIN comms.c IMPL
////////////////////////////////////////////////////////////////////////////////


//#include "comms.h"

//forward decl for p2 bindings
typedef void (*startf)(int, int, int, int); //int rxpin, int txpin, int mode, int baud
typedef uint8_t (*rxtixf)(int);//int tix
typedef void (*emitStrf)(uint8_t*, int);//char *str, int len
typedef void (*readStrf)(uint8_t*, int);//char *ret, int len
typedef int (*availablef)();

typedef struct p2comms_binding_s {
    startf startp2;
    rxtixf rxtixp2;
    emitStrf emitStrp2;
    readStrf readStrp2;
    availablef availablep2;
}p2comms_binding;


//global instances
HASP25_Comms *commsInst;
p2comms_binding *p2comms;

int runComms() {
    if(commsInst->initialized != true) {
        return -1;
    }

    while(true) {
        rxCheck(commsInst);
        txCheck(commsInst);
    }

    return 1;
}

HASP25_Comms *initComms() {

    if(commsInst->initialized == true) {
        destroyComms(commsInst);
        free(commsInst);
    }

    commsInst = (HASP25_Comms*) malloc(sizeof(HASP25_Comms)); //malloc

    //comms constants
    commsInst->RXPin = 1;
    commsInst->TXPin = 0;
    commsInst->baud = 230400;
    commsInst->currID = 0;
    
    //comms packet handlers (Tx queue and Rx map)
    initHashMapNalloc(&commsInst->rxHandles, 31, sizeof(CommsRxPacketHandler));//malloc
    initDequeNalloc(&commsInst->txBuff, 31, sizeof(CommsTXPacketHandler));//malloc

    //comms Buffers
    commsInst->header = (HASP25_Header_buff*) malloc(sizeof(HASP25_Header_buff));//malloc
    commsInst->buffer = (uint8_t*) calloc(256, sizeof(uint8_t));//malloc    

    //crc helper
    initCRCNalloc(&commsInst->crc);

    //////////////////////////////////////
    //init stupid shallow spin binding
    p2comms = (p2comms_binding*) malloc(sizeof(p2comms_binding));
    p2comms->startp2 = &startfdser;
    p2comms->rxtixp2 = &rxtixfdser;
    p2comms->emitStrp2 = &emitStr;
    p2comms->readStrp2 = &readStr;
    p2comms->availablep2 = &available;
    //////////////////////////////////////

    //start serial comms
    p2comms->startp2(commsInst->RXPin, commsInst->TXPin, 0, commsInst->baud);

    commsInst->initialized = true;

    return commsInst;
}

void destroyComms(HASP25_Comms* comms) {
    free(comms->buffer);
    free(comms->header);
    destroyDeque(&comms->txBuff);
    destroyHashMap(&comms->rxHandles);
}

void regRxPacket(   
    HASP25_Comms* comms,
    uint32_t key, 
    Deque *output, 
    rxPHandler packetHandler
    ) 
    {
        CommsRxPacketHandler handle = {0};
        handle.key = key;
        handle.buff = output;
        handle.packetHandler = packetHandler;

        insertHashMap(&comms->rxHandles, key, &handle);
    }

void enqueTXPacket( HASP25_Comms* comms,
                    int dataLength,
                    uint8_t type,
                    txPHandler handler
    ) 
    {
        CommsTXPacketHandler handle = {0};
        handle.phandler = handler;
        handle.type = type;
        handle.dataLength = dataLength;
        dqEnqueue(&comms->txBuff, &handle);
    }

/** packet format:
 * 
 * Byte#        Type        Value
 * 
 * 0            Char        'S' capital S
 * 1            Char        'P' capital P
 * 2-5          int32       Packet ID
 * 6            Byte        Packet Type
 * 7-8         int16       CRC16 checksum
 * 9           Byte        DataLength in Bytes
 * 10           Not Used    Not Used/ null char
 * 11-end       Data        Data
 */
int8_t rxCheck(HASP25_Comms *comms) {
    #define timeout 1000
    #define maxItrs 256

    //zero out header
    zeroHeader(comms);

    //local bind header
    HASP25_Header* currHeader = &(comms->header)->hdr;
    uint8_t *headerBuff = (comms->header)->hdrBuff;

    uint8_t validHeader = false;

    //initialize header start message
    currHeader->S = p2comms->rxtixp2(timeout);

    ///////////////////////////////////////////////////
    //align with valid start of header message
    ///////////////////////////////////////////////////
    for(int i = 0; i < maxItrs; i++) {
        currHeader->P = p2comms->rxtixp2(timeout);
        if(currHeader->S == 'S' && currHeader->P == 'P') {
            validHeader = true;
            break;
        }
        currHeader->S = currHeader->P;
    }
    //if valid header not found, return
    if(validHeader != true) {
        return -1;
    }

    ///////////////////////////////////////////////////
    //read in valid header
    ///////////////////////////////////////////////////

    unpackHeaderRXCheck(comms->header);

    //#define PRINT_HEADER
    #ifdef PRINT_HEADER
        printf("Print Header:\nraw bytes:\nstart: %c%c, PID: 0x%x%x%x%x, type: %x, CRC: 0x%x%x, length: %x, overFlow:0x%x%x%x%x\n",
            headerBuff[0],
            headerBuff[1],
            headerBuff[2],
            headerBuff[3],
            headerBuff[4],
            headerBuff[5],
            headerBuff[6],
            headerBuff[7],
            headerBuff[8],
            headerBuff[9],
            headerBuff[10],
            headerBuff[11],
            headerBuff[12],
            headerBuff[13],
            headerBuff[14]
        );
        printf("header:\nstart: %c%c, PID: %u, type: %u, CRC: 0x%u, length: %u\n",
            currHeader->S,
            currHeader->P,
            currHeader->PID,
            currHeader->type,
            currHeader->crc,
            currHeader->length
        );
    #endif

    ///////////////////////////////////////////////////
    //read in data packet
    ///////////////////////////////////////////////////
    
    for(int i = 0; i < currHeader->length; i++) {
        comms->buffer[i] = p2comms->rxtixp2(timeout);
    }
    comms->buffer[currHeader->length] = 0; //idk makes me feel better for crc
    comms->buffer[currHeader->length+1] = 0;//length must be less than 256-12 anyways

    ///////////////////////////////////////////////////
    //validate data packet crc checksum
    ///////////////////////////////////////////////////
    uint16_t compCRC = compute_CRC16(&comms->crc, &comms->buffer[0], currHeader->length);
    
    ///////////////////////////////////////////////////
    //dispatch valid data packet
    ///////////////////////////////////////////////////
    
    if(true) {//compCRC == currHeader->crc) { //TODO: use crc if time allows
        CommsRxPacketHandler phandler = {0};
        getHashMap(&comms->rxHandles, currHeader->type, &phandler);
        phandler.packetHandler(phandler.buff, &comms->buffer[0], currHeader->length);
        return 1;
    }
    return 0;
}

int8_t txCheck(HASP25_Comms *comms) {
   
    if(comms->txBuff.size <= 0) {
        return 0;
    }
    
    CommsTXPacketHandler handler = {0};

    dqDequeue(&comms->txBuff, &handler);

    //if size is greater than  256-11 multipacket (not needed)
    if(handler.dataLength > 256-12) {
        return -1;
    }
    
    uint16_t length = handler.phandler(&comms->buffer[0]);

    buildHeader(comms, &comms->buffer[0], length, handler.type);

    comms->buffer[length] = 0;
    
    uint8_t txhead[HEADER_SIZE_BYTES] = {0};
    

    packHeaderBytes(comms->header, txhead);

    #ifdef PRINT_HEADER_tx
    printf("header:\nstart: %c%c, PID: 0x%u, type: %u, CRC: 0x%u, length: %u\n",
            comms->header->hdr.S,
            comms->header->hdr.P,
            comms->header->hdr.PID,
            comms->header->hdr.type,
            comms->header->hdr.crc,
            comms->header->hdr.length
    );
    printf("%s", comms->buffer);
    #endif
    
    p2comms->emitStrp2(txhead,HEADER_SIZE_BYTES);
    p2comms->emitStrp2(&comms->buffer[0], length);
    return 1;
}

void packHeaderBytes(HASP25_Header_buff * header, uint8_t *txBuff) {
    // bytes 0-1 are S,P
    txBuff[0] = header->hdrBuff[0];
    txBuff[1] = header->hdrBuff[1];
    //bytes 2-5 are PID
    txBuff[2] = header->hdrBuff[4];
    txBuff[3] = header->hdrBuff[5];
    txBuff[4] = header->hdrBuff[6];
    txBuff[5] = header->hdrBuff[7];
    //byte 6 is type
    txBuff[6] = header->hdrBuff[8];
    //byte 7-8 is crc
    txBuff[7] = header->hdrBuff[10];
    txBuff[8] = header->hdrBuff[11];
    //byte 9 is length
    txBuff[9] = header->hdrBuff[12];
    //byte 10 is null/unused
    txBuff[10] = 0;
}

void unpackHeaderRXCheck(HASP25_Header_buff * header) {
    /** 
     * something fucky going on in the P2 here... SP are packed into bytes 1,2 as
     * expected, but then there are two zero bytes preceeding PID. its not an endian
     * thing bc otherwise there would be a zero byte between S and P. idk man. 
     * 
     * basically means I have to hand read in and out the transmitted bytes to 
     * endure specified packing.
     */
    // for(int i = 2; i < HEADER_SIZE_BYTES; i++) {  //this woulda been too easy :(
    //     headerBuff[i] = p2comms->rxtixp2(timeout);
    // }

    //bytes 0-1 decoded by rxcheck
    //bytes 4-7 are PID
    header->hdrBuff[4] = p2comms->rxtixp2(timeout);//2
    header->hdrBuff[5] = p2comms->rxtixp2(timeout);//3
    header->hdrBuff[6] = p2comms->rxtixp2(timeout);//4
    header->hdrBuff[7] = p2comms->rxtixp2(timeout);//5
    //byte 8 is type
    header->hdrBuff[8] = p2comms->rxtixp2(timeout);//6
    //byte 10-11 is crc
    header->hdrBuff[10] = p2comms->rxtixp2(timeout);//7
    header->hdrBuff[11] = p2comms->rxtixp2(timeout);//8
    //byte 12 is length
    header->hdrBuff[12] = p2comms->rxtixp2(timeout);//9
    //byte 13 is not used null terminator
    header->hdrBuff[13] = p2comms->rxtixp2(timeout);//10
}

void zeroHeader(HASP25_Comms *comms) {
    HASP25_Header *hdr = &(comms->header)->hdr;

    hdr->S = 0;
    hdr->P = 0;
    hdr->PID = 0;
    hdr->type = 0;
    hdr->crc = 0;
    hdr->length = 0;
}

void buildHeader(HASP25_Comms *comms, uint8_t *data, uint16_t length, uint8_t type) {
    zeroHeader(comms);

    HASP25_Header *hdr = &(comms->header)->hdr;

    hdr->S = 'S';
    hdr->P = 'P';
    hdr->PID = comms->currID;
    comms->currID++;
    hdr->type = type;
    hdr->crc = compute_CRC16(&comms->crc, data, length);
    hdr->length = length;
}

////////////////////////////////////////////////////////////////////////////////
// p2<->spin binding
////////////////////////////////////////////////////////////////////////////////
/**
 * p2 bindings at the bottom because they fuck with the pure c parsing
 */ 
struct __using("spin/jm_fullduplexserial.spin2") p2comms_spin;

void startfdser(int rxpin, int txpin, int mode, int baud) {
    p2comms_spin.start(rxpin, txpin, mode, baud);
}

uint8_t rxtixfdser(int tix) {
    return p2comms_spin.rxtix(tix);
}

void emitStr(uint8_t *str, int len) {
    int i = 0;
    for(i = 0; i < len; i++) {
        p2comms_spin.tx(str[i]);
    }
}

void readStr(uint8_t *ret, int len) {
    int i = 0;
    for(i = 0; i < len; i++) {
        ret[i] = p2comms_spin.rxtix(100);
    }
}

int available() {
    return p2comms_spin.available();
}
#endif
