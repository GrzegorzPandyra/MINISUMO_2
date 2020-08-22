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
#define ICCM_FRAME_SIZE (ICCM_START_BIT+ICCM_DATA_SIZE+ICCM_STOP_BIT)
#define ICCM_MAX_DATA_LENGTH 20
/* Local macro-like functions */
/* Local static variables */

typedef union ICCM_DataFrame_Tag{
    uint16_t raw_bits;
    struct{
        uint8_t start_bit : ICCM_START_BIT;
        uint8_t data      : ICCM_DATA_SIZE;
        uint8_t stop_bit  : ICCM_STOP_BIT;
    }struct_bits;
}ICCM_DataFrame_T;

/* Global variables */
/* Local static functions */
/* Global functions */
void iccm_init(void);
void iccm_send(char *str);
void iccm_receive(void);
uint8_t iccm_receive_char(char c);
void iccm_set_rx_complete_flag(uint8_t value);

#endif /* ICCM_GUARD */




