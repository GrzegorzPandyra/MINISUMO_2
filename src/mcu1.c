/*! @file mcu1.c
    @brief Main loop
    Starting point of software for MCU1
*/

#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial_tx.h"
#include "ISR.h"
#include "ICCM.h"
#include "distance_sensor.h"
#include "line_sensor.h"
#include "ADC.h"
#include "AI.h"

#include "stdio.h"

/**
 * @brief Main function
 */ 
int main(){
    serial_init(F_CPU, BAUD);
    ICCM_init();
    ADC_init();
    sei();
    serial_info_P(MCU1_ONLINE);
    uint16_t DS_reading = 0;
    uint8_t LS_readings = 0;
    _delay_ms(5000);
    AI_init();
    serial_info("READY");
    while(1)
    { 
        _delay_ms(5);
        DS_reading = distance_sensor_get_status();
        LS_readings = line_sensor_get_status();
        AI_run(LS_readings, DS_reading);
    }
    return 0;
}