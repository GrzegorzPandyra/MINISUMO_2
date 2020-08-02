#ifndef ISR_GUARD
#define ISR_GUARD

/** @file ISR.h
    @brief Interrupt Service Routines
    This file needs to be included in file with "main" function. ISRs are kept separated here for the purpose of clarity. 
*/

#include "serial_interface.h"
#include "iccm.h"

#define NULL 0

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

/**
 * @brief Interrupt routine for ICCM RX pin (INT0)
 * Rising edge on RX pin triggers sampling of RX pin signal. Routine will read defined number of bits equal to the size of data frame. 
 * Then data (one character) will be copied to rx_buffer in ICCM module.
 * This function is doubled, since on each MCU different pin is used for interrupt.
 */
ISR(INT0_vect){
    // uint8_t i;

    // PORTB |= 1<<PB0;
    serial_warn("INT0 triggered");
    // _delay_ms(50);
    // char c = 0;
    // for(i = 0; i < 8; i++)
    // {
    //     c |= (PIND & (1<<ICCM_RX))<<i;
    //     serial_warn("bit received");
    //     _delay_ms(50);
    // }
    // _delay_ms(1000);
    // iccm_receive_char(c);
    // if(c == NULL){
    //     iccm_set_rx_complete_flag(1);
    // }
    // PORTB &= ~(1<<PB0);
}
#endif /* ISR_GUARD */
