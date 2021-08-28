#ifndef DISTANCE_SENSOR_GUARD
#define DISTANCE_SENSOR_GUARD

#include <stdint.h>
#include "config.h"

typedef enum DS_ID_Tag{
    DS1_ID = 0,
    DS2_ID
}DS_ID_T;

void distance_sensor_read_ADC(DS_ID_T DS_ID, volatile uint16_t adc_val);
uint16_t distance_sensor_get_status(DS_ID_T DS_ID);
void distance_sensor_init(void);

#endif /* DISTANCE_SENSOR_GUARD */
