/*! @file ADC.c
    @brief Analog-Digital Converter
*/

#include "ADC.h"
#include "config.h"
#include "serial_tx.h"

void ADC_init(void){
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