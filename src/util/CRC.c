#include "CRC.h"
#include <stdint.h>

uint16_t getGenerator(CRC16 *crc) {
  return crc->generator;
}

void initCRCNalloc(CRC16 *crc) {
  crc->machineEndianness = getEndianness();
  crc->generator = 0x1021;
  setupTable(crc);
}

CRC16 *initCRC() {
    CRC16 *crc = (CRC16 *) malloc(sizeof(CRC16));
    initCRCNalloc(crc);
    return crc;
}

void setGenerator(CRC16 *crc, uint16_t nGenerator) {
  crc->generator = nGenerator;
  setupTable(crc);
}

//implementation of CRC-16 checksum
//Endianess of encode and decode system matters for ordering of data*
uint16_t compute_CRC16(CRC16 *curcrc, uint8_t *bytes, uint32_t size) {
    uint16_t crc = 0;
    uint8_t pos;

    for(uint32_t i = 0; i < size; i++){
      // XOR-in next input byte into MSB of crc, that's our new intermediate dividend
      pos = (uint8_t)((crc >> 8) ^ bytes[i]);
      // Shift out the MSB used for division per lookuptable and XOR with the remainder
      crc = ((crc<<8) ^ curcrc->byteTable[pos]);
    }

    return crc;
}

void setupTable(CRC16 *crc) {
  uint16_t currByte = 0;

  for(int i = 0; i < 256; i++) {
    currByte = (uint16_t) i << 8; // shift value of i into MSB
    for(int j = 0; j < 8; j++) {// iterate over all 256 values
      if ((currByte & 0x8000) != 0) { // test for MSB = bit 15 
      
        currByte = (currByte << 1) ^ crc->generator; // crc remainder
      }
      else {
        currByte <<= 1;
      }
    }
    crc->byteTable[i] = currByte; // put precomputed value into table
  }
}

uint8_t getEndianness() {
  //if little endian, LSB will be stored in Least memory Byte cell.(reversed)
  //if big endian, LSB will be stored in Largest memory Byte cell.
  uint16_t num[] = 0x0102; // two byte value
  return ((uint8_t*)num)[0] == 0x02 ? 1 : 0;
}
