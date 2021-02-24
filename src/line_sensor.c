/*! @file line_sensor.c
    @brief Handles line detection with 4 IR reflection sensors
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

/**
 * @brief Helper function to set bit regardless of the bit value
 */
static void force_bit(uint8_t *result, bool bit_status, uint8_t index)
{
    if(result != NULL){
        if(bit_status)
            *result |= SB(index);
        else
            *result &= CB(index);
    }
}

/**
 * @brief Reads all line sensors pins and provides simple debouncing
 */
static void update_sensor_status(){
    for(uint8_t i=0; i<NUM_ELEMS(line_sensor_array); i++){
        bool new_status = !(PIND & SB(line_sensor_array[i].pin));
        line_sensor_array[i].status = new_status;
    }
}

/**
 * @brief Returns byte where bits 0-3 describe status of respective line sensor: 1(line detected) or 0(line NOT detected)
 */
uint8_t line_sensor_get_status(){
    update_sensor_status();
    uint8_t result = 0;
    for(uint8_t i=0; i<NUM_ELEMS(line_sensor_array); i++)
        force_bit(&result, line_sensor_array[i].status, i);
    return result;
}