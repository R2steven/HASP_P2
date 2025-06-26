/**
 * Authors: Alisdair Gifford, Ryan Stevens
 * 06/06/2025
 *  
 * integration of 8 temperature sensors on the Parallax Propeller 2. utilizes
 * an 8 channel analog multiplexer to reduce the number of pins used.
 */

#include <float.h>

#define DEBUG


#include <stdio.h>

struct __using("spin/analog_Mux_8.spin2") tempSensors;

#define NUM_SENSORS 8

#define THERM_FIRST_PIN 0 //TODO: figure out which pins work
#define THERM_TIMEOUT 100 //microseconds to let circuit stabilize for reading

/**
 * initialize the temperature sensor pins on the propeller and return an
 * int indicating initialization success. will be >0 on success and ==0 on
 * failure
 */
int therm_start() {
    return tempSensors.startCstm(21,22,23,-1,20, THERM_TIMEOUT,0,3300);
}

/**
 * get the temperature readings in celcius from the temperature sensors. return
 * buffer array must be at least NUM_SENSORS in length.
 * 
 * @param ret the returned array.
 */
void getTemp(double *ret) {

    for(int i = 0; i < NUM_SENSORS; i++) {
        int val = tempSensors.read(i);
        ret[i] = ((val - 500.0) / 10.0);
        #ifdef DEBUG
            //printf("Voltage %d: %d, temp:%.2f\n",i, val, ret[i]);
        #endif
    }
}

/**
 * get the temperature readings serialized as null terminated strings. on proper 
 * print, return the length of the string written to the buffer, not including 
 * null termination.
 * 
 * return buffer recommended to be at least 100 chars long. if the serialization
 * fails, the return will be the negative length of the bytes written, 
 * indicating an overflow. if it is longer, any extra space will simply not be 
 * used. 
 * 
 * 
 * @param buff must be at least 100 chars long.
 * @param len 
 */
int getTempMessage(char* buff, int len) { //TODO: finialize, idk if I like the variable mesage length.
    
    double readings[NUM_SENSORS] = {0};

    getTemp(readings);

    int prntLen = 0;
    int strLen = 0;
    char str[100] = {0};

    prntLen += sprintf(buff+prntLen, "Temperatures[8]: { ");

    for(int i = 0; i < NUM_SENSORS; i++) {
        strLen = sprintf(str, "%.2f", readings[i]);
        if(strLen < 0) {
            #ifdef DEBUG
                printf("Temperature message failed to serialize at temp %d", i);
            #endif
            return -1 * prntLen;
        }
        else if(prntLen + strLen >= len) {
            #ifdef DEBUG
                printf("Temperature message overflowed buffer at temp %d", i);
            #endif
            return -1 * prntLen;
        }

        memcpy(buff+prntLen, str, strLen);
        prntLen += strLen;

        if(i < NUM_SENSORS-1) {
            if(prntLen+2 >= len) {
                #ifdef DEBUG
                    printf("Temperature message overflowed buffer at temp delimiter %d", i);
                #endif
                return -prntLen;
            }
            buff[prntLen] = ',';
            buff[prntLen+1] = ' ';
            prntLen+=2;
        }
    }

    if(prntLen + 2 >= len) {
        #ifdef DEBUG
            printf("Temperature message overflowed buffer at end delimiter");
        #endif
        return -prntLen;
    }
    
    buff[prntLen] = '}';
    buff[prntLen+1] = '\n';

    return prntLen+2;
}
/*
int main() {
    #define DEBUG
    
    therm_start();

    char str[200] = {0};

    while(1) {
        getTempMessage(str, 200);
        printf(str);
        _waitms(500);
    }

}*/
