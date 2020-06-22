
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define F_CPU 1000000L
#define BAUD 2400
#define MYUBRR F_CPU/16/BAUD-1

#if defined MCU1
    #define SW_RX 4
    #define SW_TX 5
#elif defined MCU2
    #define SW_RX 5
    #define SW_TX 4
#endif

#endif /* CONFIG_GUARD */