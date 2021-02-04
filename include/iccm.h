#ifndef ICCM_GUARD
#define ICCM_GUARD
/*! @file iccm.c
    @brief Inter-Chip Communication Manager
*/
#include <stdint.h>
#include <stdbool.h>

#define ICCM_START_BIT 1
#define ICCM_DATA_SIZE 8
#define ICCM_STOP_BIT  1
#define ICCM_FRAME_SIZE (ICCM_START_BIT+ICCM_DATA_SIZE+ICCM_STOP_BIT)

/**
 * @brief Status describing ICCM state - if data is being transmitted, received or nothing is happening.
 */
typedef enum ICCM_Status_Tag{
    IDLE = 0,
    RX_IN_PROGRESS = 1,
    TX_IN_PROGRESS = 2,
    DISABLED = 3
} ICCM_Status_T;

/**
 * @brief Structure of data frame
 */
typedef union ICCM_DataFrame_Tag{
    uint16_t raw_bits;
    struct{
        uint8_t start_bit : ICCM_START_BIT;
        uint8_t data      : ICCM_DATA_SIZE;
        uint8_t stop_bit  : ICCM_STOP_BIT;
    }struct_bits;
}ICCM_DataFrame_T;

/* Global functions */
void ICCM_init(void);
void ICCM_send(char *str);
void ICCM_read_rx_buffer(char *buff_out, uint8_t *data_length);
void ICCM_on_rx_trigger(void);
bool ICCM_is_data_available(void);
void ICCM_clear_rx_buffer(void);
void ICCM_disable(void);
void ICCM_enable(void);
#endif /* ICCM_GUARD */




