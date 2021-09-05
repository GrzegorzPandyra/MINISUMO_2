/** @file serial_progmem.c
*   @brief Definitions of objects used by PROGMEM 
    PROGMEM macro marks objects that are stored in MCU's flash memory (instead of RAM, which is very limited).
*/
#include <avr/pgmspace.h>

#define DATA_CONVERSION_BUFFER_SIZE 30

/* Literals kept in codeflash for logging                       0|                             |29*/
static const char PROGMEM_TX_BUFFER_OVERFLOW_P[]       PROGMEM = "TX buffer overflow!";
static const char PROGMEM_LOG_BUFFERING_ENABLED_P[]    PROGMEM = "Log buffering enabled";
static const char PROGMEM_LOG_BUFFERING_DISABLED_P[]   PROGMEM = "Log buffering disabled";
static const char PROGMEM_MCU1_ONLINE_P[]              PROGMEM = "MCU1 online";
static const char PROGMEM_MCU2_ONLINE_P[]              PROGMEM = "MCU2 online";
static const char PROGMEM_CMD_NOT_FOUND_P[]            PROGMEM = "Cmd not found";
static const char PROGMEM_ICCM_SENDING_DATA_P[]        PROGMEM = "ICCM Sending data:";
static const char PROGMEM_ICCM_RX_BUFFER_DATA_P[]      PROGMEM = "ICCM RX buffer data:";
static const char PROGMEM_ICCM_RX_BUFFER_OVERFLOW_P[]  PROGMEM = "ICCM RX BUFFER OVERFLOW";
static const char PROGMEM_ICCM_RX_BUFFER_CLEARED_P[]   PROGMEM = "ICCM RX BUFFER cleared";
static const char PROGMEM_AI_STATUS_IDLE_P[]           PROGMEM = "AI status: IDLE";
static const char PROGMEM_AI_STATUS_ARMED_P[]          PROGMEM = "AI status: ARMED";
static const char PROGMEM_AI_STATUS_SEARCH_P[]         PROGMEM = "AI status: SEARCH";
static const char PROGMEM_AI_STATUS_TRACKING_P[]       PROGMEM = "AI status: TRACKING";
static const char PROGMEM_AI_STATUS_ATTACK_P[]         PROGMEM = "AI status: ATTACK";
static const char PROGMEM_AI_STATUS_RETURN_P[]         PROGMEM = "AI status: RETURN";
static const char PROGMEM_AI_FORCED_STOP_P[]           PROGMEM = "AI forced stop";
static const char PROGMEM_AI_INIT_IN_P[]               PROGMEM = "AI init in:";


/* Buffer used for data exchange between RAM and FLASH */
char data_conversion_buffer[DATA_CONVERSION_BUFFER_SIZE] = {};

/* Table for accessing PROGMEM strings*/
const char* const progmem_string_table[] PROGMEM = {
    PROGMEM_TX_BUFFER_OVERFLOW_P, 
    PROGMEM_LOG_BUFFERING_ENABLED_P,
    PROGMEM_LOG_BUFFERING_DISABLED_P,
    PROGMEM_MCU1_ONLINE_P,
    PROGMEM_MCU2_ONLINE_P,
    PROGMEM_CMD_NOT_FOUND_P,
    PROGMEM_ICCM_SENDING_DATA_P,
    PROGMEM_ICCM_RX_BUFFER_DATA_P,
    PROGMEM_ICCM_RX_BUFFER_OVERFLOW_P,
    PROGMEM_ICCM_RX_BUFFER_CLEARED_P,
    PROGMEM_AI_STATUS_IDLE_P,
    PROGMEM_AI_STATUS_ARMED_P,
    PROGMEM_AI_STATUS_SEARCH_P,
    PROGMEM_AI_STATUS_TRACKING_P,
    PROGMEM_AI_STATUS_ATTACK_P,
    PROGMEM_AI_STATUS_RETURN_P,
    PROGMEM_AI_FORCED_STOP_P,
    PROGMEM_AI_INIT_IN_P
};