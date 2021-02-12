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
    #define AI_STATUS_IDLE          10
    #define AI_STATUS_ARMED         11
    #define AI_STATUS_SEARCH        12
    #define AI_STATUS_ATTACK        13
    #define AI_STATUS_R2R           14
    #define AI_FORCED_STOP_P        15
    #define AI_INIT_IN_5            16
    #define AI_INIT_IN_4            17
    #define AI_INIT_IN_3            18
    #define AI_INIT_IN_2            19
    #define AI_INIT_IN_1            20

    /* Global variables */
    extern char data_conversion_buffer[];
    extern const char* progmem_string_table[];

#endif /* SERIAL_PROGMEM_GUARD */
