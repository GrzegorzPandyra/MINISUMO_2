#ifndef SERIAL_PROGMEM_GUARD
#define SERIAL_PROGMEM_GUARD
    /*! @file serial_progmem.h
        @brief Declarations needed to access PROGMEM data in flash
    */

    /** Indexes related to strings stored in progmem_string_table */                                                                                            
    typedef enum Progmem_Table_Index_Tag{
        TX_BUFFER_OVERFLOW = 0,
        LOG_BUFFERING_ENABLED,
        LOG_BUFFERING_DISABLED,
        MCU1_ONLINE,
        MCU2_ONLINE,
        CMD_NOT_FOUND,
        ICCM_SENDING_DATA,
        ICCM_RX_BUFFER_DATA,
        ICCM_RX_BUFFER_OVERFLOW,
        ICCM_RX_BUFFER_CLEARED,
        AI_STATUS_IDLE,
        AI_STATUS_ARMED,
        AI_STATUS_SEARCH,
        AI_STATUS_ATTACK,
        AI_STATUS_R2R,
        AI_FORCED_STOP,
        AI_INIT_IN_5,
        AI_INIT_IN_4,
        AI_INIT_IN_3,
        AI_INIT_IN_2,
        AI_INIT_IN_1,
        DRV_CTRL_INIT,
        ICCM_INIT,
        SERIAL_INIT,
        ADC_INIT
    } Progmem_Table_Index_T;

    /* Global variables */
    extern char data_conversion_buffer[];
    extern const char* progmem_string_table[];

#endif /* SERIAL_PROGMEM_GUARD */
