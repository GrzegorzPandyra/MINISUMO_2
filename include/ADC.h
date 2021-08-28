#ifndef ADC_GUARD
#define ADC_GUARD

/*! @file ADC.h
    @brief API for Analog-Digital Converter
*/

typedef enum ADC_CHANNEL_ID_Tag{
    CH_DS1 = 0xC1,
    CH_DS2 = 0xC0
}ADC_CHANNEL_ID_T;

void ADC_init(void);
void ADC_switch_channel(ADC_CHANNEL_ID_T channel_id);

#endif /* ADC_GUARD */
