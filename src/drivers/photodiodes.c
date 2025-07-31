/**
 * Authors: Alisdair Gifford, Ryan Stevens
 * 06/06/2025
 *  
 * integration of 8 photodiode sensors on the Parallax Propeller 2. utilizes
 * an 8 channel analog multiplexer to reduce the number of pins used.
 * 
 * NOTE: in our implementation, the steering angles are all relative to the 
 * directin in which the payload is pointing. the photodiodes will not be set in
 * a global frame of refrence.
 */

#include <float.h>
#include <math.h>

struct __using("spin/analog_Mux_8.spin2") photodiodes;

#define NUM_SENSORS 8
/*
const int NUM_ALT_SENSORS = 3;
const int ALT_SENSORS[] = {0,6,7};

const int NUM_AZ_SENSORS = 6;
const int AZ_SENSORS[] = {0,1,2,3,4,5};

const int FIRST_PIN = 4; //TODO: figure out which pins work
*/
const int TIMEOUT = 250; //microseconds to let circuit stabilize for reading

//tuning params
const int HI = 10000;
const int LO = 0;

/**
 * initialize the temperature sensor pins on the propeller and return an
 * int indicating initialization success. will be >0 on success and ==0 on
 * failure
 */
int photoDiode_start() {
    return photodiodes.startCstm(14,13,12,-1,15, TIMEOUT,0,HI);
}

/**
 * get the azimuth angle off of photodiode direct forward for the brightest 
 * object seen by the photodiodes
 * 
 * @return vertical steering angle above horizontal
 */
double getHorizontalAngle() { 
    int intensityArray[NUM_SENSORS] = {0};// intensities between 0 and 1000

    pollDiode(intensityArray);

    int angle1Intensity = intensityArray[0], angle2Intensity = 0, angle1IDX = 0, angle2IDX = 0, angle1, angle2;

    for (int i = 1; i < 6; i++){
        if (intensityArray[i] > angle1Intensity){
            angle2Intensity = angle1Intensity;
            angle2IDX = angle1IDX;

            angle1Intensity = intensityArray[i];
            angle1IDX = i;
        } else if (intensityArray[i] > angle2Intensity){
            angle2Intensity = intensityArray[i];
            angle2IDX = i;
        }
    }

    angle1 = degposition(angle1IDX);
    angle2 = degposition(angle2IDX);
    double totalintensity = (double)angle1Intensity + (double)angle2Intensity;

    return ( ( (double)angle1 * ( (double)angle1Intensity/totalintensity) ) + ( (double)angle2 * ((double)angle2Intensity/totalintensity) ) );

}

/**
 * get the altitude angle off of vertical for the brightest object seen by the 
 * photodiodes.
 * 
 * @return altitude steering angle above photodiode horizontal
 */
double getVerticalAngle() { 
    int intensityArray[NUM_SENSORS] = {0};// intensities between 0 and 1000

    pollDiode(intensityArray);

    double totalintensity = (double)intensityArray[6] + (double)intensityArray[7];
    return ( ( 45.0 * ( (double)intensityArray[6]/totalintensity) ) + ( 90.0 * ((double)intensityArray[7]/totalintensity) ) );

}

/**
 * get the angle the photodiode is set off of directly forward.
 */
int degposition(int idx){
    switch (idx){
        case 0:
            return 0;
        case 1:
            return 45;
        case 2:
            return 90;
        case 3:
            return 180;
        case 4:
            return -90;
        case 5:
            return -45;
        default:
            return -1000;
    }
}

/** 
 * returning arrays from spin2 objects is weird and confusing to just defining 
 * the poll function here.
 */
int pollDiode(int *ret) {
    for(int i = 0; i < NUM_SENSORS; i++) {
        ret[i] = photodiodes.read(i);
        #ifdef DEBUG
            //printf("reading %d: %d\n",i, ret[i]);
        #endif
    }
}

double getConfidenceValue(int* readings) {

}


int getDiodeSteeringMessage(char *buff, int len) { //TODO: implement me! idk if I want to send double or the string reprisentation
    double angle = getHorizontalAngle();

    int len = snprintf(buff,50,"HorizontalSteer: $%.4f", angle);
    return len;
}

/*int main() {
    photoDiode_start();

    for(int i = 0; i < 100; i++) {
        printf("Running Photodiode! %.4f\n", getHorizontalAngle());
        _waitms(500);
    }
}*/
