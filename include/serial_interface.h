#ifndef SERIAL_INTERFACE_GUARD
#define SERIAL_INTERFACE_GUARD

/*! @file serial_interface.h
    @brief API for serial communication
    Interface file for other components to access serial communication between MCU and PC. 
*/

#include <stdint.h>

/**
 * @brief Enum for message type
 * Describes the type of log being send.
 */
typedef enum Message_Type_Tag{
    NOTIFY = 0,
    WARNING = 1,
    ERROR = 2
} Message_Type_T;

/* Global variables */
/* Global functions */
void serial_init(uint32_t f_cpu, uint32_t baudrate);
void serial_log(const char *src, Message_Type_T msg_type, const char *str);
void serial_read();
void serial_receive_char(const char c);
void serial_send_data(uint8_t size, void *data);

#endif /* SERIAL_INTERFACE_GUARD */
