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

#ifndef CHARDEQUE_H
#define CHARDEQUE_H

#include <export.h>
#include <stdlib.h>
#include <stdint.h>
#define uchar uint8_t //unsigned char, generally used when a raw pointer is to be passed.
#define bool8_t uint8_t //bool indicating actual data size in bits
#define size16_t uint16_t //size type.

/**
 * Deque data instance. stores Deque data members
 */
typedef struct charDeque{
    char* data; //dynamically allocated data array
    void (*myFreeFunc)(void*); // stored free function for destruction
    size16_t queueSize;
    size16_t size; // current size of deque in dataItems
    size16_t head; // next available head
    size16_t tail; // next available tail
    uint8_t threadLock;
}charDeque;

/**
 * allocates required memory for dequeue construction and initilizes the 
 * resulting data structure.
 * 
 * resource allocation is initialization (raii) style init function
 * 
 * @param queueSize the size of the desired queue
 * @param sizeOfData the size of each data object stored in queue in bytes
 */
EXPORT charDeque *initCharDeque(uint16_t queueSize) __fromfile("src/util/charDeque.c");

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
/**Deque *initCharDequeCalloc(void *(*myCallocFunc)(size_t,size_t), void (*myFreeFunc)(void*),
                    size_t queueSize, uint8_t sizeOfData) __fromfile("src/util/charDeque.c");
*/

/**
 * perform all necessary operations to destroy the deque and free its memory
 */
EXPORT void destroyCharDeque(charDeque *deque) __fromfile("src/util/charDeque.c");

/**
 * Insert item on the right of the dequeue. Operating over an allocated array of 
 * data of size deque->dataSize.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
bool8_t insertRightCharDeque(charDeque *deque, char item) __fromfile("src/util/charDeque.c");

/**
 * Insert an array of chars
 * 
 * TODO: currently bugged bc it will happily add an array over the size limit
 */
bool8_t insertRightArrCharDeque(charDeque *deque, char *item, int length) __fromfile("src/util/charDeque.c");

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
bool8_t removeLeftCharDeque(charDeque *deque, char *ret) __fromfile("src/util/charDeque.c");


/**
 * dequeue an array of chars
 */
bool8_t removeLeftArrCharDeque(charDeque *deque, char *item, int length) __fromfile("src/util/charDeque.c");

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
bool8_t removeRightCharDeque(charDeque *deque, char *ret) __fromfile("src/util/charDeque.c");

/**
 * Push an item onto the stack reprisented by this deque.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqPushCharDeque(charDeque *deque, char item) __fromfile("src/util/charDeque.c");

/**
 * Pop an item from the top of the stack reprisented by this deque, and move its
 * data into data pointer.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory of size deque->dataSize to move returned data to
 *          MUST BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqPopCharDeque(charDeque *deque, char *ret) __fromfile("src/util/charDeque.c");

/**
 * Enqueue an item into the queue reprisented by this deque.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqEnqueueCharDeque(charDeque *deque, char item) __fromfile("src/util/charDeque.c");

/**
 * Enqueue an  array of items into the queue reprisented by this deque.
 * 
 * @param *deque data structure containing deque data members
 * @param *item pointer to memory of size deque->dataSize to copy into queue MUST
 *          BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqEnqueueArrCharDeque(charDeque *deque, char *item, int len);

/**
 * Dequeue an item from the front of the queue reprisented by this deque, and move its
 * data into data pointer.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory of size deque->dataSize to move returned data to
 *          MUST BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqDequeueCharDeque(charDeque *deque, char *ret) __fromfile("src/util/charDeque.c");

/**
 * Dequeue an array of items from the front of the queue reprisented by this deque, and move its
 * data into data pointer.
 * 
 * @param *deque data structure containing deque data members
 * @param *ret pointer to memory of size deque->dataSize to move returned data to
 *          MUST BE SAME DATA SIZE AS deque->dataSize
 */
EXPORT bool8_t dqDequeueArrCharDeque(charDeque *deque, char *ret, int len);

/**
 * returns a boolean indicating whether the deque is empty
 * 
 * @param *deque data structure containing deque data members
 */
EXPORT bool8_t isEmptyCharDeque(charDeque *deque) __fromfile("src/util/charDeque.c");

/**
 * returns a boolean indicating whether the deque is full
 * 
 * @param *deque data structure containing deque data members
 */
EXPORT bool8_t isFullCharDeque(charDeque *deque) __fromfile("src/util/charDeque.c");

#endif
