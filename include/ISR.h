#ifndef ISR_GUARD
#define ISR_GUARD

/** @file ISR.h
    @brief Interrupt Service Routines
    This file needs to be included in file with "main" function. ISRs are kept separated here for the purpose of clarity. 
*/

#include "serial_tx.h"
#include "serial_rx.h"
#include "iccm.h"
#include "config.h"

/* Local macro-like functions */
// #define SB(x) (1<<(x))    //set bit
// #define CB(x) (~(1<<(x))) //clear bit
// #define NULL 0

/**
 * @brief Interrupt routine for USART receive complete bit
 * Reads a single character from UDR and calls function handle this data
 */
ISR(USART_RXC_vect){
    char c = (char)UDR;
    serial_on_receive(c);
}

/**
 * @brief Interrupt routine for ICCM RX pin (INT0)
 * Rising edge on RX pin triggers sampling of RX pin signal. Routine will read defined number of bits equal to the size of data frame. 
 * Then data (one character) will be copied to rx_buffer in ICCM module.
 * This function is doubled, since on each MCU different pin is used for interrupt.
 */
ISR(INT0_vect){
    char c = (char)UDR;
    iccm_on_receive(c);
}

#endif /* ISR_GUARD */
