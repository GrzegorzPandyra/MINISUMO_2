/*! @file distance_sensor.c
    @brief This module handles the data from ADC output, stores it and performs calculations on them. 
    Data comes from two distance sensors (DS). It is read via ADC and stored in buffers separate for each DS. 
    Other modules may request to read the average of the data samples - this is meant as a form of basic "debauncing" to avoid erratic behavior on invalid data.
*/

#include "distance_sensor.h"
#include "serial_tx.h"

#define MAX_NUM_SAMPLES 10

/* Local macro-like functions */
#define SB(x) (1<<(x))          /* set bit   */
#define CB(x) (~(1<<(x)))       /* clear bit */

typedef struct Distance_Sensor_Tag{
    uint16_t samples[MAX_NUM_SAMPLES];
    uint8_t num_samples_stored;
    uint16_t *sample_ptr;
} Distance_Sensor_T;

/* DS array */
static Distance_Sensor_T ds_sensors[] = {
    {{0}, 0, NULL},
    {{0}, 0, NULL}
}; 

/**********************************************************************
* Static functions 
***********************************************************************/
static uint16_t getAvg(const uint16_t *samples, uint8_t num_samples_stored){
    uint16_t result = 0;
    for(uint8_t i=0; i<num_samples_stored; i++){
        result += samples[i];
    }
    return result/(uint16_t)num_samples_stored;
}

/**********************************************************************
* Public functions 
***********************************************************************/
void distance_sensor_read_ADC(DS_ID_T DS_ID, volatile uint16_t adc_val){
    Distance_Sensor_T *current_ds = &ds_sensors[DS_ID];

    *(current_ds->sample_ptr) = adc_val;

    if(current_ds->sample_ptr == &(current_ds->samples[MAX_NUM_SAMPLES-1])){
        current_ds->sample_ptr = current_ds->samples;
    } else {
        current_ds->sample_ptr++;
    }

    if(current_ds->num_samples_stored < MAX_NUM_SAMPLES){
        current_ds->num_samples_stored++;
    }
}

uint16_t distance_sensor_get_status(DS_ID_T DS_ID){
    return getAvg(ds_sensors[DS_ID].samples, ds_sensors[DS_ID].num_samples_stored);
}

void distance_sensor_init(void){
    /* Set DS pins as input */
    DDRC &= CB(DS1);
    DDRC &= CB(DS2);
    /* Initialize sample_ptrs */
    ds_sensors[DS1_ID].sample_ptr = ds_sensors[DS1_ID].samples;
    ds_sensors[DS2_ID].sample_ptr = ds_sensors[DS2_ID].samples;
}