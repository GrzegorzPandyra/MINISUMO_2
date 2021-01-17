/*! @file mcu2.c
    @brief Main loop
    Starting point of software for MCU2
*/

/* Local macro definitions */
/* Local macro-like functions */
/* Local static variables */
/* Global variables */
/* Local static functions */
/* Global functions */

#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial_tx.h"
#include "ISR.h"
#include "iccm.h"
#include "drive_ctrl.h"

void set_debug_pin(){
    PORTB |= 1<<PB2;
}

void clear_debug_pin(){
    PORTB &= ~(1<<PB2);
}

void drv_logic(){
    static char rx_buff[20];
    static uint8_t data_length = 0;
    if(iccm_is_data_available()){
        iccm_read_rx_buffer(rx_buff, &data_length);
        serial_data_str("dt",rx_buff, data_length);
    }

    if(rx_buff[0] == 'f'){
        drive_ctrl_go_forward();
        clear_debug_pin();
    } else if(rx_buff[0] == 'b'){
        drive_ctrl_go_backward();
        set_debug_pin();
    } else if(rx_buff[0] == 'l')
        drive_ctrl_turn_left();
    else if(rx_buff[0] == 'r')
        drive_ctrl_turn_right();
    // _delay_us(100);
    rx_buff[0] = 0;
    rx_buff[1] = 0;
}

/**
 * @brief Main function
 */ 
int main(){
    // char data[] = "Hello from MCU2";
    serial_init(F_CPU, BAUD);
    iccm_init();
    sei();
    DDRB |= 0x01;
    DDRB |= 1<<PB2;
    serial_info_P(MCU2_ONLINE);
    // iccm_disable_rx();
    drive_ctrl_init();
    drive_ctrl_enable_PWM();

    while(1) /* Loop the messsage continously */
    { 
        // serial_info(data);
        // iccm_send("Al");
        // _delay_us(1000);
        // iccm_send("ABCD23456qwerty");
        // if(iccm_is_data_available()){
            // iccm_read_rx_buffer();
        // }
        drv_logic();
    }
    return 0;
}