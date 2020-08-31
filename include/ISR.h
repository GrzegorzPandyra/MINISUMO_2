#ifndef ISR_GUARD
#define ISR_GUARD

/** @file ISR.h
    @brief Interrupt Service Routines
    This file needs to be included in file with "main" function. ISRs are kept separated here for the purpose of clarity. 
*/

#include "serial_tx.h"
#include "serial_rx.h"
#include "iccm.h"

#define NULL 0

/**
 * @brief Interrupt routine for USART receive complete bit
 * Reads a single character from UDR and calls function to store character c in rx_buffer
 */
ISR(USART_RXC_vect){
    char c = (char)UDR;
    // serial_on_receive(c);
    if(c == '1'){
        serial_read_tx_buffer();
    } else if(c == '2'){
        serial_clear_tx_buffer();
    } else if(c == '3'){
        serial_read_rx_buffer();
    } else if(c == '4'){
        serial_clear_rx_buffer();
    } else if(c == '5'){
        serial_disable_buffering();
    } else if(c == '6'){
        serial_enable_buffering();
    } else if(c == '7'){
        UDR = '7';
    } else if(c == '8'){
        if(serial_is_tx_buffer_full()){
            serial_info("tx buffer full");
        } else {
            serial_info("tx buffer not full");
        }
    } else if(c == '9'){
        if(serial_is_rx_buffer_full()){
            serial_info("rx buffer full");
        } else {
            serial_info("rx buffer not full");
        }
    } if (c == '0'){
        if(sizeof("12345") == 6){
            serial_info("sizeof = 6");
        }
        if(sizeof("12345") == 5){
            // serial_info("sizeof = 5");
        }
    }
}

/**
 * @brief Interrupt routine for ICCM RX pin (INT0)
 * Rising edge on RX pin triggers sampling of RX pin signal. Routine will read defined number of bits equal to the size of data frame. 
 * Then data (one character) will be copied to rx_buffer in ICCM module.
 * This function is doubled, since on each MCU different pin is used for interrupt.
 */
ISR(INT0_vect)
{
    uint8_t i;
    _delay_ms(50);
    char c = 0;
    for(i = 0; i < 8; i++)
    {
        c |= (PINB & (1<<SW_RX))<<i;
        serial_warn("bit received");
        _delay_ms(50);
    }
    _delay_ms(50);
    _delay_ms(50);
    iccm_receive_char(c);
    if(c == NULL)
    {
        iccm_set_rx_complete_flag(1);
    }
}

#endif /* ISR_GUARD */
