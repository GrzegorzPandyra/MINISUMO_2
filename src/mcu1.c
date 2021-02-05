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
void logic(){
    uint8_t ls_status = line_sensor_get_status();
    if(ls_status & 0x01 && ls_status & 0x02)
        ICCM_send("b");
    else if(ls_status & 0x04 && ls_status & 0x08)
        ICCM_send("f");
    else if(ls_status & 0x01 && ls_status & 0x08)
        ICCM_send("l");
    else if(ls_status & 0x02 && ls_status & 0x04)
        ICCM_send("r");
}

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
    while(1)
    { 
        DS_reading = distance_sensor_get_status();
        LS_readings = line_sensor_get_status();
        AI_run(LS_readings, DS_reading);
    }
    return 0;
}