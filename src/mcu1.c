/*! @file mcu1.c
    @brief Main loop
    Starting point of software for MCU1
*/

/* Local macro definitions */
#define MCU1
/* Local macro-like functions */
/* Local static variables */
/* Global variables */
/* Local static functions */
/* Global functions */

#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial_tx.h"
#include "ISR.h"
#include "iccm.h"
#include "distance_sensor.h"

void adc_init(){
    /* Enable ADC*/
    ADCSRA |= 1<<ADEN;
}


/**
 * @brief Main function
 */ 
int main(){
    serial_init(F_CPU, BAUD);
    iccm_init();
    adc_init();
    sei();
    DDRB |= 0x01;
    serial_info_P(MCU1_ONLINE);
    uint8_t cnt = 0;
    uint8_t ds1_status = 0;
    while(1) /* Loop the messsage continously */
    { 
        _delay_ms(100);
        distance_sensor_update_status();
        if(cnt >= 10){
            cnt = 0;
            distance_sensor_get_status(&ds1_status);
        } else 
            cnt++;
        serial_data_uint8("DS1 status:", &ds1_status, 1);
        // iccm_send("D11111112222222");
        if(iccm_is_data_available()){
            iccm_read_rx_buffer();
        }
    }
    return 0;
}