#include <stdio.h>
#include "config.h"
#include "hashMap.h"


#define TEST_HASHMAP
#ifdef TEST_HASHMAP

typedef struct someData_s{
    uint32_t key;
    int *data;
}someData;

int main() {
    HashMap map = {0};
    initHashMap(&map, 61, sizeof(someData));

    int arr[30] = {0};

    for(uint32_t i = 0; i < 30; i++) {
        arr[i] = i;
    }

    

    for(uint32_t i = 0; i < 30; i++) {
        someData *data = (someData*) malloc(sizeof(someData));
        data->key = i<<4;
        data->data = &arr[i];
        //printf("input: %u, %u, %d\n", i, data->key, *data->data);
        insertHashMap(&map, i<<4, data);
    }

    someData ret = {0};

    for(uint32_t i = 0; i < 30; i++) {
        getHashMap(&map, i<<4, &ret);
        printf("output: %u, %u, %d\n", i, ret.key, *ret.data);
    }
}

#endif

