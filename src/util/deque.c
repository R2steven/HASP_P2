#include <deque.h>
#include <string.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

uint16_t wrapIndex(uint16_t index, uint16_t size);
void *idxToPointer(Deque *deque, uint16_t idx);

EXPORT Deque *initDeque(uint16_t queueSize, uint8_t sizeOfData) {
    if(queueSize == 0 || sizeOfData == 0) {return NULL;}

    Deque *this = (Deque*) calloc(1,sizeof(Deque));
    this->queueSize = queueSize;
    this->dataSize = sizeOfData;
    this->size = 0;
    this->head = 0;
    this->tail = 0;
    this->threadLock = 0;
    this->data = calloc(queueSize,sizeOfData);
    this->myFreeFunc = &free;
    return this;
}

Deque *initDequeCalloc(void *(*myCallocFunc)(size_t,size_t), void (*myFreeFunc)(void*),
                    size_t queueSize, uint8_t sizeOfData) {

    if(queueSize == 0 || sizeOfData == 0
        || myCallocFunc == NULL || myFreeFunc == NULL) {
            return NULL;
        }
        
    Deque *this = (Deque*) (*myCallocFunc)(1,sizeof(Deque));
    this->queueSize = queueSize;
    this->dataSize = sizeOfData;
    this->size = 0;
    this->head = 0;
    this->tail = 0;
    this->threadLock = 0;
    this->data = (*myCallocFunc)(queueSize,sizeOfData);
    this->myFreeFunc = &(*myFreeFunc);
    return this;
}

EXPORT void destroyDeque(Deque *deque) {
    deque->myFreeFunc(deque->data);
    deque->myFreeFunc(deque);
}

bool8_t insertRight(Deque *deque, void *item) {

    if(deque->size != 0 && deque->tail == deque->head) {
        return 0;
    }

    memcpy(idxToPointer(deque, deque->tail), item, deque->dataSize);

    deque->tail = wrapIndex(deque->tail+1, deque->queueSize);
    deque->size++;

    return 1;
}

bool8_t removeLeft(Deque *deque, void *ret) {
    if(isEmpty(deque)||ret==NULL) {return 0;}
    
    //move item to return
    memmove(ret, idxToPointer(deque, deque->head), deque->dataSize);

    // pop item off
    deque->head = wrapIndex(deque->head+1, deque->queueSize);
    deque->size--;

    return 1;
}

bool8_t removeRight(Deque *deque, void *ret) {
    if(isEmpty(deque)||ret==NULL) {return 0;}

    deque->tail = wrapIndex((deque->tail)-1, deque->queueSize);
    deque->size--;

    memmove(ret, idxToPointer(deque, deque->tail),deque->dataSize);

    return 1;
}

EXPORT bool8_t dqPush(Deque *deque, void *item) {
    return insertRight(deque, item);
}

EXPORT bool8_t dqPop(Deque *deque, void *ret) {
    return removeRight(deque,ret);
}

EXPORT bool8_t dqEnqueue(Deque *deque, void *item) {
    return insertRight(deque,item);
}

EXPORT bool8_t dqDequeue(Deque *deque, void *ret) {
    return removeLeft(deque, ret);
}

EXPORT bool8_t isEmpty(Deque *deque) {
    return deque->size <= 0;
}

EXPORT bool8_t isFull(Deque *deque) {
    return deque->size >= deque->queueSize - 1;
}

/**
 * wrap an index around to the beginning of the array range
 */
inline uint16_t wrapIndex(uint16_t index, uint16_t size) {
    //int[size]; -> |-> [0 .. size-1] <-|
    //              ---------------------
    return index = index % size;
}

/**
 * calculate and return a pointer to a data item at a given index.
 */
inline void *idxToPointer(Deque *deque, uint16_t idx) {
    return deque->data + deque->dataSize * idx;
}

#ifdef __cplusplus
};
#endif