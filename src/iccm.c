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
#define NULL 0

/* Local macro-like functions */
#define SB(x) (1<<(x))          //set bit
#define CB(x) (~(1<<(x)))       //clear bit

/* Local static variables */
static char iccm_rx_buffer[ICCM_RX_BUFFER_LENGTH] = {'\0'};
static char *iccm_rx_buffer_ptr = iccm_rx_buffer;
static uint8_t rx_complete_flag = 0;

/* Global variables */
/* Local static functions */

/**
 * @brief Search for NULL character in string, returns 0 if no null found in MAX_STRING_LENGTH characters
 * @param str String to be searched
 * @return 0 if NULL character was not found, otherwise number of characters
 */
static uint8_t get_str_length(char *str){
    uint8_t i = 0;
    while(i < MAX_STRING_LENGTH && *str != NULL_CHAR){
        i++;
        str++;
    }
    return (*str == NULL_CHAR)?i:0;
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
    serial_data_str("Sending data:", str, get_str_length(str));
    for(uint8_t cnt = 0; *str != NULL_CHAR && cnt < ICCM_MAX_DATA_LENGTH; cnt++, str++){
        ICCM_DataFrame_T frame = create_frame(*str);
        transmit(frame);
    }
}

/**
 * @brief Stores single char in iccm_rx_buffer
 * Used by ISR to put received character to iccm_rx_buffer. 
 * @param c         Char to be stored
 * @return uint8_t  Result if char could be stored or if buffer is full
 */
uint8_t iccm_receive_char(char c){
    uint8_t result = 0;
    if(*iccm_rx_buffer_ptr == iccm_rx_buffer[ICCM_RX_BUFFER_LENGTH-1]){
        // serial_warn("ICCM RX BUFFER OVERFLOW");
    }else{
        *iccm_rx_buffer_ptr = c;
        iccm_rx_buffer_ptr++;
        result = 1;
        serial_info( iccm_rx_buffer_ptr-1);
    }
    return result;
}

void iccm_receive(void){
    if(rx_complete_flag){
        iccm_rx_buffer_ptr = iccm_rx_buffer;
        serial_info("Reading RX buffer...");
        serial_info(iccm_rx_buffer_ptr);
        iccm_rx_buffer_ptr = NULL;
        iccm_set_rx_complete_flag(0);
    }
}

void iccm_set_rx_complete_flag(uint8_t value){
    rx_complete_flag = value;
    // serial_info("rx flag changed");
}


uint8_t cnt = 0;
void iccm_on_receive(char c){
    
    // serial_info("ISR triggered");
    char cc = 0;
    // int data_arr[8] = {0};
    serial_enable_buffering();
    _delay_ms(ICCM_DELAY*3/2);
    for(uint8_t i = 0; i < ICCM_DATA_SIZE; i++){
        cc |= (PIND & SB(ICCM_RX)) ? SB(i) : c;
        // data_arr[ICCM_DATA_SIZE-1-i] = (PIND & SB(ICCM_RX))>>ICCM_RX;
        _delay_ms(ICCM_DELAY);
        // if(i == ICCM_DATA_SIZE -1 ){
        //     _delay_ms(ICCM_DELAY*3);
        // }
    }
    GIFR |= 1<<INTF0;
    // _delay_ms(ICCM_DELAY*2);
    // serial_data_int(":",data_arr, 8);
    serial_data_str(":", &cc, 1);
    // serial_data_uint8("cnt:", &cnt, 1);
    serial_disable_buffering();
    cnt++;  
    if(cnt == 3 ){
        cnt = 0;
        serial_read_tx_buffer();
    }
}





