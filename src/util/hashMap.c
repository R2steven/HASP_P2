#include <string.h>
#include <stdlib.h>
#include "hashMap.h"

#define map_null -1

HashMap *initHashMap(uint32_t mapSize, uint32_t dataSize) {
    HashMap *map = (HashMap *) malloc(sizeof(HashMap));
    initHashMapNalloc(map, mapSize, dataSize);
    return map;
}

void initHashMapNalloc(HashMap *map, uint32_t mapSize, uint32_t dataSize) {
    map->numItems = 0;
    map->mapSize = mapSize;
    map->dataSize = dataSize;
    map->keys = (uint32_t*) calloc(mapSize, sizeof(uint32_t));

    for(uint32_t i = 0; i < mapSize; i++) {
        map->keys[i] = map_null;
    }

    map->data = calloc(mapSize, dataSize);
}

void destroyHashMap(HashMap *this) {
    free(this->keys);
    free(this->data);
}

void insertHashMap(HashMap *map, uint32_t key, void *data) {

    uint32_t idx = key % map->mapSize;
    uint32_t x = 2;
    uint32_t itrs = 0;


    while(map->keys[idx]!=map_null
    && itrs < map->mapSize * 2) 
    {
        if(map->keys[idx]==key) {
            return;
        }
        idx = (idx + (x*x)) % map->mapSize;
        x++;
        itrs++;
        
    }
    
    if(map->keys[idx]==map_null) {
        map->keys[idx] = key;
        memcpy(&map->data[idx*map->dataSize], data, map->dataSize);
        map->numItems++;
    }
}

uint8_t getHashMap(HashMap *map, uint32_t key, void* ret) {
    uint32_t idx = key % map->mapSize;
    uint32_t x = 2;
    uint32_t itrs = 0;

    while(map->keys[idx]!=map_null 
    && map->keys[idx]!=key 
    && itrs < map->numItems * 2)   
        {
        idx = (idx * (x*x)) % map->mapSize;
        x++;
        itrs++;
    }

    if(map->keys[idx]==key) {
        memcpy(ret, &map->data[idx*map->dataSize], map->dataSize);
        return 1;
    }

    return 0;
}
