/*! @file mcu2.c
    @brief Main loop
    Starting point of software for MCU2
*/
#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_tx.h"
#include "ISR.h"
#include "ICCM.h"
#include "drive_ctrl.h"


/**
 * @brief Main function
 */ 
int main(){
    // char data[] = "Hello from MCU2";
    serial_init(F_CPU, BAUD);
    ICCM_init();
    drive_ctrl_init();
    sei();
    log_info_P(MCU2_ONLINE);

    while(1) /* Loop the messsage continously */
    { 
        drive_ctrl_run();
    }
    return 0;
}