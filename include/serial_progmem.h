#ifndef SERIAL_PROGMEM_GUARD
#define SERIAL_PROGMEM_GUARD
    /*! @file serial_progmem.h
        @brief Declarations needed to access PROGMEM data in flash
    */

    /** Indexes related to strings stored in progmem_string_table */                                                                                            
    #define TX_BUFFER_OVERFLOW      0 
    #define LOG_BUFFERING_ENABLED   1 
    #define LOG_BUFFERING_DISABLED  2 
    #define MCU1_ONLINE             3 
    #define MCU2_ONLINE             4 
    #define CMD_NOT_FOUND           5
    #define ICCM_SENDING_DATA       6
    #define ICCM_RX_BUFFER_DATA     7
    #define ICCM_RX_BUFFER_OVERFLOW 8
    #define ICCM_RX_BUFFER_CLEARED  9

    /* Global variables */
    extern char data_conversion_buffer[];
    extern const char* progmem_string_table[];

#endif /* SERIAL_PROGMEM_GUARD */
