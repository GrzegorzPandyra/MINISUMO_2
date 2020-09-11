/*! @file iccm.c
    @brief Inter-Chip Communication Manager
*/

#include "iccm.h"
#include "config.h"
#include <util/delay.h>
#include <avr/io.h>
#include "serial_tx.h"

/* Local macro definitions */
#define ICCM_RX_BUFFER_LENGTH 20
#define MAX_STRING_LENGTH 200
#define NULL 0
#define INCLUDE_NULL_CHAR 1

/* Local macro-like functions */
#define SB(x) (1<<(x))          //set bit
#define CB(x) (~(1<<(x)))       //clear bit

/* Local static variables */
static char iccm_rx_buffer[ICCM_RX_BUFFER_LENGTH] = {'\0'};
static char *iccm_rx_buffer_head = iccm_rx_buffer;
static bool rx_complete_flag = false;

/* Global variables */
/* Local static functions */

/**
 * @brief Search for NULL character in string, returns 0 if no null found in MAX_STRING_LENGTH characters
 * @param str String to be searched
 * @return 0 if NULL character was not found, otherwise number of characters (including null-terminator)
 */
static uint8_t cstrlen(char *str){
    uint8_t str_len = 0;
    while(str_len < MAX_STRING_LENGTH && *str != NULL){
        str_len++;
        str++;
    }
    return (*str == NULL)?str_len+INCLUDE_NULL_CHAR:0;
}

/**
 * @brief Creates a data frame to be send via ICCM_TX pin
 * @param c Character to be send
 */
static ICCM_DataFrame_T create_frame(char c){
    ICCM_DataFrame_T frame = {
        .struct_bits = {
            .start_bit = ICCM_START_BIT, 
            .data = (uint8_t)c,
            .stop_bit = ICCM_STOP_BIT
        }
    };
    return frame;
}

/**
 * @brief Transmits frame bits into a sequence of high and low voltage states on ICCM_TX pin
 * Data is send starting from far right bit. ICCM_DELAY must be the same on both MCUs.
 * @param frame Data to be send to another MCU
 */
static void transmit(ICCM_DataFrame_T frame){
    for(uint8_t i = 0; i < ICCM_FRAME_SIZE; i++){
        uint8_t current_bit = (frame.raw_bits>>i) & 0x0001;
        if(current_bit){
            PORTD |= SB(ICCM_TX);
        } else {
            PORTD &= CB(ICCM_TX);
        }
        _delay_ms(ICCM_DELAY);
    }
    PORTD &= CB(ICCM_TX);
}

/**
 * @brief Checks if iccm_rx_buffer is full
 */
bool static is_iccm_rx_buffer_full(void){
    return (iccm_rx_buffer_head >= (iccm_rx_buffer+ICCM_RX_BUFFER_LENGTH));
}

/**
 * @brief Stores single char in iccm_rx_buffer
 * @param c Char to be stored
 * @return True, if char could be stored or false, if buffer is full
 */
bool to_iccm_rx_buffer(const char c){
    if(is_iccm_rx_buffer_full()){
        // serial_warn("ICCM RX BUFFER OVERFLOW");
        return false;
    }else{
        *iccm_rx_buffer_head = c;
        iccm_rx_buffer_head++;
        return true;
    }
}

/**
 * @brief Read byte on ICCM_RX pin
 * This function is triggered by ISR of ICCM_RX pin. Function periodically reads the state of ICCM_RX pin to decode a data frame transmitted from second MCU.
 *      _   _     __   _
 * ____| |_| |___|  |_| |___
 * 1start    8data    1stop
 * Frame consists of START, DATA and STOP bits (in that order). Initial START bit triggers the ISR and is skipped during read process (1.5 period wait to skip 
 * the start bit and read further bits somewhere in the middle of their state). Then 8 bits of data are being read and shifted into correct positions 
 * in response byte (from LSB to MSB). STOP bit is disregarded.
 * INT0 flag is set to make sure ISR won't execute twice.
 * WARNING: This code is time-sensitive, avoid printouts or setting delay too low
 * @return received byte 
 */
static char read_byte_on_pin(){
    char response_byte = 0;
    _delay_ms(ICCM_DELAY*3/2);
    for(uint8_t i = 0; i < ICCM_DATA_SIZE; i++){
        response_byte |= (PIND & SB(ICCM_RX)) ? SB(i) : response_byte;
        _delay_ms(ICCM_DELAY);
    }
    //Manually set INT0 Flag to ensure interrupt won't fire twice
    GIFR |= 1<<INTF0;
    return response_byte;
}

/* Global functions */

/**
 * @brief Initializes ICCM module
 * Configures rx/tx pins, enables interrupts
 * This module requires followind #defines to be created:
 * ICCM_RX, ICCM_TX, ICCM_DELAY
 */
void iccm_init(void){
    //configure INT0 to activate on rising edge 
    MCUCR |= SB(ISC01) | SB(ISC00);
    //enable INT0
    GICR |= SB(INT0);
    //set rx as input
    DDRD &= CB(ICCM_RX);
    //set tx as output
    DDRD |= SB(ICCM_TX);
    //set SW_TX low
    PORTD &= CB(ICCM_TX);
}

/**
 * @brief Sends string str to another MCU
 * Packs each character of the str into a data frame and sends it bit-by-bit. Data is received by another MCU and handled by ISR.
 * @param str String to be send
 */
void iccm_send(char *str){
    uint8_t str_len = cstrlen(str);
    serial_data_str_P(ICCM_SENDING_DATA, str, str_len);
    for(uint8_t cnt = 0; cnt < str_len && cnt < ICCM_MAX_TRANSMIT_DATA_LENGTH; cnt++, str++){
        ICCM_DataFrame_T frame = create_frame(*str);
        transmit(frame);
    }
}


/**
 * @brief Reads the contents of iccm_rx_buffer and clears it.
 */
void iccm_read_iccm_rx_buffer(void){
    uint8_t str_len = cstrlen(iccm_rx_buffer);
    if(str_len > 0){
        serial_data_str_P(ICCM_RX_BUFFER_DATA ,iccm_rx_buffer, str_len);
        iccm_rx_buffer_head = iccm_rx_buffer;
        rx_complete_flag = false;
    }
}

/**
 * @brief Checks if iccm_rx_buffer has received any data
 */
bool iccm_is_data_available(void){
    return rx_complete_flag;
} 

/**
 * @brief Local RX ISR handler
 * NULL determines the end of data transfer which is marked by rx_complete_flag.
 */
void iccm_on_rx_trigger(){
    const char c = read_byte_on_pin();
    if(to_iccm_rx_buffer(c)){
       if(c == NULL){
           rx_complete_flag = true;
       } 
    } else {
        serial_err_P(ICCM_RX_BUFFER_OVERFLOW);
    }
}





