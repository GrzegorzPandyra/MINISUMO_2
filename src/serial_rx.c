/** @file serial_rx.c
*   @brief Implementation of serial communication 
*   Source file implementing communication between MCU and PC.
*   @TODO add \n for sending data, change order of args in serial send data,
*   delete serial_log_data reference
*   
*   TX operation
*   To send data over UART use serial_send(src,type,str) function. Str must be a null-terminated string. 
*   All arguments are written to USART's UDR char-by-char.
*   
*   RX operation
*   Data is received by ISR defined in serial_interrupts.h ISR accesses serial_receiver_char(c) to write received character to rx_buffer.
*   Once all data is stored in rx_buffer, rx_buffer_ready flag is set, to indicate that data is ready to be processed. For that, serial_read()
*   function is called periodically. IMPORTANT: Rx data must be newline-terminated (configure your terminal to add LF at the end of RX data).
*/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "common_const.h"
#include "serial_rx.h"
#include "serial_tx.h"
// #include <string.h>

/* Local macro definitions */
#define RX_BUFFER_SIZE 20
#define CMD_MAX_STRING_LENGTH 30

/* Local functions and macro-like functions */
#define arr_length(arr) (sizeof(arr)/sizeof(arr[0]))

static char rx_buffer[RX_BUFFER_SIZE] = {0};
static char *rx_buffer_head = rx_buffer;

static bool str_cmp(const char *str1, const char *str2);
// static void on_dummy_func_1(void);
// static void on_dummy_func_2(void);
static void to_udr(const unsigned char c);
// static void clear_rx_buffer(void);
/* Local static variables */

typedef struct Cmd_Record_Tag{
    char string[CMD_MAX_STRING_LENGTH];
    void (*callback)();
} Cmd_Record_T;

const Cmd_Record_T cmd_list[] = {
    // {"serial enable buffering", serial_enable_buffering},
    // {"serial disable buffering", serial_disable_buffering},
    // {"serial read tx buffer", serial_read_tx_buffer},
    // {"serial dummy func 1", on_dummy_func_1},
    // {"sdf2", on_dummy_func_2}
};


/* Global variables */
/* Local static functions */
static void to_rx_buffer(const char c);
static void parse_cmd(const char *str);

// static void on_dummy_func_1(void){
//     serial_info("dummy1");
//     serial_read_tx_buffer();
// }

// static void on_dummy_func_2(void){
//     serial_info("dummy2");
//     serial_read_tx_buffer();
// }

static void to_rx_buffer(const char c){
    if(!serial_is_rx_buffer_full()){
        *rx_buffer_head = c;
        rx_buffer_head++;
        *rx_buffer_head = NULL_CHAR;
    }
}

static void parse_cmd(const char *str){
    for(uint8_t i = 0; i < arr_length(cmd_list); i++){
        // serial_data_str("rx bufer", rx_buffer, 10);
        // serial_log_data( (Log_Metadata_T){"", "", INFO}, "str1", (Data_T){cmd_list[i].string, i, STRING} );
        // serial_log_data( (Log_Metadata_T){"", "", INFO}, "str2", (Data_T){str, i, STRING} );
        serial_data_str("str1", (char*)cmd_list[i].string, i);
        serial_data_str("str2", (char*)str, i);
        if(str_cmp(cmd_list[i].string, str)){
            serial_info("found cmd");
            cmd_list[i].callback();
            break;
        }
    }
    // serial_info("cmd not found");
}

static bool str_cmp(const char *str1, const char *str2){
    for(; *str1 == *str2 && *str1 != NULL_CHAR && *str2 != NULL_CHAR; str1++, str2++){
        if(*str1 == NULL_CHAR && *str2 == NULL_CHAR)
            return true;    
    }
    return false;
}

static void to_udr(const unsigned char c){
    while ( !( UCSRA & (1<<UDRE)) )
    ;
    UDR = c;
}



/* Global functions */

/**
 * @brief Returns if rx buffer is full
 */
bool serial_is_rx_buffer_full(void){
    return (rx_buffer_head >= rx_buffer+RX_BUFFER_SIZE);
}

/**
 * @brief Puts single character c into rx_buffer
 * Used by USART RX ISR to store data from RX channel. 
 * @param c Char received from ISR
 */
void serial_on_receive(const char c){
    // serial_info("on receive");
    to_rx_buffer(c);
    // parse_cmd(rx_buffer);
    // clear_rx_buffer();
}
void serial_read_rx_buffer(void){
    if(rx_buffer_head != rx_buffer){
        char *rx_buffer_head_ptr = rx_buffer;

        while(rx_buffer_head_ptr < rx_buffer_head){
            to_udr(*rx_buffer_head_ptr++);
        }
    }
}

void serial_clear_rx_buffer(void){
    rx_buffer_head = rx_buffer;
}