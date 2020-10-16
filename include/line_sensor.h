#ifndef LINE_SENSOR_GUARD
#define LINE_SENSOR_GUARD

/*! @file line_sensor.h
    @brief 
*/

#include "config.h"
#include <stdbool.h>

void update_sensor_status();
uint8_t line_sensor_get_status(); 

#endif /* LINE_SENSOR_GUARD */
