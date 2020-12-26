
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1

#define ICCM_RX PD2
#define ICCM_TX PD3
#define ICCM_DELAY 10

#define COMMON_SERIAL_CMD_LIST \
    {"enbuff", serial_enable_buffering}, \
    {"disbuff", serial_disable_buffering}, \
    {"rdtx", serial_read_tx_buffer}, \
    {"clrtx", serial_clear_tx_buffer}, \
    {"rdrx", serial_read_rx_buffer}, \
    {"clrrx", serial_clear_rx_buffer}, \
    {"iccmdis", iccm_disable}, \
    {"iccmen", iccm_enable} 

#ifdef MCU1
/* Line sensor pins*/
#define LS1 PD4
#define LS2 PD5
#define LS3 PD6
#define LS4 PD7

#define MCU_SPECIFIC_SERIAL_CMD_LIST \

#endif

#ifdef MCU2
/* Refer to TB6612FNG datasheet and PCB design */
#define M1_PWM PC0
#define M1_IN1 PC2
#define M1_IN2 PC1
#define M2_IN1 PC3
#define M2_IN2 PC4
#define M2_PWM PC5
#define M4_IN1 PD4
#define M3_PWM PD5
#define M3_IN2 PD6
#define M3_IN1 PD7
#define M4_IN2 PB0
#define M4_PWM PB1

#define MCU_SPECIFIC_SERIAL_CMD_LIST \
    {"drvfw", drive_ctrl_go_forward}, \
    {"drvbw", drive_ctrl_go_backward}, \
    {"drvtl", drive_ctrl_turn_left}, \
    {"drvtr", drive_ctrl_turn_right}, \
    {"drvinit", drive_ctrl_init}, \
    {"drvpwmen", drive_ctrl_enable_PWM}, \
    {"drvpwmdis", drive_ctrl_disable_PWM}, \
    {"drvstop", drive_ctrl_stop}

#endif




#endif /* CONFIG_GUARD */