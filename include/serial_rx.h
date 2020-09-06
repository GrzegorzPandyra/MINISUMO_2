#ifndef SERIAL_RX_GUARD
#define SERIAL_RX_GUARD

/*! @file serial_rx.h
    @brief API for serial communication
    Interface file for other components to access serial communication between MCU and PC. 
*/
#include <stdint.h>
#include <stdbool.h>

void serial_on_receive(const char c);
bool serial_is_rx_buffer_full(void);
void serial_clear_rx_buffer(void);
void serial_read_rx_buffer(void);

#endif /* SERIAL_RX_GUARD */
