
#include "comms.h"


struct __using("spin/jm_fullduplexserial.spin2") p2comms;

////////////////////////////////////////////////////////
// define comms IDs
////////////////////////////////////////////////////////
#define NO_PACKET -1
#define COMMS_ID 1


HASP25_Comms *commsInst;

HASP25_Comms *initComms() {
    commsInst = (HASP25_Comms*) malloc(sizeof(HASP25_Comms));
    
    ////// Constant configs ///////////////////////////
    commsInst->RXPin = 1;
    commsInst->TXPin = 0;
    commsInst->Baud = 230400;
    commsInst->size = 0;
    ///////////////////////////////////////////////////

    initHashMapNalloc(&commsInst->rxHandles,31,sizeof(CommsPacketHandler));
    //initD(&commsInst->txHandles, 31,sizeof(CommsPacketHandler));
    commsInst->workingBuff =  (uint8_t *) malloc(512);
    commsInst->header = (HASP25_Header *) commsInst->workingBuff;


    p2comms.start(commsInst->RXPin, commsInst->TXPin, 0, commsInst->Baud);

    initCRCNalloc(&commsInst->crc);

    return commsInst;
}

void regRxPacket(   HASP25_Comms* comms,
                    uint32_t key, 
                    Deque *output, 
                    pHandler packetHandler
                ) 
{
    CommsPacketHandler handle = {0};
    handle.key = key;
    handle.buff = output;
    handle.packetHandler = packetHandler;

    insertHashMap(&comms->rxHandles, key, &handle);
}

// void regTxPacket(   HASP25_Comms* comms,
//                     uint32_t key, 
//                     Deque *input, 
//                     pHandler CommsPacketHandler
//                 )
// {
//     CommsPacketHandler handle = {0};
//     handle.key = key;
//     handle.buff = input;
//     handle.packetHandler = packetHandler;

//     insertHashMap(&comms->txHandles, key, &handle);
//}

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
void rxCheck(HASP25_Comms *comms) {
    uint8_t itrs = 0;

    comms->header->S = 0;
    comms->header->P = 0;
    comms->header->PID = 0;
    comms->header->type = 0;
    comms->header->crc = 0;
    comms->header->length = 0;


    int smalltimeout = 100;
    int largetimeout = 1000;

    (comms->header)->S = p2comms.rxtix(smalltimeout);
    while((comms->header)->S == -1 || (comms->header)->S != 'S') { //align with packet header
        if(itrs >= 128) {
            printf("unable\n");
            return 0; //unable to find header
        }
        (comms->header)->S = p2comms.rxtix(smalltimeout);
        itrs++;
    }

    itrs = 0;
    (comms->header)->P = p2comms.rxtix(largetimeout);

    if((comms->header)->P != 'P') {
        printf("broke invalid header: %c\n", (comms->header)->P);
        return 0;
    }
    
    int32_t pid = 0;
    pid = pid | (((uint8_t)p2comms.rxtix(largetimeout)) << 24);
    pid = pid | (((uint8_t)p2comms.rxtix(largetimeout)) << 16);
    pid = pid | (((uint8_t)p2comms.rxtix(largetimeout)) << 8);
    pid = pid | (((uint8_t)p2comms.rxtix(largetimeout)));

    (comms->header)->PID = pid;

    (comms->header)->type = p2comms.rxtix(largetimeout);

    uint16_t crc = 0;
    crc = crc | (((uint8_t)p2comms.rxtix(largetimeout)) << 8);
    crc = crc | (((uint8_t)p2comms.rxtix(largetimeout)));
    (comms->header)->crc = crc;
    (comms->header)->length = p2comms.rxtix(largetimeout);

    //header read
    printf("header recieved; S: %c, P: %c, PID: %d, type: 0x%x, crc: %d, length: %x",(comms->header)->S, (comms->header)->P, (uint32_t)(comms->header)->PID, (uint8_t)(comms->header)->type, (uint16_t)(comms->header)->crc, (uint8_t)(comms->header)->length);
    
    //printf("index %d, length %d",comms->size,(comms->header)->length);
    //read data into buffer
    comms->size = 0;
    for (;comms->size < (comms->header)->length; comms->size++) {
        (comms->workingBuff)[comms->size] = p2comms.rxtix(largetimeout);
    }
    //data recieved
    
    if ((comms->header)->crc != compute_CRC16(&comms->crc, &comms->workingBuff[12], comms->header->length)) {
        printf("crc vs computed: %d, %d",(comms->header)->crc, compute_CRC16(&comms->crc, &comms->workingBuff[12], comms->header->length));
        printf("data: %c", comms->workingBuff[12]);
        return 0;
    }
    
    
    CommsPacketHandler *rxhandler = {0};

    getHashMap(&comms->rxHandles, (comms->header)->type, rxhandler);
    printf("retrieved addr: %d", *rxhandler);
    (*rxhandler->packetHandler)(rxhandler->buff, &comms->workingBuff[12], (comms->header)->length);
}

/** packet format:
 * 
 * Byte#        Type        Value
 * 
 * 1            Char        'S' capitol S
 * 2            Char        'P' capitol P
 * 3-7          int32       Packet ID
 * 8            Byte        Packet Type
 * 9-10         int16       CRC16 checksum
 * 11           Byte        DataLength in Bytes
 * 12           Not Used    Not Used/ null char
 * 13-end       Data        Data
 */
void txCheck() {

}

int runComms() {

}
