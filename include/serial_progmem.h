#ifndef SERIAL_PROGMEM_GUARD
#define SERIAL_PROGMEM_GUARD
    /*! @file serial_progmem.h
        @brief Declarations needed to access PROGMEM data in flash
    */

    /** Indexes related to strings stored in progmem_string_table */                                                                                            
    typedef enum Progmem_Table_Index_Tag{
        PROGMEM_TX_BUFFER_OVERFLOW = 0,
        PROGMEM_LOG_BUFFERING_ENABLED,
        PROGMEM_LOG_BUFFERING_DISABLED,
        PROGMEM_MCU1_ONLINE,
        PROGMEM_MCU2_ONLINE,
        PROGMEM_CMD_NOT_FOUND,
        PROGMEM_ICCM_SENDING_DATA,
        PROGMEM_ICCM_RX_BUFFER_DATA,
        PROGMEM_ICCM_RX_BUFFER_OVERFLOW,
        PROGMEM_ICCM_RX_BUFFER_CLEARED,
        PROGMEM_AI_STATUS_IDLE,
        PROGMEM_AI_STATUS_ARMED,
        PROGMEM_AI_STATUS_SEARCH,
        PROGMEM_AI_STATUS_TRACKING,
        PROGMEM_AI_STATUS_ATTACK,
        PROGMEM_AI_STATUS_RETURN,
        PROGMEM_AI_FORCED_STOP,
        PROGMEM_AI_INIT_IN
    } Progmem_Table_Index_T;

    /* Global variables */
    extern char data_conversion_buffer[];
    extern const char* const progmem_string_table[];

#endif /* SERIAL_PROGMEM_GUARD */
