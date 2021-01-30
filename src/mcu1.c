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

void set_debug_pin(){
    PORTC |= 1<<PC3;
}

void clear_debug_pin(){
    PORTC &= ~(1<<PC3);
}

void logic(){
    uint8_t ls_status = line_sensor_get_status();
    if(ls_status)
        set_debug_pin();
    else
        clear_debug_pin();
    
    if(ls_status & 0x01 && ls_status & 0x02)
        iccm_send("b");
    else if(ls_status & 0x04 && ls_status & 0x08)
        iccm_send("f");
    else if(ls_status & 0x01 && ls_status & 0x08)
        iccm_send("l");
    else if(ls_status & 0x02 && ls_status & 0x04)
        iccm_send("r");
}

/**
 * @brief Main function
 */ 
int main(){
    serial_init(F_CPU, BAUD);
    iccm_init();
    sei();
    DDRB |= 0x01;
    DDRC |= 1<<PC3;
    serial_info_P(MCU1_ONLINE);
    while(1) /* Loop the messsage continously */
    { 
        // _delay_us(200);
        // iccm_send("D11111112222222");
        // if(iccm_is_data_available()){
            // iccm_read_rx_buffer();
        // }
        logic();
    }
    return 0;
}