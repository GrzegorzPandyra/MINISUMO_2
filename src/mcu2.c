/*! @file mcu2.c
    @brief Main loop
    Starting point of software for MCU2
*/

/* Local macro definitions */
#define MCU2
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

/**
 * @brief Main function
 */ 
int main(){
    char data[] = "Hello from MCU2";
    serial_init(F_CPU, BAUD);
    sei();
    DDRB |= 0x01;

    while(1) /* Loop the messsage continously */
    { 
        serial_info(data);
        _delay_ms(1000);
        iccm_receive();      
    }
    return 0;
}