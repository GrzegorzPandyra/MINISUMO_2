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
#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))
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

static void force_bit(uint8_t *result, bool bit_status, uint8_t index)
{
    if(result != NULL){
        if(bit_status)
            *result |= SB(index);
        else
            *result &= CB(index);
    }
}

void update_sensor_status(){
    for(uint8_t i=0; i<NUM_ELEMS(line_sensor_array); i++){
        bool new_status = !(PIND & SB(line_sensor_array[i].pin));
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
    for(uint8_t i=0; i<NUM_ELEMS(line_sensor_array); i++)
        force_bit(&result, line_sensor_array[i].status, i);

    if(result & 0x01)
        serial_info("LS1");
    if(result & 0x02)
        serial_info("LS2");
    if(result & 0x04)
        serial_info("LS3");
    if(result & 0x08)
        serial_info("LS4");
    return result;
}