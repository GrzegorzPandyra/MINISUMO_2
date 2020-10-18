/*! @file distance_sensor.c
    @brief 
*/

#include "distance_sensor.h"
#include <avr/io.h>

#define num_elems(x) (sizeof(x)/sizeof(x[0]))

typedef struct Distance_Sensor_Tag{
    uint8_t pin;
    uint8_t samples[10];
    uint8_t num_samples;
    uint8_t (*getAvg)(uint8_t *, uint8_t);
} Distance_Sensor_T;

static uint8_t getAvg(uint8_t *samples, uint8_t num_samples);

Distance_Sensor_T distance_sensor_array[] = {
    {DS1, {0}, 0, getAvg},
    {DS2, {0}, 0, getAvg}
};

static uint8_t getAvg(uint8_t *samples, uint8_t num_samples){
    uint16_t result = 0;
    for(uint8_t i=0; i<num_samples; i++)
        result += samples[i];
    return result/num_samples;
}

void distance_sensor_update_status(){

}

void distance_sensor_get_status(uint8_t *results){
    for(uint8_t i=0; i<num_elems(distance_sensor_array); i++)
        results[i] = distance_sensor_array[i].getAvg(distance_sensor_array[i].samples, distance_sensor_array[i].num_samples);
}