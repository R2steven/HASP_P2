/**
 * Author: Ryan Stevens
 * 
 * HASP25 SpectraSolis propeller comms module. Handles 
 */

#ifndef HASP25_COMMS_MODULE
#define HASP25_COMMS_MODULE

#include "config.h"
#include "deque.h"

Deque *initComms() __fromfile("src/comms.c");

void setCommsTx() __fromfile("src/sensors.c");

void runComms() __fromfile("src/comms.c");

#endif
