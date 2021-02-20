/*! @file mcu1.c
    @brief Main loop
    Starting point of software for MCU1
*/

#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_tx.h"
#include "ISR.h"
#include "distance_sensor.h"
#include "line_sensor.h"
#include "ADC.h"
#include "AI.h"


static uint16_t DS_reading = 0;
static uint8_t LS_readings = 0;

/**
 * @brief Main function
 */ 
int main(){
    PORTC |= 1<<MASTER_INIT;
    serial_init(F_CPU, BAUD);
    ICCM_init();
    ADC_init();
    sei();
    log_info_P(PROGMEM_MCU1_ONLINE);
    print_AI_status();
    while(1)
    { 
        if((PINC & (1<<MASTER_INIT)) == 0){
            if(AI_get_status() == AI_IDLE){
                AI_init();
            } else {
                AI_force_stop();
                print_AI_status();
            }
        }

        switch (AI_get_status()){
            case AI_SEARCH:
            case AI_ATTACK:
            case AI_R2R:
                DS_reading = distance_sensor_get_status();
                LS_readings = line_sensor_get_status();
                log_data_1("LS status=%d", LS_readings);
                AI_run(LS_readings, DS_reading);
                break;
            case AI_IDLE:
            case AI_ARMED:
                /* Do nothing */
                break;
            default:
                break;
        }
    }
    return 0;
}