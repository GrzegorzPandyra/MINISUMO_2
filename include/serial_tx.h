#ifndef SERIAL_TX_GUARD
#define SERIAL_TX_GUARD

/*! @file serial_tx.h
    @brief API for serial communication
    Interface file for other components to access serial communication between MCU and PC. 
*/
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Describes the type of log being send.
 */
typedef enum Log_Type_Tag{
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    DATA = 3
} Log_Type_T;

/**
 * @brief Marker for data type
 */
typedef enum Message_Data_Type_Tag{
    STRING = 0,
    INT = 1,
    HEX = 2,
    UINT8 = 3
} Message_Data_Type_T;

/**
 * @brief Structure describing the origin and type of log
 */
typedef struct Log_Metadata_Tag{
    const char *filename;
    const uint32_t line_num;
    const Log_Type_T log_type;
} Log_Metadata_T;

/**
 * @brief Wrapper for data, Contains pointer to data, as well as data length and data type
 */
typedef struct Data_Tag{
    void *data;
    uint8_t data_length;
    Message_Data_Type_T data_type;
} Data_T;

/**
 * @brief Marker used to determine if logs should be passed directly to UDR 
 * or stored in TX buffer until explicitly moved to UDR
 */
typedef enum Data_Target_Tag{
    T_UDR = 0,
    T_TX_BUFFER = 1
} Data_Target_T;

/* Local macro-like functions */
#define METADATA(type) (Log_Metadata_T){__FILE__, __LINE__, type}
#define serial_info(str) (serial_log( METADATA(INFO), str ))
#define serial_warn(str) (serial_log( METADATA(WARNING), str ))
#define serial_err(str)  (serial_log( METADATA(ERROR), str ))
#define serial_data_int(str, data, data_length) (serial_log_data( METADATA(DATA), str, (Data_T){data, data_length, INT} ))
#define serial_data_uint8(str, data, data_length) (serial_log_data( METADATA(DATA), str, (Data_T){data, data_length, UINT8} ))
#define serial_data_str(str, data, data_length) (serial_log_data( METADATA(DATA), str, (Data_T){data, data_length, STRING} ))

/* Global variables */
/* Global functions */
void serial_init(uint32_t f_cpu, uint32_t baudrate);
void serial_log(const Log_Metadata_T metadata, const char *str);
void serial_log_data(const Log_Metadata_T metadata, const char *str, Data_T data);
bool serial_is_tx_buffer_full(void);
void serial_disable_buffering(void);
void serial_enable_buffering(void);
void serial_read_tx_buffer(void);
void serial_clear_tx_buffer(void);
#endif /* SERIAL_TX_GUARD */
