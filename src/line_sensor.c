/*! @file line_sensor.c
    @brief 
*/

#include "config.h"
#include "serial_tx.h"
#include "line_sensor.h"
#include "serial_tx.h"

/* Local macro-like functions */
#define SB(x) (1<<(x))          
#define CB(x) (~(1<<(x)))       
#define num_elems(x) (sizeof(x)/sizeof(x[0]))
#define DEBOUNCE_COUNTER_MAX 3

typedef struct Line_Sensor_Tag{
    uint8_t pin;
    bool status;
    uint8_t debounce_counter; 
} Line_Sensor_T;

static Line_Sensor_T line_sensor_array[] = {
    {LS1, false, 0},
    {LS2, false, 0},
    {LS3, false, 0},
    {LS4, false, 0}
};

void update_sensor_status(){
    for(uint8_t i=0; i<num_elems(line_sensor_array); i++){
        bool new_status = PIND & SB(line_sensor_array[i].pin);
        if(line_sensor_array[i].status == new_status){
            line_sensor_array[i].debounce_counter = 0;
        } else if(line_sensor_array[i].debounce_counter >= DEBOUNCE_COUNTER_MAX){
            line_sensor_array[i].status = new_status;
            line_sensor_array[i].debounce_counter = 0;
        } else
            line_sensor_array[i].debounce_counter++;
    }
}

uint8_t line_sensor_get_status(){
    uint8_t result = 0;
    for(uint8_t i=0; i<num_elems(line_sensor_array); i++){
        result = line_sensor_array[i].status<<i;
    }
    if(result)
        serial_data_uint8("Line detected! Status ", &result, 1);
    return result;
}