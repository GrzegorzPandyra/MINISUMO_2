/** @file serial_tx.c
*   @brief Implementation of UART transmission 
*/
#include "serial_tx.h"
#include "common_const.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Disable debug logs if AI_DEBUG is not defined during build */
#ifndef SERIA_TX_DEBUG
    #undef log_info_P
    #define log_info_P(str)
    #undef log_info
    #define log_info(str)
    #undef log_warn
    #define log_warn(str)
    #undef log_warn_P
    #define log_warn_P(str)
    #undef log_err
    #define log_err(str)
    #undef log_err_P
    #define log_err_P(str)
    #undef log_raw_string
    #define log_raw_string(str)
#endif

/* Local macro definitions */
#define MAX_UART_DATA_LENGTH 100
#define TX_BUFFER_SIZE       300
#define MSG_SRC_LENGTH       8
#define MSG_TYPE_LENGTH      8
#define MSG_TYPES_NUM        4
#define DECIMAL              10
#define UINT32_MAX_DIGITS    10
#define OMIT_SLASH 1

/**
 * @brief Marker used to determine if logs should be passed directly to UDR 
 * or stored in TX buffer until explicitly moved to UDR
 */
typedef enum Data_Target_Tag{
    T_UDR = 0,
    T_TX_BUFFER = 1
} Data_Target_T;


/* Local static variables */
static const char msg_type_str[MSG_TYPES_NUM][MSG_TYPE_LENGTH] = {"INFO", "WARNING", "ERROR", "DATA"};
static char tx_buffer[TX_BUFFER_SIZE] = {0};
static char *tx_buffer_head = tx_buffer;
static Data_Target_T data_destination = T_UDR;

/* Local static functions */
static void to_udr(const unsigned char c);
static void to_tx_buffer(const char c);
static void process_char(const unsigned char c);
static void get_filename_from_path(char *filename, const char *path);
static void print_msg_src(const char *src);
static void print_msg_type(Log_Type_T msg_type);
static void print_msg_data(const char *data);
static void print_line_number(const uint32_t line_num);
static void show_tx_buffer_overflow_error(void);
/**
 * @brief Moves character 'c' into UDR (USART Data Register), which equals to sending via UART/USART
 * @param c character to be stored
 */
static void to_udr(const unsigned char c){
    while ( !( UCSRA & (1<<UDRE)) )
    ;
    UDR = c;
}

/**
 * @brief Stores character 'c' in TX buffer
 * @param c character to be stored
 */
static void to_tx_buffer(const char c){
    *tx_buffer_head = c;
    tx_buffer_head++;
}
/**
 * @brief Directs character 'c' into current data destination (UDR or TX_BUFFER)
 * @param c character to be send
 */
static void process_char(const unsigned char c){
    switch(data_destination){
        case T_UDR:
            to_udr(c);
            break;
        case T_TX_BUFFER:
            if(!serial_is_tx_buffer_full()){
                to_tx_buffer(c);
            } 
            break;
        default:
            /*None*/
            break;
    }
} 

/**
 * @brief Parses path, looking for last '/' character, then treats the rest as filename. 
 * Function tracks filename length and keeps it at max value MSG_SRC_LENGTH
 * @param filename output buffer for filename
 * @param path     path to be parsed (from __FILE__ attribute)
 */
static void get_filename_from_path(char *filename, const char *path){
    const char *last_slash_ptr = path;
    uint8_t filename_length = 0;
    while(*path != NULL_CHAR){
        if(*path == SLASH_CHAR){
            last_slash_ptr = path;
            filename_length = 0;
        } else {
            filename_length++;
        }
        path++;
    }
    if(filename_length > MSG_SRC_LENGTH){
        filename_length = MSG_SRC_LENGTH;
    }
    memcpy(filename, last_slash_ptr+OMIT_SLASH, filename_length);
}

/**
 * @brief Prints log source
 * Prints the name of the file where log has been created. Src string is based on __FILE__ attribute.
 * @param src __FILE__ attribute of file where message originated from
 */
static void print_msg_src(const char *src){
    char filename[MSG_SRC_LENGTH] = {"        "};
    get_filename_from_path(filename, src);
    for(uint8_t i = 0; i < MSG_SRC_LENGTH; i++){
        process_char(filename[i]);
    }
}

/**
 * @brief Prints message type
 * Prints whether given log is INFO, WARNING or an ERROR
 * @param msg_type Enum indicating the type
 */
static void print_msg_type(Log_Type_T msg_type){
    uint8_t i = 0;
    while( msg_type_str[(uint8_t)msg_type][i] != NULL_CHAR){
        process_char(msg_type_str[(uint8_t)msg_type][i]);
        i++;
    }
}

/**
 * @brief Print message data
 * Iterates over data until NULL-terminator is found or MAX_UART_DATA_LENGTH characters are printed.
 * @param data Data (string) to be send
 */
static void print_msg_data(const char *data){
    uint16_t i = 0;
    while( (*(data+i) != NULL_CHAR) && (MAX_UART_DATA_LENGTH > i) ){
        process_char(*(data+i));
        i++;
    }
}

/**
 * @brief Print line number
 * Converts @line_num to string and sends via serial. buff size takes into account NULL-terminator, but it is not send. 
 * @param line_num line number based on __LINE__ attribute
 */
static void print_line_number(const uint32_t line_num){
    char buff[UINT32_MAX_DIGITS+1] = {NULL_CHAR};
    itoa(line_num, buff, DECIMAL);
    for(uint8_t i = 0; buff != NULL && i < UINT32_MAX_DIGITS; i++){
        process_char(buff[i]);
    }
}

/**
 * @brief Handler for TX_BUFFER overflow. This function disables data buffering to show error. 
 * NEWLINE_CHAR compensates for missing newline character from unfinished log.
 */
static void show_tx_buffer_overflow_error(void){
    bool is_buffering_enabled = (data_destination == T_TX_BUFFER)? true: false;
    if(is_buffering_enabled)
        serial_disable_buffering();
    
    to_udr(NEWLINE_CHAR);
    log_err_P(PROGMEM_TX_BUFFER_OVERFLOW);
    
    if(is_buffering_enabled)
        serial_enable_buffering();
}

/* Global functions */

/**
 * @brief UART initialization
 * Function writes to appropriate registers to enable communication over UART. Frame format is 8data, 2stop bit.
 * @param f_cpu     MCU clock
 * @param baudrate  Target baudrate
 */
void serial_init(uint32_t f_cpu, uint32_t baudrate){
    /* Set baud rate */
    uint32_t ubrr = f_cpu/(16*baudrate)-1;
    UBRRH = (unsigned char)(ubrr>>8);
    UBRRL = (unsigned char)ubrr;

    /* Enable receiver and transmitter */
    UCSRB = (1<<RXEN)|(1<<TXEN);

    /* Set frame format: 8data, 2stop bit */
    UCSRC = (1<<URSEL)|(1<<USBS)|(1<<UCSZ0)|(1<<UCSZ1);

    /* Enable Rx interrupt */
    UCSRB |= (1<<RXCIE);
    
    /* Clear INT0 flag */
    GIFR |= 1<<INTF0;
}

/**
 * @brief Send string str via serial
 * Prints logs formatted as below:
 * main.c  :  44      NOTIFY    Hello from ATmega8
 * <source> <line>  <log type>  <Log data (string)>
 * Function calls subfunctions to print parts of the log + adds formatting characters
 * @param str       String to be send. Must be null-terminated
 * @param msg_type  Label describing what kind of log str is. Can be NOTIFY, WARNING, ERROR or DATA
 * @param file      String describing source of message, typically a file
 * @param line      Line in source file identifing the log
 */
void serial_log(const Log_Metadata_T metadata, const char *str){
    print_msg_src(metadata.filename);
    process_char(COLON_CHAR);
    print_line_number(metadata.line_num);
    process_char(SPACE_CHAR);
    print_msg_type(metadata.log_type);
    process_char(SPACE_CHAR);
    print_msg_data(str);
    if(metadata.log_type != DATA){
        process_char(NEWLINE_CHAR);
    }
}

/**
 * @brief Enable data buffering
 */
void serial_enable_buffering(void){
    log_info_P(PROGMEM_LOG_BUFFERING_ENABLED);
    data_destination = T_TX_BUFFER;
}

/**
 * @brief Disable data buffering
 */
    
void serial_disable_buffering(void){
    data_destination = T_UDR;
    log_info_P(PROGMEM_LOG_BUFFERING_DISABLED);
}

/**
 * @brief Moves content of TX_BUFFER into UDR until current position of tx_buffer_head is not found.
 * Prints additional log if buffer overflow occured, then moves tx_buffer_head to the begining of TX_BUFFER
 * which is considered as clearing the buffer
 */
void serial_read_tx_buffer(void){
    if(tx_buffer_head != tx_buffer){
        char *tx_buffer_read_ptr = tx_buffer;

        while(tx_buffer_read_ptr < tx_buffer_head){
            to_udr(*tx_buffer_read_ptr++);
        }

        if(serial_is_tx_buffer_full()){
            show_tx_buffer_overflow_error();
        }

        serial_clear_tx_buffer();
    }
}

void serial_clear_tx_buffer(void){
    tx_buffer_head = tx_buffer;
    *tx_buffer_head = NULL_CHAR;
}

/**
 * @brief Returns if tx buffer is full
 */
bool serial_is_tx_buffer_full(void){
    return (tx_buffer_head >= tx_buffer+TX_BUFFER_SIZE);
}

/**
 * @brief Print only the string provided in @str (no metadata)
 */
void serial_log_raw_string(const char *str){
    print_msg_data(str);
}
