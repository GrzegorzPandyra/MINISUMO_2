#ifndef SERIAL_INTERRUPTS_GUARD
#define SERIAL_INTERRUPTS_GUARD

/** @file serial_interrupts.h.c
    @brief Interrupts for serial communication
*/

#include "serial_interface.h"

/**
 * @brief Interrupt routine for USART receive complete bit
 * Reads a single character from UDR and calls function to store character c in rx_buffer
 */
ISR(USART_RXC_vect)
{
    char c; 
    c = UDR;
    UDR = c;
    serial_receive_char(c);
}
#endif /* SERIAL_INTERRUPTS_GUARD */
