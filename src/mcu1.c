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
#include "ADC.h"
#include "AI.h"

/**
 * @brief Main function
 */ 
int main(){
    /* Initialization */
    PORTB |= 1<<MASTER_INIT;
    serial_init(F_CPU, BAUD);
    ICCM_init();
    ADC_init();
    distance_sensor_init();
    sei();
    log_info_P(PROGMEM_MCU1_ONLINE);
    /* MCU1 start processing */
    print_AI_status();
    while(1){ 
        AI_run();
    }
    return 0;
}