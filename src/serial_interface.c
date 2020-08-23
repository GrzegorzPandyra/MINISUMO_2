/** @file serial_interface.c
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
#include "serial_interface.h"
#include "common_const.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Local macro definitions */
#define MAX_UART_DATA_LENGTH 300
#define MSG_SRC_LENGTH       8
#define DECIMAL              10
#define UINT32_MAX_DIGITS    10
#define OMIT_SLASH 1

/* Local macro-like functions */
/* Local static variables */
static char rx_buffer[MAX_UART_DATA_LENGTH] = {0};
static char tx_buffer[MAX_UART_DATA_LENGTH] = {0};
static char *tx_buffer_head = tx_buffer;
static Data_Target_T data_destination = T_UDR;

/* Global variables */
/* Local static functions */

/**
 * @brief Moves character 'c' into UDR (USART Data Register), which equals to sending via UART/USART
 * @param c character to be stored
 */
static void serial_to_udr(const unsigned char c){
    while ( !( UCSRA & (1<<UDRE)) )
    ;
    UDR = c;
}

/**
 * @brief Stores character 'c' in TX buffer
 * @param c character to be stored
 */
static void serial_to_tx_buffer(const char c){
    *tx_buffer_head = c;
    tx_buffer_head++;
}
/**
 * @brief Directs character 'c' into current data destination (UDR or TX_BUFFER)
 * @param c character to be send
 */
static void serial_process_char(const unsigned char c){
    switch(data_destination){
        case T_UDR:
            serial_to_udr(c);
            break;
        case T_TX_BUFFER:
            if(!serial_is_tx_buffer_full()){
                serial_to_tx_buffer(c);
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
static void serial_print_msg_src(const char *src){
    char filename[MSG_SRC_LENGTH] = {"        "};

    get_filename_from_path(filename, src);
    for(uint8_t i = 0; i < MSG_SRC_LENGTH; i++){
        serial_process_char(filename[i]);
    }
}

/**
 * @brief Prints message type
 * Prints whether given log is INFO, WARNING or an ERROR
 * @param msg_type Enum indicating the type
 */
static void serial_print_msg_type(Log_Type_T msg_type){
    const char msg_type_str[4][8] = {"INFO", "WARNING", "ERROR", "DATA"};
    uint8_t i = 0;

    while( msg_type_str[(uint8_t)msg_type][i] != NULL_CHAR){
        serial_process_char(msg_type_str[(uint8_t)msg_type][i]);
        i++;
    }
}

/**
 * @brief Print message data
 * Iterates over data until NULL-terminator is found or MAX_UART_DATA_LENGTH characters are printed.
 * @param data Data (string) to be send
 */
static void serial_print_msg_data(const char *data){
    uint8_t i = 0;

    while( (*(data+i) != NULL_CHAR) && (MAX_UART_DATA_LENGTH > i) ){
        serial_process_char(*(data+i));
        i++;
    }
}

/**
 * @brief Print line number
 * Converts @line_num to string and sends via serial. buff size takes into account NULL-terminator, but it is not send. 
 * @param line_num line number based on __LINE__ attribute
 */
static void serial_print_line_number(const uint32_t line_num){
    char buff[UINT32_MAX_DIGITS+1] = {NULL_CHAR};

    itoa(line_num, buff, DECIMAL);
    for(uint8_t i = 0; buff != NULL && i < UINT32_MAX_DIGITS; i++){
        serial_process_char(buff[i]);
    }
}

/**
 * @brief Handler for TX_BUFFER overflow. This function disables data buffering to show error. 
 * NEWLINE_CHAR compensates for missing newline character from unfinished log.
 */
static void serial_show_tx_buffer_overflow_error(void){
    serial_disable_buffering();
    serial_to_udr(NEWLINE_CHAR);
    serial_err("TX buffer overflow!");
    serial_enable_buffering();
}

/* Global functions */
/**
 * @brief Puts single character c into rx_buffer
 * Used by USART RX ISR to store data from RX channel. Rx buffer is read cyclically in main function.
 * @param c Char received from ISR
 */
void serial_receive_char(const char c){
    static char *rx_buffer_ptr = rx_buffer;

    if(c == NULL_CHAR){
        *rx_buffer_ptr = NULL_CHAR;   
        rx_buffer_ptr = rx_buffer;
        serial_read();
    }else{
        *rx_buffer_ptr = c;
        rx_buffer_ptr++;
    }
    
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
    serial_print_msg_src(metadata.filename);
    serial_process_char(COLON_CHAR);
    serial_print_line_number(metadata.line_num);
    serial_process_char(SPACE_CHAR);
    serial_print_msg_type(metadata.log_type);
    serial_process_char(SPACE_CHAR);
    serial_print_msg_data(str);
    if(metadata.log_type != DATA){
        serial_process_char(NEWLINE_CHAR);
    }
}


/**
 * @brief TODO
 * 
 */
void serial_read(void){
    uint8_t i = 0;

    while(rx_buffer[i] != NULL_CHAR){
        i++;
    }
    serial_log( (Log_Metadata_T){__FILE__, __LINE__, WARNING}, rx_buffer);
    //do something with data in rx_buffer
} 

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
 * @brief Send variable amount of raw data
 * Sends data under data.data pointer as a series if integers or characters. Function cuts into normal operation of serial_log()
 * and prints data between str and final newline character
 * @param data Structure containing data to be send
 */
void serial_log_data(const Log_Metadata_T metadata, const char *str, Data_T data)
{
    serial_log(metadata, str);
    serial_process_char(SPACE_CHAR);
    char buff[UINT32_MAX_DIGITS] = {0};
    for(uint8_t i = 0; i<data.data_length && i<MAX_UART_DATA_LENGTH; i++){
        switch(data.data_type){
            case INT:
                sprintf(buff, "%d", *( (int*)data.data + i) );
                serial_print_msg_data(buff);
                buff[0] = NULL_CHAR;
                break;
            case UINT8:
                sprintf(buff, "%d", *( (uint8_t*)data.data + i) );
                serial_print_msg_data(buff);
                buff[0] = NULL_CHAR;
                break;
            case STRING:
                serial_process_char( *( (char*)data.data + i) );
                break;
            default:
                /*Do nothing*/
                break;
        }
    }
    serial_process_char(NEWLINE_CHAR);
}

/**
 * @brief Enable data buffering
 */
void serial_enable_buffering(void){
    data_destination = T_TX_BUFFER;
}

/**
 * @brief Disable data buffering
 */
void serial_disable_buffering(void){
    data_destination = T_UDR;
}

/**
 * @brief Moves content of TX_BUFFER into UDR until current position of tx_buffer_head is not found.
 * Prints additional log if buffer overflow occured, then moves tx_buffer_head to the begining of TX_BUFFER
 * which is considered as clearing the buffer
 */
void serial_read_tx_buffer(void){
    if(tx_buffer_head != tx_buffer){
        char *tx_buffer_read_ptr = tx_buffer;

        while(tx_buffer_read_ptr <= tx_buffer_head){
            serial_to_udr(*tx_buffer_read_ptr++);
        }

        if(serial_is_tx_buffer_full()){
            serial_show_tx_buffer_overflow_error();
        }

        tx_buffer_head = tx_buffer;
    }
}

/**
 * @brief Returns if tx buffer is full
 */
bool serial_is_tx_buffer_full(void){
    return (tx_buffer_head >= tx_buffer+MAX_UART_DATA_LENGTH);
}