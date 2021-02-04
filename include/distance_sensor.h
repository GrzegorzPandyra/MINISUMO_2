#ifndef DISTANCE_SENSOR_GUARD
#define DISTANCE_SENSOR_GUARD

#include <stdint.h>
#include "config.h"

void distance_sensor_update_status(const uint16_t adc_val);
uint16_t distance_sensor_get_status(void);

#endif /* DISTANCE_SENSOR_GUARD */
