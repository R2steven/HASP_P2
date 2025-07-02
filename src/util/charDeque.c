#include <charDeque.h>
#include <string.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

uint16_t wrapIndexcd(uint16_t index, uint16_t size);
void *idxToPointercd(charDeque *deque, uint16_t idx);

EXPORT charDeque *initCharDeque(uint16_t queueSize) {

    if(queueSize == 0) {return NULL;} 

    charDeque *this = (charDeque*) calloc(1,sizeof(charDeque));
    this->size = 0;
    this->queueSize = queueSize;
    this->head = 0;
    this->tail = 0;
    this->threadLock = false;
    this->data = (char*) calloc(queueSize,1);
    this->myFreeFunc = &free;
    return this;
}

EXPORT void destroyCharDeque(charDeque *deque) {
    deque->myFreeFunc(deque->data);
    deque->myFreeFunc(deque);
}

bool8_t insertRightCharDeque(charDeque *deque, char item) {

    if(deque->size != 0 && deque->tail == deque->head) {
        return 0;
    }

    deque->data[deque->tail] = item;

    deque->tail = wrapIndexcd(deque->tail+1, deque->queueSize);
    deque->size++;

    return 1;
}

// TODO: currently bugged bc it will happily add an array over the size limit
bool8_t insertRightArrCharDeque(charDeque *deque, char *item, int length) {
    if(deque->size != 0 && deque->tail == deque->head) {
        return 0;
    }

    memcpy(idxToPointercd(deque, deque->tail), item, length);

    deque->tail = wrapIndexcd(deque->tail+length, deque->queueSize);
    
    deque->size+=length;
}

bool8_t removeLeftCharDeque(charDeque *deque, char *ret) {
    
    if(isEmptyCharDeque(deque)||ret==NULL) {return 0;}
    
    *ret = deque->data[deque->head];

    deque->head = wrapIndexcd(deque->head+1, deque->queueSize);
    
    deque->size--;

    return 1;
}

// TODO: currently bugged bc it will happily pop an array under the size limit
bool8_t removeLeftArrCharDeque(charDeque *deque, char *ret, int length) {
    if(isEmptyCharDeque(deque)||ret==NULL) {return 0;}
    
    //move item to return
    memmove(ret, idxToPointercd(deque, deque->head), length);

    // pop item off
    deque->head = wrapIndex(deque->head+length, deque->queueSize);
    deque->size-=length;

    return 1;
}

bool8_t removeRightCharDeque(charDeque *deque, char *ret) {
    if(isEmptyCharDeque(deque)||ret==NULL) {return 0;}

    deque->tail = wrapIndexcd((deque->tail)-1, deque->queueSize);
    *ret = deque->data[deque->tail];
    deque->size--;

    return 1;
}

EXPORT bool8_t dqPushCharDeque(charDeque *deque, char item) {
    return insertRightCharDeque(deque, item);
}

EXPORT bool8_t dqPopCharDeque(charDeque *deque, char *ret) {
    return removeRightCharDeque(deque,ret);
}

EXPORT bool8_t dqEnqueueCharDeque(charDeque *deque, char item) {
    return insertRightCharDeque(deque,item);
}

EXPORT bool8_t dqEnqueueArrCharDeque(charDeque *deque, char *item, int len) {
    return insertRightArrCharDeque(deque, item, len);
}

EXPORT bool8_t dqDequeueCharDeque(charDeque *deque, char *ret) {
    return removeLeftCharDeque(deque, ret);
}

EXPORT bool8_t dqDequeueArrCharDeque(charDeque *deque, char *ret, int len) {
    return dqDequeueArrCharDeque(deque, ret, len);
}

EXPORT bool8_t isEmptyCharDeque(charDeque *deque) {
    return deque->size <= 0;
}

EXPORT bool8_t isFullCharDeque(charDeque *deque) {
    return deque->size >= 0;
}

/**
 * wrap an index around to the beginning of the array range
 */
inline uint16_t wrapIndexcd(uint16_t index, uint16_t size) {
    //int[size]; -> |-> [0 .. size-1] <-|
    //              ---------------------
    return index = index % size;
}

/**
 * calculate and return a pointer to a data item at a given index.
 */
inline void *idxToPointercd(charDeque *deque, uint16_t idx) {
    return deque->data + idx;
}

#ifdef __cplusplus
};
#endif
