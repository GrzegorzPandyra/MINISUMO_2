
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1

#define ICCM_RX PD2
#define ICCM_TX PD3
#define ICCM_DELAY 10

#endif /* CONFIG_GUARD */