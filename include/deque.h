/**
 * Author: Ryan Stevens
 * 
 * A dequeue is a generalized datastructre capable of acting as both a queue and 
 * a stack. general dequeue implementation in C, designed to be fit over a byte 
 * array buffer. Works on raw Bytes by default. If dataType is larger, be sure 
 * to set it utilizing setters.
 * 
 * Switching between using this data structure as a stack and a queue can result
 * in undefined behavior. Currently both stack and queue operations will grow
 * the data structure to the right, the stack will shrink from the right and the
 * queue will shrink from the left.
 * 
 * Does not perform dynamic growth. If the underlying data array fills up the
 * Deque will return a false flag from the operation. 
 * 
 * performs data return by either copying or moving data into a pointer which
 * is passed in. This is to avoid conflicts with data ownership. the internal
 * deque data will always be owned by the deque, and all data retrieved from the
 * deque will be owned by the calling function.
 * 
 * TODO: evaluate use and cost/benefit for storing the dataItems directly, 
 *          instead of storing pointers and enabling move semantics.
 */

#ifndef DEQUE_H
#define DEQUE_H

#include <export.h>
#include <stdlib.h>
#include <stdint.h>
#define uchar uint8_t //unsigned char, generally used when a raw pointer is to be passed.
#define bool8_t uint8_t //bool indicating actual data size in bits
#define size16_t uint16_t //size type.

/**
 * Deque data instance. stores Deque data members
 */
typedef struct Deque{
    void* data; //dynamically allocated data array
    void (*myFreeFunc)(void*); // stored free function for destruction
    uint8_t dataSize; //size of dataMembers in Bytes
    size16_t queueSize; //size of underlying data array in dataItems
    size16_t size; // current size of deque in dataItems
    size16_t head; // next available head
    size16_t tail; // next available tail
    uint8_t threadLock;
}Deque;

/**
 * allocates required memory for dequeue construction and initilizes the 
 * resulting data structure.
 * 
 * resource allocation is initialization (raii) style init function
 * 
 * @param queueSize the size of the desired queue
 * @param sizeOfData the size of each data object stored in queue in bytes
 */
EXPORT Deque *initDeque(uint16_t queueSize, uint8_t sizeOfData) __fromfile("src/util/deque.c");

/**
 * same as initDeque but initializes deque struct at the pointer specified.
 */
void initDequeNalloc(Deque *this, uint16_t queueSize, uint8_t sizeOfData) __fromfile("src/util/deque.c");

/**
 * allocates required memory for dequeue construction and initilizes the 
 * resulting data structure.
 * 
 * allows the passing of a user defined calloc and free functions, must take as 
 * input parameter the size of memory to allocate in bytes and return a pointer 
 * to the allocated memory. the allocated memory must be initialized to 0. 
 * 
 * Intended for use with memory structures such as arenas.
 * 
 * @param calloc_func user-defined calloc function. initializes memory to 0.
 * @param queueSize the size of the desired queue
 * @param sizeOfData the size of each data object stored in queue
 */
Deque *initDequeCalloc(void *(*myCallocFunc)(size_t,size_t), void (*myFreeFunc)(void*),
                    size_t queueSize, uint8_t sizeOfData) __fromfile("src/util/deque.c");

/**
 * perform all necessary operations to destroy the deque and free its memory
 */
EXPORT void destroyDeque(Deque *deque) __fromfile("src/util/deque.c");

/**
 * Insert item on the right of the dequeue. Operating over an allocated array of 
 * data of size deque->dataSize.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
bool8_t insertRight(Deque *deque, void *item) __fromfile("src/util/deque.c");

/**
 * Remove item on the left of the dequeue, and move it into ret pointer. 
 * 
 * Operating over an allocated array of data of size deque->dataSize.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory to move return val to. must be large enough to
 *          accept data of size deque->dataSize
 * @return flag indicating the sucess of the operation
 */
bool8_t removeLeft(Deque *deque, void *ret) __fromfile("src/util/deque.c");

/**
 * Remove item on the right of the dequeue, and move it into ret pointer. 
 * 
 * Operating over an allocated array of data of size deque->dataSize.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory to move return val to. must be large enough to
 *          accept data of size deque->dataSize
 * @return flag indicating the sucess of the operation
 */
bool8_t removeRight(Deque *deque, void *ret) __fromfile("src/util/deque.c");

/**
 * Push an item onto the stack reprisented by this deque.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqPush(Deque *deque, void *item) __fromfile("src/util/deque.c");

/**
 * Pop an item from the top of the stack reprisented by this deque, and move its
 * data into data pointer.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory of size deque->dataSize to move returned data to
 *          MUST BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqPop(Deque *deque, void *ret) __fromfile("src/util/deque.c");

/**
 * Enqueue an item into the queue reprisented by this deque.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqEnqueue(Deque *deque, void *item) __fromfile("src/util/deque.c");

/**
 * Dequeue an item from the top of the stack reprisented by this deque, and move its
 * data into data pointer.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory of size deque->dataSize to move returned data to
 *          MUST BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqDequeue(Deque *deque, void *ret) __fromfile("src/util/deque.c");

/**
 * returns a boolean indicating whether the deque is empty
 * 
 * @param *deque data structure containing deque data members
 */
EXPORT bool8_t isEmpty(Deque *deque) __fromfile("src/util/deque.c");

/**
 * returns a boolean indicating whether the deque is full
 * 
 * @param *deque data structure containing deque data members
 */
EXPORT bool8_t isFull(Deque *deque) __fromfile("src/util/deque.c");

#endif
