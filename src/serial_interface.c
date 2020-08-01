/** @file serial_interface.c
*   @brief Implementation of serial communication 
*   Source file implementing communication between MCU and PC.
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

/* Local macro definitions */
#define MAX_UART_DATA_LENGTH 256
#define MSG_SRC_LENGTH       10 
#define DECIMAL              10
#define UINT32_MAX_DIGITS    10

/* Local macro-like functions */
/* Local static variables */
static char rx_buffer[MAX_UART_DATA_LENGTH] = {0};

/* Global variables */
/* Local static functions */
/**
 * @brief Sends a single char over uart
 * Writes a single character to UDR, effectively sending it over uart. 
 * @param c Character to be send
 */
void static serial_send_char(const unsigned char c){
    while ( !( UCSRA & (1<<UDRE)) )
    ;
    UDR = c;
} 

/**
 * @brief Prints log source
 * Prints the name of the file where log has been created. Src string is based on __FILE__ attribute.
 * @param src __FILE__ attribute of file where message originated from
 */
static void serial_print_msg_src(const char *src){
    uint8_t i = 0;

    while(*(src+i) != NULL_CHAR && i < MSG_SRC_LENGTH){
        serial_send_char(*(src+i));
        i++;
    }
    while(i < MSG_SRC_LENGTH){
        serial_send_char(SPACE_CHAR);
        i++;
    }
}

/**
 * @brief Prints message type
 * Prints whether given log is INFO, WARNING or an ERROR
 * @param msg_type Enum indicating the type
 */
static void serial_print_msg_type(Log_Type_T msg_type){
    const char msg_type_str[3][8] = {"INFO", "WARNING", "ERROR"};
    uint8_t i = 0;

    while( msg_type_str[(uint8_t)msg_type][i] != NULL_CHAR){
        serial_send_char(msg_type_str[(uint8_t)msg_type][i]);
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
        serial_send_char(*(data+i));
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
    uint8_t i;

    itoa(line_num, buff, DECIMAL);
    for(i = 0; buff != NULL && i < UINT32_MAX_DIGITS; i++){
        serial_send_char(buff[i]);
    }
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
 * @param msg_type  Label describing what kind of log str is. Can be NOTIFY, WARNING or ERROR
 * @param file      String describing source of message, typically a file
 * @param line      Line in source file identifing the log
 */
void serial_log(const Log_Metadata_T metadata, const char *str){
    serial_print_msg_src(metadata.filename);
    serial_send_char(COLON_CHAR);
    serial_print_line_number(metadata.line_num);
    serial_send_char(SPACE_CHAR);
    serial_print_msg_type(metadata.log_type);
    serial_send_char(SPACE_CHAR);
    serial_print_msg_data(str);
    serial_send_char(NEWLINE_CHAR);
}


/**
 * @brief TODO
 * 
 */
void serial_read(){
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
}

/**
 * @brief Send variable amount of raw data
 * Sends data under data.data pointer as a series if integers or characters
 * @param data Structure containing data to be send
 */
void serial_log_data(Data_T data)
{
    uint8_t i = 0;
    char buff[UINT32_MAX_DIGITS] = {0};

    for(i = 0; i<data.data_length && i<MAX_UART_DATA_LENGTH; i++){
        switch(data.data_type){
            case INT:
                sprintf(buff, "%d", *( (int*)data.data + i) );
                serial_print_msg_data(buff);
                serial_send_char(NEWLINE_CHAR);
                buff[0] = NULL_CHAR;
                break;
            case STRING:
                serial_send_char( *( (char*)data.data + i) );
                break;
            default:
                /*Do nothing*/
                break;
        }
    }
}
