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
#define ICCM_TX_DELAY 50
#define MAX_STRING_LENGTH 200
#define NULL_CHAR '\0'
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
    while(str_len < MAX_STRING_LENGTH && *str != NULL_CHAR){
        str_len++;
        str++;
    }
    return (*str == NULL_CHAR)?str_len+INCLUDE_NULL_CHAR:0;
}

/**
 * @brief Creates a data frame to be send
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
 * @brief Transmits frame bits into a sequence of high and low voltage states on TX pin
 * Data is send starting from far right bit. ICCM_DELAY must be the same on both MCUs.
 * Final delay is to allow other MCU to process data before sending more.
 * @param frame Data to be send to another MCU
 */
static void transmit(ICCM_DataFrame_T frame){
    int data_arr[10] = {0};
    for(uint8_t i = 0; i < ICCM_FRAME_SIZE; i++){
        uint8_t current_bit = (frame.raw_bits>>i) & 0x0001;
        if(current_bit){
            PORTD |= SB(ICCM_TX);
        } else {
            PORTD &= CB(ICCM_TX);
        }
        data_arr[i] = current_bit;
        _delay_ms(ICCM_DELAY);
    }
    serial_data_int("send data:",data_arr, 10);
    PORTD &= CB(ICCM_TX);
    _delay_ms(ICCM_DELAY*20);
}

bool static is_iccm_rx_buffer_full(void){
    return (iccm_rx_buffer_head >= (iccm_rx_buffer+ICCM_RX_BUFFER_LENGTH));
}

/**
 * @brief Stores single char in iccm_rx_buffer
 * @param c         Char to be stored
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
    serial_data_str("Sending data:", str, str_len);
    for(uint8_t cnt = 0; cnt < str_len && cnt < ICCM_MAX_TRANSMIT_DATA_LENGTH; cnt++, str++){
        ICCM_DataFrame_T frame = create_frame(*str);
        transmit(frame);
    }
}



void iccm_read_iccm_rx_buffer(void){
    uint8_t str_len = cstrlen(iccm_rx_buffer);
    if(str_len > 0){
        serial_info("Reading RX buffer...");
        serial_info(iccm_rx_buffer);
        iccm_rx_buffer_head = iccm_rx_buffer;
        rx_complete_flag = false;
    }
}

static char receive_char(){
    // serial_info("ISR triggered");
    char c = 0;
    // int data_arr[8] = {0};
    serial_enable_buffering();
    _delay_ms(ICCM_DELAY*3/2);
    for(uint8_t i = 0; i < ICCM_DATA_SIZE; i++){
        c |= (PIND & SB(ICCM_RX)) ? SB(i) : c;
        // data_arr[ICCM_DATA_SIZE-1-i] = (PIND & SB(ICCM_RX))>>ICCM_RX;
        _delay_ms(ICCM_DELAY);
        // if(i == ICCM_DATA_SIZE -1 ){
        //     _delay_ms(ICCM_DELAY*3);
        // }
    }
    GIFR |= 1<<INTF0;
    // _delay_ms(ICCM_DELAY*2);
    // serial_data_int(":",data_arr, 8);
    serial_data_str(":", &c, 1);
    // to_iccm_rx_buffer(c);
    // serial_data_uint8("cnt:", &cnt, 1);
    serial_disable_buffering();
    // cnt++;  
    // if(cnt == 3 ){
    //     cnt = 0;
    //     serial_read_tx_buffer();
    // }
    return c;
}

// uint8_t cnt = 0;
void iccm_on_rx_trigger(){
    char c = receive_char();
    if(to_iccm_rx_buffer(c)){
       if(c == NULL_CHAR){
           rx_complete_flag = true;
       } 
    } else {
        serial_err("ICCM RX BUFFER OVERFLOW");
    }
}

bool iccm_is_data_available(void){
    return rx_complete_flag;
}




