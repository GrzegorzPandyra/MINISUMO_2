/** @file serial_rx.c
*   @brief Implements receiving part of serial communication
*   Source file implementing communication between MCU and PC.
*/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common_const.h"
#include "serial_rx.h"
#include "serial_tx.h"
#include "ICCM.h"
#include "config.h"
#include "drive_ctrl.h"


#define CMD_NAME_LENGTH 7
#define RX_BUFFER_SIZE 12 /* 7 chars for cmd, 1 space, 3 chars for arg, NULL */

/**
 * @brief Structure binding cmd to a callback, used during rx_buffer parsing
 */
typedef struct Cmd_Record_Tag{
    char string[RX_BUFFER_SIZE];
    void (*direct_callback)();
    void (*custom_callback)(const void *data, size_t data_len);
} Cmd_Record_T;

/* Local static variables */
static char rx_buffer[RX_BUFFER_SIZE] = {0};
static char *rx_buffer_head = rx_buffer;
const Cmd_Record_T cmd_list[] = {
    COMMON_SERIAL_CMD_LIST,
    MCU_SPECIFIC_SERIAL_CMD_LIST
};


/* Local static functions */
static bool to_rx_buffer(const char c);
static void to_udr(const unsigned char c);
static const Cmd_Record_T* find_cmd(const char *cmd);

/**
 * @brief Attempts to store char c in rx_buffer 
 * @param c Char to be stored
 */
static bool to_rx_buffer(const char c){
    if(!serial_is_rx_buffer_full()){
        *rx_buffer_head = c;
        rx_buffer_head++;
        *rx_buffer_head = NULL_CHAR;
        return true;
    }
    return false;
}

/**
 * @brief Searches for cmd in cmd_list 
 * Parses first work from @cmd string and tries to match it (further 'words' are considered arguments for the @cmd)
 * @param cmd string to be matched in cmd_list
 * @return record if cmd has been found, NULL otherwise
 */
static const Cmd_Record_T* find_cmd(const char *cmd){
    const char *first_space = strchr(cmd, SPACE_CHAR);
    char cmd_name[CMD_NAME_LENGTH] = {};

    if(first_space != NULL){
        memcpy((void *)cmd_name, (const void*)cmd, (size_t)(first_space-cmd));
        cmd_name[first_space-cmd] = NULL_CHAR;
    } else {
        strcpy(cmd_name, cmd);
    }

    for(uint8_t i = 0; i < arr_length(cmd_list); i++){
        if(0 == strcmp(cmd_list[i].string, cmd_name)){
            return &(cmd_list[i]);
        }
    }
    
    log_err_P(CMD_NOT_FOUND);
    return NULL;
}

/**
 * @brief Sends char c to UDR
 * @param c Char to be send
 */
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
 * @brief Controls data received in ISR - stores them in rx_buffer or executes matched callbacks
 * @param c Char received from ISR
 */
void serial_on_receive(const char c){
    if(c == LF || c == CR) {
        const Cmd_Record_T *cmd_record = find_cmd(rx_buffer);
        if(cmd_record != NULL){
            if(cmd_record->direct_callback != NULL){
                cmd_record->direct_callback();
            } else if(cmd_record->custom_callback != NULL){
                cmd_record->custom_callback(rx_buffer, (size_t)(rx_buffer_head - rx_buffer));
            }
        }
        serial_clear_rx_buffer();
    } else {
        to_rx_buffer(c);
    }
}

/**
 * @brief Transfer contents of rx_buffer to UDR (this is debug command)
 */
void serial_read_rx_buffer(void){
    if(rx_buffer_head != rx_buffer){
        char *rx_buffer_head_ptr = rx_buffer;
        while(rx_buffer_head_ptr < rx_buffer_head){
            to_udr(*rx_buffer_head_ptr++);
        }
    }
}

/**
 * @brief Return rx_buffer_head to the beginning of rx_buffer
 */
void serial_clear_rx_buffer(void){
    rx_buffer_head = rx_buffer;
    *rx_buffer_head = NULL_CHAR;
}