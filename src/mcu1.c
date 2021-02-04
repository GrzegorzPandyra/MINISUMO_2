/*! @file mcu1.c
    @brief Main loop
    Starting point of software for MCU1
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
#include "distance_sensor.h"
#include "line_sensor.h"

void adc_init(){
    /* Set DS1 pin as input */
    DDRC &= ~(1<<DS1);
    PORTC &= ~(1<<DS1);
    /* Set prescaler to 128 */
    ADCSRA |= 1<<ADPS0 | 1<<ADPS1 | 1<<ADPS2;
    /* Enable Free Running mode*/
    ADCSRA |= 1<<ADFR;
    /* Internal 2.56V voltage ref */
    ADMUX |= (1<<REFS0);
    /* Set ADC channel to match DS1 */
    ADMUX &= 0xF0;
    /* Enable interrupt on ADC conversion finished */
    ADCSRA |= 1<<ADIE;
    /* Enable ADC*/
    ADCSRA |= 1<<ADEN;
    /* Trigger initial conversion */
    ADCSRA |= 1<<ADSC;
}   

void set_debug_pin(){
    PORTC |= 1<<PC3;
}

void clear_debug_pin(){
    PORTC &= ~(1<<PC3);
}

void logic(){
    uint8_t ls_status = line_sensor_get_status();
    if(ls_status)
        set_debug_pin();
    else
        clear_debug_pin();
    
    if(ls_status & 0x01 && ls_status & 0x02)
        iccm_send("b");
    else if(ls_status & 0x04 && ls_status & 0x08)
        iccm_send("f");
    else if(ls_status & 0x01 && ls_status & 0x08)
        iccm_send("l");
    else if(ls_status & 0x02 && ls_status & 0x04)
        iccm_send("r");
}

/**
 * @brief Main function
 */ 
int main(){
    serial_init(F_CPU, BAUD);
    iccm_init();
    adc_init();
    sei();
    DDRB |= 0x01;
    DDRC |= 1<<PC3;
    serial_info_P(MCU1_ONLINE);
    uint8_t cnt = 0;
    uint8_t ds1_status = 0;
    while(1) /* Loop the messsage continously */
    { 
        // _delay_ms(100);
        // ds1_status = distance_sensor_get_status();
        // serial_data_uint8("DS1 status:", &ds1_status, 1);
        // iccm_send("D11111112222222");
        // if(iccm_is_data_available()){
        //     iccm_read_rx_buffer();
        // }
        // _delay_us(200);
        // iccm_send("D11111112222222");
        // if(iccm_is_data_available()){
            // iccm_read_rx_buffer();
        // }
        // logic();
    }
    return 0;
}