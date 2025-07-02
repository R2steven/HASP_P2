/**
 * Author: Ryan Stevens
 * 
 * Sensor thread module, manages and pulls data from all propeller connected 
 * SpectraSolis instruments. dispatches data to the main computer. 
 */
#ifndef HASP_25_SENSORS_H
#define HASP_25_SENSORS_H

#include "config.h"
#include "deque.h"

Deque *initSensors() __fromfile("src/sensors.c");

void setSensorTx() __fromfile("src/sensors.c");

void runSensors() __fromfile("src/sensors.c")

#endif
