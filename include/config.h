
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

/* ATmega setup */
#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1

/* Inter-Chip Communication Manager setup*/
#define ICCM_RX PD2
#define ICCM_TX PD3
#define ICCM_DELAY 10

/* distance sensor config */
#define DS1 PC0
#define DS2 PC1
#define NUM_DISTANCE_SENSORS 1

#endif /* CONFIG_GUARD */