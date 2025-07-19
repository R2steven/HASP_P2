#ifndef CRC_H
#define CRC_H

#include <stdint.h>

typedef struct CRC16_s {
    uint16_t byteTable[256]; //table of pre-computed crc values
    uint16_t generator; // generator polynomial
    uint8_t machineEndianness; // native machine endianness
}CRC16;

/**
 * compute the crc 16 checksum of an array of bytes with specified size.
 * 
 * @param bytes pointer to array of bytes
 * @param size size of byte array.
 * 
 * @return checksum value of the byte array
 */
uint16_t compute_CRC16(CRC16 *currcrc, uint8_t* bytes, uint32_t size) __fromfile("src/util/CRC.c");

/**
 * precompute the table of crc values. generator polynomial must
 * be set prior to calling. uninitialized generator variable will 
 * lead to undefined behavior.
 */
void setupTable(CRC16 *crc) __fromfile("src/util/CRC.c");

/**
 * default constructor
 * generator poly is 0x1021
 */
CRC16 *initCRC() __fromfile("src/util/CRC.c");

/**
 * default constructor no allocation
 * generator poly is 0x1021
 */
void initCRCNalloc(CRC16 *crc) __fromfile("src/util/CRC.c");

/**
 * set the generator polynomial. recomputes the byteTable.
 * 
 * @param nGenerator the new generator polynomial
 */
void setGenerator(CRC16 *crc, uint16_t nGenerator) __fromfile("src/util/CRC.c");

/**
 * get the generator polynomial
 * 
 * @return generator polynomial
 */
uint16_t getGenerator(CRC16 *crc) __fromfile("src/util/CRC.c");

/**
 * get the endianess of the current system. 
 * @return: 
 *          - true if current system is little endian
 *          - false if current system is big endian
 */
uint8_t getEndianness() __fromfile("src/util/CRC.c");

#endif
