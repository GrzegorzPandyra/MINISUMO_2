/*! @file ADC.c
    @brief Analog-Digital Converter
    ADC conversion is triggered using TIMER0 (no Free-Running mode). This is to prevent choking of ICCM.
*/

#include "ADC.h"
#include "config.h"
#include "serial_tx.h"

static void timer0_init(void){
    /* Set timer clk source and prescaler(256) */
    TCCR0 |= (1<<CS02)|(1<<CS00);
    /* Enable timer0 overflow interrupt */
    TIMSK |= (1<<TOIE0); 
}

void ADC_init(void){
    /* Set prescaler to 128 */
    ADCSRA |= 1<<ADPS0 | 1<<ADPS1 | 1<<ADPS2;
    /* Internal 2.56V voltage ref */
    ADMUX |= (1<<REFS0);
    /* Set ADC channel to match DS1 */
    ADMUX &= 0xF0;
    /* Enable interrupt on ADC conversion finished */
    ADCSRA |= 1<<ADIE;
    /* Enable ADC*/
    ADCSRA |= 1<<ADEN;
    timer0_init();
    log_info_P(ADC_INIT);
} 