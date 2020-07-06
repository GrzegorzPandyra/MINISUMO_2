#ifndef ICCM_GUARD
#define ICCM_GUARD
/*! @file iccm.c
    @brief Inter-Chip Communication Manager
*/
#include <stdint.h>
/* Local macro definitions */
#define ICCM_START_BIT 1
#define ICCM_DATA_SIZE 8
#define ICCM_STOP_BIT  1
#define ICCM_MAX_DATA_LENGTH 20
/* Local macro-like functions */
/* Local static variables */

typedef struct ICCM_Register_Tag
{
    uint8_t rx_pin;
    uint8_t tx_pin;
    uint8_t baudrate;
} ICCM_Register_T;

typedef union ICCM_DataFrame_Tag
{
    uint16_t raw_frame;
    struct 
    {
        uint8_t start_bit : ICCM_START_BIT;
        uint8_t data      : ICCM_DATA_SIZE;
        uint8_t stop_bits : ICCM_STOP_BIT;
    } 
    struct_frame;
} ICCM_DataFrame_T;

/* Global variables */
/* Local static functions */
/* Global functions */
void iccm_init(uint8_t rx_pin, uint8_t tx_pin, uint8_t baudrate);
void iccm_send(char *str);
void iccm_receive();
uint8_t iccm_receive_char(char c);
void iccm_set_rx_complete_flag(uint8_t value);

#endif /* ICCM_GUARD */




