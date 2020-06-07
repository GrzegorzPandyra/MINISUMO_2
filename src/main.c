/*! @file main.c
      @brief Main loop
    
    Starting point of software
*/

// #include <avr/io.h>
// #include <util/delay.h>
// #include "config.h"

/* Local macro definitions */
#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1
#define FILE_ID "main.c"
/* Local macro-like functions */
/* Local static variables */
/* Global variables */
/* Local static functions */
/* Global functions */

// #define __AVR_ATmega8__

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial_interface.h"
#include "serial_interrupts.h"

/**
 * @brief Main function
 */ 
int main(){
    char data[] = "Hello from ATmega8";
    serial_init(F_CPU, BAUD);
    sei();
    int i = 0;
    DDRB |= 0x01;

    while(1) /* Loop the messsage continously */
    { 
        i = 0;
        serial_send(FILE_ID, NOTIFY, data);
        _delay_ms(500);
        PORTB &= 0x00;
            i++;
        _delay_ms(500);
        PORTB |= 0x01;                          
        // serial_read("qq");
    }
    return 0;
}