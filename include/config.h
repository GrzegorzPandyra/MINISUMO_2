
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1

#define ICCM_RX PD2
#define ICCM_TX PD3
#define ICCM_DELAY 10

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

#endif /* CONFIG_GUARD */