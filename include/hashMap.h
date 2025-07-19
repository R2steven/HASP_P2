/**
 * Author: Ryan Stevens
 * 
 * simple hashmap. dynamic allocation of two arrays, no dynamic resize. it is 
 * recommended to size it at least 2x the amount of items you want to store. the 
 * size should also be a prime number for best results.
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>

typedef struct HashMap_s {
    uint32_t dataSize;
    uint32_t mapSize;
    uint32_t numItems;//number of items currently 
    uint32_t *keys;//stored key array
    void *data;//stored data item array
}HashMap;


HashMap *initHashMap(uint32_t mapSize, uint32_t dataSize) __fromfile("src/util/hashMap.c");

void destroyHashMap(HashMap *this) __fromfile("src/util/hashMap.c");

void initHashMapNalloc(HashMap *map, uint32_t mapSize, uint32_t dataSize) __fromfile("src/util/hashMap.c");

void insertHashMap(HashMap *map, uint32_t key, void *data) __fromfile("src/util/hashMap.c");

uint8_t getHashMap(HashMap *map, uint32_t key, void *ret) __fromfile("src/util/hashMap.c");

#endif
