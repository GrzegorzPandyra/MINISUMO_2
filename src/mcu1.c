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
#include "iccm.h"
#include "serial_interface.h"
#include "ISR.h"

/**
 * @brief Main function
 */ 
int main(){
    char str[] = "Hello from MCU1";
    char str2[] = "G";
    serial_init(F_CPU, BAUD);
    sei();
    int i = 0;
    DDRB |= 0x01;

    while(1) /* Loop the messsage continously */
    { 
        i = 0;
        serial_log(__FILE__, NOTIFY, str);
        iccm_send(str2);
        _delay_ms(500);
        PORTB &= 0x00;
        i++;
        _delay_ms(500);
        PORTB |= 0x01;                          
        // serial_read("qq");
    }
    return 0;
}