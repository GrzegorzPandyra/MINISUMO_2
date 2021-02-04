/*! @file distance_sensor.c
    @brief 
*/

#include "distance_sensor.h"
#include "serial_tx.h"

#define MAX_NUM_SAMPLES 10

struct Distance_Sensor{
    uint8_t pin;
    uint16_t samples[MAX_NUM_SAMPLES];
    uint8_t num_samples_stored;
    uint16_t *sample_ptr;

};
static struct Distance_Sensor distance_sensor = {DS1, {0}, 0, distance_sensor.samples};

static uint16_t getAvg(const uint16_t *samples, uint8_t num_samples_stored){
    uint16_t result = 0;
    for(uint8_t i=0; i<num_samples_stored; i++){
        result += samples[i];
    }
    return result/num_samples_stored;
}

void distance_sensor_update_status(const uint16_t adc_val){
    *(distance_sensor.sample_ptr) = adc_val;
    if(distance_sensor.sample_ptr == &(distance_sensor.samples[MAX_NUM_SAMPLES-1])){
        distance_sensor.sample_ptr = &(distance_sensor.samples[0]);
    } else {
        distance_sensor.sample_ptr++;
    }
    if(distance_sensor.num_samples_stored < MAX_NUM_SAMPLES){
        distance_sensor.num_samples_stored++;
    }
}

uint16_t distance_sensor_get_status(void){
    return getAvg(distance_sensor.samples, distance_sensor.num_samples_stored);
}