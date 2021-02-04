/*! @file distance_sensor.c
    @brief 
*/

#include "distance_sensor.h"
#include <avr/io.h>
#include "serial_tx.h"
#include "string.h"

#define num_elems(x) (sizeof(x)/sizeof(x[0]))
#define NUM_SAMPLES 10
#define VREF 256
// #define adc_start_conversion() (ADCSRA |= (1<<ADSC))
// #define adc_result_ready() (~(ADCSRA & (1<<ADSC)))

struct Distance_Sensor{
    uint8_t pin;
    uint8_t samples[NUM_SAMPLES];
    uint8_t num_samples_stored;
    uint8_t *sample_ptr;

};
static struct Distance_Sensor distance_sensor = {DS1, {0}, 0, distance_sensor.samples};

static uint8_t getAvg(uint8_t *samples, uint8_t num_samples_stored){
    volatile uint16_t result = 0;
    for(uint8_t i=0; i<num_samples_stored; i++)
        result += samples[i];
    serial_data_uint8("result ", &result, 1);
    return result/num_samples_stored;
}

void distance_sensor_update_status(void){
    volatile uint16_t adc_value = 0;
    // serial_data_uint8("adc_value0 ", &adc_value, 1);
    // adc_value = ((uint16_t)(ADCL | (ADCH<<8)));
    // adc_value = adc_value*256/102400;
    // adc_value = ADCH*256;
    // adc_value = (adc_value + ADCL);
    // adc_value = adc_value * 256 / 102400;
    /* ADCW is compiler help for reading ADCH & ADCL registers */
    adc_value = ADCW;//ADCW*5/1024;
    char str[10];
    sprintf(str, "%d", adc_value);
    // serial_data_uint8("adc_value1 ", &adc_value, 1);
    serial_data_str("adc_value ", str, 4);
    *(distance_sensor.sample_ptr) = adc_value;
    if(distance_sensor.sample_ptr == &(distance_sensor.samples[NUM_SAMPLES-1]))
        distance_sensor.sample_ptr = distance_sensor.samples;
    else
        distance_sensor.sample_ptr++;
    if(PINC & (1<<PC0))
        PORTC |= (1<<PC3);
    else
        PORTC &= ~(1<<PC3);
    

    // ADCSRA |= 1<<ADSC;
}

uint8_t distance_sensor_get_status(void){
    return getAvg(distance_sensor.samples, distance_sensor.num_samples_stored);
}