#ifndef SERIAL_TX_GUARD
#define SERIAL_TX_GUARD

/*! @file serial_tx.h
    @brief API for serial communication
    Interface file for other components to access serial communication between MCU and PC. 
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <serial_progmem.h>

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
 * @brief Structure describing the origin and type of log
 */
typedef struct Log_Metadata_Tag{
    const char *filename;
    const uint32_t line_num;
    const Log_Type_T log_type;
} Log_Metadata_T;

/** Logging API */
#define copy_to_ram(id)     (strcpy_P(data_conversion_buffer, (char*)pgm_read_word(&progmem_string_table[(uint8_t)id])))
#define get_metadata(type)  (Log_Metadata_T){__FILE__, __LINE__, type}

#define log_info(str)   (serial_log( get_metadata(INFO), str ))
#define log_info_P(id)  copy_to_ram(id); \
                        serial_log(get_metadata(INFO), data_conversion_buffer)
                          
#define log_warn(str)   (serial_log( get_metadata(WARNING), str ))
#define log_warn_P(id)  copy_to_ram(id); \
                        serial_log(get_metadata(WARNING), data_conversion_buffer)
#define log_err(str)    (serial_log( get_metadata(ERROR), str ))
#define log_err_P(id)   copy_to_ram(id); \
                        serial_log(get_metadata(ERROR), data_conversion_buffer)

#define log_raw_string(str) serial_log_raw_string(str)

#define log_data_1(format, arg1)                                            sprintf(data_conversion_buffer, format, arg1);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_2(format, arg1, arg2)                                      sprintf(data_conversion_buffer, format, arg1, arg2);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_3(format, arg1,arg2, arg3)                                 sprintf(data_conversion_buffer, format, arg1, arg2, arg3);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_4(format, arg1, arg2, arg3, arg4)                          sprintf(data_conversion_buffer, format, arg1, arg2, arg3, arg4);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_5(format, arg1, arg2, arg3, arg4, arg5)                    sprintf(data_conversion_buffer, format, arg1, arg2, arg3, arg4, arg5);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_6(format, arg1, arg2, arg3, arg4, arg5, arg6)              sprintf(data_conversion_buffer, format, arg1, arg2, arg3, arg4, arg5, arg6);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_7(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7)        sprintf(data_conversion_buffer, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)
#define log_data_8(format, arg1, arg2, arg3, arg4, arg5,arg6, arg7, arg8)   sprintf(data_conversion_buffer, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);\
                                                                            serial_log(get_metadata(INFO), data_conversion_buffer)                                                                           
                                   
void serial_init(uint32_t f_cpu, uint32_t baudrate);
void serial_log(const Log_Metadata_T metadata, const char *str);
bool serial_is_tx_buffer_full(void);
void serial_disable_buffering(void);
void serial_enable_buffering(void);
void serial_read_tx_buffer(void);
void serial_clear_tx_buffer(void);
void serial_log_raw_string(const char *str);
#endif /* SERIAL_TX_GUARD */
