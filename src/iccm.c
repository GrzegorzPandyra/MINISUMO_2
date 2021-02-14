/*! @file iccm.c
    @brief Inter-Chip Communication Manager
*/

#include "ICCM.h"
#include "config.h"
#include <util/delay.h>
#include <avr/io.h>
#include "serial_tx.h"
#include "string.h"
#include "ICCM_message_catalog.h"

/* Local macro definitions */
#define ICCM_RX_BUFFER_SIZE 20
#define MAX_STRING_LENGTH ICCM_RX_BUFFER_SIZE
#define NULL_CHAR 0
#define STX 0x02
#define ETX 0x03

/* Local macro-like functions */
#define SB(x) (1<<(x))          /* set bit   */
#define CB(x) (~(1<<(x)))       /* clear bit */

/* Local static variables */
static char rx_buffer[ICCM_RX_BUFFER_SIZE] = {0};
static char *rx_buffer_head = rx_buffer;
static ICCM_Status_T iccm_status = IDLE;
static bool rx_complete = false;

/* Global variables */
/* Local static functions */

/**
 * @brief Search for NULL character in string, returns 0 if no null found in MAX_STRING_LENGTH characters
 * @param str String to be searched
 * @return 0 if NULL character was not found, otherwise number of characters (excluding null-terminator)
 */
static uint8_t cstrlen(char *str){
    uint8_t str_len = 0;
    while(str_len < MAX_STRING_LENGTH){
        if(*str == NULL_CHAR){
            return str_len;
        }
        str_len++;
        str++;
    }
    return 0;
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
 * Data is send starting from far right bit. ICCM_DELAY_US must be the same on both MCUs.
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
        _delay_us(ICCM_DELAY_US);
    }
    PORTD &= CB(ICCM_TX);
}

/**
 * @brief Checks if rx_buffer is full
 */
bool static is_rx_buffer_full(void){
    return (rx_buffer_head >= (rx_buffer+ICCM_RX_BUFFER_SIZE));
}

/**
 * @brief Attempts to store a single char in rx_buffer. Keeps buffer null-terminated. 
 * @param c Char to be stored
 * @return True, if char could be stored or false, if buffer is full
 */
bool to_rx_buffer(const char c){
    if(is_rx_buffer_full()){
        log_err_P(ICCM_RX_BUFFER_OVERFLOW);
        rx_buffer[ICCM_RX_BUFFER_SIZE-1] = NULL_CHAR;
        return false;
    }else{
        *rx_buffer_head = c;
        rx_buffer_head++;
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
    _delay_us(ICCM_DELAY_US*3/2);
    for(uint8_t i = 0; i < ICCM_DATA_SIZE; i++){
        response_byte |= (PIND & SB(ICCM_RX)) ? SB(i) : response_byte;
        _delay_us(ICCM_DELAY_US);
    }
    /* Manually set INT0 Flag to ensure interrupt won't fire twice */
    GIFR |= 1<<INTF0;
    return response_byte;
}

/* Global functions */

/**
 * @brief Initializes ICCM module
 * Configures rx/tx pins, enables interrupts
 * This module requires followind #defines to be created:
 * ICCM_RX, ICCM_TX, ICCM_DELAY_US
 */
void ICCM_init(void){
    log_info_P(ICCM_INIT);
    /* configure INT0 to activate on rising edge  */
    MCUCR |= SB(ISC01) | SB(ISC00);
    /* enable INT0 */
    GICR |= SB(INT0);
    /* set rx as input */
    DDRD &= CB(ICCM_RX);
    /* set tx as output */
    DDRD |= SB(ICCM_TX);
    /* set SW_TX low */
    PORTD &= CB(ICCM_TX);
}

/**
 * @brief Sends string str to another MCU
 * Packs each character of the str into a data frame and sends it bit-by-bit. Data is received by another MCU and handled by ISR.
 * Data is appended with start character STX (0x02) and end character ETX (0x03)
 * @param str String to be send
 */
void ICCM_send(char *str){
    iccm_status = TX_IN_PROGRESS;
    uint8_t str_len = cstrlen(str);
    // log_info_P(ICCM_SENDING_DATA, str, str_len);
    
    ICCM_DataFrame_T start_frame = create_frame(STX);
    transmit(start_frame);
    for(uint8_t cnt = 0; cnt < str_len && cnt < ICCM_RX_BUFFER_SIZE; cnt++, str++){
        ICCM_DataFrame_T frame = create_frame(*str);
        transmit(frame);
    }
    ICCM_DataFrame_T end_frame = create_frame(ETX);
    transmit(end_frame);

    iccm_status = IDLE;
}


/**
 * @brief Reads the contents of rx_buffer and clears it.
 */
void ICCM_read_rx_buffer(char *buff_out, uint8_t *data_length){
    uint8_t str_len = cstrlen(rx_buffer);
    if(str_len > 0){
        memcpy(buff_out, rx_buffer, str_len);
        *data_length = str_len;
        log_data_1("%s", rx_buffer);
        ICCM_clear_rx_buffer();
    }
}

/**
 * @brief Checks if rx_buffer has received any data
 */
bool ICCM_is_data_available(void){
    return rx_complete;
} 

/**
 * @brief Local RX ISR handler
 * STX indicates start of data, ETX indicates the end. If no STX is received, then received data is invalid. If TX is in progress, ISR is disregarded.
 */
void ICCM_on_rx_trigger(void){
    if(iccm_status == TX_IN_PROGRESS || iccm_status == DISABLED)
        return;
 
    static bool stx_received = false;
    const char c = read_byte_on_pin();
 
    switch (c){
    case STX:
        iccm_status = RX_IN_PROGRESS;
        stx_received = true;
        ICCM_clear_rx_buffer();
        break;
    case ETX:
        if(stx_received){
            to_rx_buffer(NULL_CHAR);
            rx_complete = true;
            stx_received = false;
            iccm_status = IDLE;    
        }
        break;
    default:
        if(stx_received)
            to_rx_buffer(c);
        break;
    }
}

/**
 * @brief Buffer is considered "clear" when head points to buffer start and has value of NULL_CHAR
 */
void ICCM_clear_rx_buffer(void){
    rx_complete = false;
    rx_buffer_head = rx_buffer;
    *rx_buffer_head = NULL_CHAR;
}

void ICCM_disable(void){
    iccm_status = DISABLED;
}

void ICCM_enable(void){
    iccm_status = IDLE;
}