/*! @file mcu1.c
    @brief Main loop
    Starting point of software for MCU1
*/

/* Local macro definitions */
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
#include "line_sensor.h"


/**
 * @brief Main function
 */ 
int main(){
    serial_init(F_CPU, BAUD);
    iccm_init();
    sei();
    DDRB |= 0x01;
    serial_info_P(MCU1_ONLINE);
    while(1) /* Loop the messsage continously */
    { 
        _delay_ms(200);
        // iccm_send("D11111112222222");
        // if(iccm_is_data_available()){
            // iccm_read_rx_buffer();
        // }
        line_sensor_get_status();
    }
    return 0;
}