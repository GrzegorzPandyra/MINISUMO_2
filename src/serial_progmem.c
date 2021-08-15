/** @file serial_progmem.c
*   @brief Definitions of objects used by PROGMEM 
    PROGMEM macro marks objects that are stored in MCU's flash memory (instead of RAM, which is very limited).
*/
#include <avr/pgmspace.h>

#define DATA_CONVERSION_BUFFER_SIZE 30

/* Literals kept in codeflash for logging               0|                             |29*/
static const char TX_BUFFER_OVERFLOW_P[]       PROGMEM = "TX buffer overflow!";
static const char LOG_BUFFERING_ENABLED_P[]    PROGMEM = "Log buffering enabled";
static const char LOG_BUFFERING_DISABLED_P[]   PROGMEM = "Log buffering disabled";
static const char MCU1_ONLINE_P[]              PROGMEM = "MCU1 online";
static const char MCU2_ONLINE_P[]              PROGMEM = "MCU2 online";
static const char CMD_NOT_FOUND_P[]            PROGMEM = "Cmd not found";
static const char ICCM_SENDING_DATA_P[]        PROGMEM = "ICCM Sending data:";
static const char ICCM_RX_BUFFER_DATA_P[]      PROGMEM = "ICCM RX buffer data:";
static const char ICCM_RX_BUFFER_OVERFLOW_P[]  PROGMEM = "ICCM RX BUFFER OVERFLOW";
static const char ICCM_RX_BUFFER_CLEARED_P[]   PROGMEM = "ICCM RX BUFFER cleared";
static const char AI_STATUS_IDLE_P[]           PROGMEM = "AI status: IDLE";
static const char AI_STATUS_ARMED_P[]          PROGMEM = "AI status: ARMED";
static const char AI_STATUS_SEARCH_P[]         PROGMEM = "AI status: SEARCH";
static const char AI_STATUS_ATTACK_P[]         PROGMEM = "AI status: ATTACK";
static const char AI_STATUS_R2R_P[]            PROGMEM = "AI status: R2R";
static const char AI_FORCED_STOP_P[]           PROGMEM = "AI forced stop";
static const char AI_INIT_IN_5_P[]             PROGMEM = "AI init in 5..";
static const char AI_INIT_IN_4_P[]             PROGMEM = "AI init in 4..";
static const char AI_INIT_IN_3_P[]             PROGMEM = "AI init in 3..";
static const char AI_INIT_IN_2_P[]             PROGMEM = "AI init in 2..";
static const char AI_INIT_IN_1_P[]             PROGMEM = "AI init in 1..";
static const char DRV_CTRL_INIT_P[]            PROGMEM = "DrvCtrl init";
static const char ICCM_INIT_P[]                PROGMEM = "ICCM init";
static const char SERIAL_INIT_P[]              PROGMEM = "Serial init";
static const char ADC_INIT_P[]                 PROGMEM = "ADC init";



char data_conversion_buffer[DATA_CONVERSION_BUFFER_SIZE] = {0};
const char* const  progmem_string_table[] PROGMEM = {
    TX_BUFFER_OVERFLOW_P, 
    LOG_BUFFERING_ENABLED_P,
    LOG_BUFFERING_DISABLED_P,
    MCU1_ONLINE_P,
    MCU2_ONLINE_P,
    CMD_NOT_FOUND_P,
    ICCM_SENDING_DATA_P,
    ICCM_RX_BUFFER_DATA_P,
    ICCM_RX_BUFFER_OVERFLOW_P,
    ICCM_RX_BUFFER_CLEARED_P,
    AI_STATUS_IDLE_P,
    AI_STATUS_ARMED_P,
    AI_STATUS_SEARCH_P,
    AI_STATUS_ATTACK_P,
    AI_STATUS_R2R_P,
    AI_FORCED_STOP_P,
    AI_INIT_IN_5_P,
    AI_INIT_IN_4_P,
    AI_INIT_IN_3_P,
    AI_INIT_IN_2_P,
    AI_INIT_IN_1_P,
    DRV_CTRL_INIT_P,
    ICCM_INIT_P,
    SERIAL_INIT_P,
    ADC_INIT_P
};