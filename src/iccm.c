/*! @file iccm.c
    @brief Inter-Chip Communication Manager
*/

#include "iccm.h"
#include "config.h"
#include <util/delay.h>
#include <avr/io.h>
#include "serial_interface.h"
/* Local macro definitions */
#define ICCM_RX_BUFFER_LENGTH 20
#define ICCM_TX_DELAY 50
#define NULL 0
/* Local macro-like functions */
#define SB(x) (1<<(x))          //set bit
#define CB(x) (~(1<<(x)))       //clear bit
/* Local static variables */
static ICCM_Register_T iccm_register;
static char rx_buffer[ICCM_RX_BUFFER_LENGTH] = {'\0'};
static char *rx_buffer_ptr = rx_buffer;
static uint8_t rx_complete_flag = 0;
/* Global variables */
/* Local static functions */
/* Global functions */

/**
 * @brief Initializes ICCM module
 * Configures rx/tx pins, enables interrupts
 * @param rx_pin    rx pin
 * @param tx_pin    tx pin
 * @param baudrate  baudrate
 */
void iccm_init(uint8_t rx_pin, uint8_t tx_pin, uint8_t baudrate)
{
    iccm_register.rx_pin = rx_pin;
    iccm_register.tx_pin = tx_pin;
    iccm_register.baudrate = baudrate;
    //enable interrupt on rx pin
    //set rx as input
    DDRD &= ~(1<<iccm_register.rx_pin);
    //set tx as output
    DDRD |= 1<<iccm_register.tx_pin;
    //set SW_TX low
    PORTD &= CB(iccm_register.tx_pin);
}

/**
 * @brief Sends string str to another MCU
 * Packs each character of the str into a data frame and sends it bit-by-bit. Data is received by another MCU and handled by ISR.
 * @param str String to be send
 */
void iccm_send(char *str)
{
    ICCM_DataFrame_T frame;
    uint8_t i,j;
    frame.struct_frame.start_bit = 1;
    frame.struct_frame.stop_bits = 3; //that's 11b
    //iterate over str, char by char
    for(i = 0; *(str+i) != '\0' && i < ICCM_MAX_DATA_LENGTH; i++)
    {
        //put char into data frame
        frame.struct_frame.data = *(str+i);
        //iterate bit-by-bit through frame
        for(j = 0; j < (ICCM_START_BIT+ICCM_DATA_SIZE+ICCM_NUM_STOP_BITS); j++)
        {
            PORTD = (1>>frame.raw_frame) & 0x01; 
            serial_info(str+i);
            _delay_ms(ICCM_TX_DELAY);
        }
    }
    PORTD &= CB(iccm_register.tx_pin);
}

/**
 * @brief Stores single char in rx_buffer
 * Used by ISR to put received character to rx_buffer. 
 * @param c         Char to be stored
 * @return uint8_t  Result if char could be stored or if buffer is full
 */
uint8_t iccm_receive_char(char c)
{
    uint8_t result = 0;
    if(*rx_buffer_ptr == rx_buffer[ICCM_RX_BUFFER_LENGTH-1])
    {
        serial_warn("ICCM RX BUFFER OVERFLOW");
    } 
    else 
    {
        *rx_buffer_ptr = c;
        rx_buffer_ptr++;
        result = 1;
        serial_info( rx_buffer_ptr-1);
    }
    return result;
}

void iccm_receive()
{
    if(rx_complete_flag)
    {
        rx_buffer_ptr = rx_buffer;
        serial_info( rx_buffer_ptr);
        rx_buffer_ptr = NULL;
        iccm_set_rx_complete_flag(0);
    }
}

void iccm_set_rx_complete_flag(uint8_t value)
{
    rx_complete_flag = value;
    serial_info("rx flag changed");
}






