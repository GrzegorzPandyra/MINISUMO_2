#ifndef DISTANCE_SENSOR_GUARD
#define DISTANCE_SENSOR_GUARD

#include <stdint.h>
#include "config.h"

void distance_sensor_update_status();
void distance_sensor_get_status(uint8_t *results);

#endif /* DISTANCE_SENSOR_GUARD */
