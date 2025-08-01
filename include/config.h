/**
 * Author: Ryan Stevens
 * 
 * common configuration definitions and macros
 */

#ifndef CONFIG_H
#define CONFIG_H

///////// P2 Configuration /////////


#include <propeller.h> // include P2 lib
#include <stdint.h>

// set compiler flag for P2
#ifndef __P2__
#define __P2__
#endif

#define P2_TARGET_MHZ 160 // target clock rate
#include "sys/p2es_clock.h" // allow setting clock rate
#define ARENA_SIZE 32768  // heap basically

#ifndef _BAUD
#define _BAUD 230400 // comms baud
#endif


#define true 1
#define false 0

////////////////////////////////////////////////////////
// define comms IDs
////////////////////////////////////////////////////////
#define NO_PACKET -1
#define COMMS_ID 1
#define MOTO_ID 2
#define SENSORS_ID 3

#endif
