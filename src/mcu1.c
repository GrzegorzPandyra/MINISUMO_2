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
    log_info_P(MCU1_ONLINE);
    uint16_t DS_reading = 0;
    uint8_t LS_readings = 0;
    _delay_ms(5000);
    // AI_init();
    log_info("READY");
    while(1)
    { 
        // _delay_ms(5);
        // DS_reading = distance_sensor_get_status();
        // LS_readings = line_sensor_get_status();
        // AI_run(LS_readings, DS_reading);
        // uint8_t x = 55;
        // char z = 'z';
        // char tab[] = "tablica";
        // int y = 20; //0x14
        // log_info_1("x=%d", x);
        // log_info_1("z=%c", z);
        // log_info_1("tab=%s", tab);
        // log_info_1("y=%d", y);
        // log_info_1("y hex=%x", y);
        // sprintf(data_conversion_buffer, "raw x=%x", x);
        // serial_log(get_metadata(INFO), data_conversion_buffer);
    }
    return 0;
}