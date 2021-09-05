
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define F_CPU 16000000L
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

/* Inter-Chip Communication Manager setup*/
#define ICCM_RX PD2
#define ICCM_TX PD3
#define ICCM_DELAY_US 30

#define COMMON_SERIAL_CMD_LIST \
{"enbuff", serial_enable_buffering, NULL}, \
{"disbuff", serial_disable_buffering, NULL}, \
{"rdtx", serial_read_tx_buffer, NULL}, \
{"clrtx", serial_clear_tx_buffer, NULL}, \
{"rdrx", serial_read_rx_buffer, NULL}, \
{"clrrx", serial_clear_rx_buffer, NULL}, \
{"iccmdis", ICCM_disable, NULL}, \
{"iccmen", ICCM_enable, NULL}

#ifdef MCU1
    /* Line sensor pins*/
    #define LS1 PD4
    #define LS2 PD5
    #define LS3 PD6
    #define LS4 PD7

    /* AI init btn */
    #define MASTER_INIT PB1

    /* distance sensor config */
    #define DS1 PC0
    #define DS2 PC1

    /* Cmds specific to MCU1*/
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

    /* Cmds specific to MCU2*/
    #define MCU_SPECIFIC_SERIAL_CMD_LIST \
    {"drvinit", drive_ctrl_init, NULL}, \
    {"drvpwm", NULL, drive_ctrl_set_pwm_cbk}, \
    {"drvctrl", NULL, drive_ctrl_set_movement_cbk}, \
    {"drvtr", NULL, drive_ctrl_turn_right_cbk} \

#endif

#endif /* CONFIG_GUARD */