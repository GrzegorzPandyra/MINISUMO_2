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
    uint8_t i;

    // cli();
    // PORTB |= 1<<PB0;
    // GICR &= ~(1<<INT0);
    // _delay_ms(200);
    char c = 0;
    uint8_t q = 0;
    uint8_t d = 0;
    _delay_ms(200);
    for(i = 0; i < 8; i++){
        c |= (PIND & (1<<ICCM_RX))<<i;
        q = (PIND & (1<<ICCM_RX));
        // serial_warn("bit received");
        // serial_data_int(&i, 1);
        if(q != 0){
            serial_warn(" 1");
            d++;
        } else {
            serial_warn(" 0");
        }
        q = 0;
        _delay_ms(200);
    }
    // if(d==0){
    // serial_data_str(&c, 1);
    // }
    if(d==3){
        serial_warn("ok");
        serial_data_str(&c, 1);
    }
    // _delay_ms(1000);
    // serial_warn("INT0 triggered");
    // serial_data_str(&c, 1);
    // GICR |= 1<<INT0;
    // sei();
    // serial_info("read character:");
    // serial_data_str(&c, 1);
    // _delay_ms(1000);
    // iccm_receive_char(c);
    // if(c == '\0'){
        // iccm_set_rx_complete_flag(1);
    // }
    // PORTB &= ~(1<<PB0);
}
#endif /* ISR_GUARD */
