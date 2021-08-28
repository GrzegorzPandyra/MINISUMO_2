#ifndef ISR_GUARD
#define ISR_GUARD

    /** @file ISR.h
        @brief Interrupt Service Routines
        This file needs to be included in file with "main" function. ISRs are kept separated here for the purpose of clarity. This file is shared between 
        both MCUs, differences are handled by #ifdef sections 
    */

    #include "serial_tx.h"
    #include "serial_rx.h"
    #include "ICCM.h"
    #include "config.h"
    #include "ADC.h"


    #ifdef MCU1
        #include "distance_sensor.h"
        static uint8_t current_ADC_channel = CH_DS1;

        /**
         * @brief Interrupt routine executed when ADC completes conversion
         */
        ISR(ADC_vect){  
            volatile uint16_t adc_val = ADCW;
            switch(current_ADC_channel){
                case CH_DS1:
                    ADC_switch_channel(CH_DS1);
                    distance_sensor_read_ADC(DS1_ID, adc_val);
                    current_ADC_channel = CH_DS2;
                    break;
                case CH_DS2:
                    ADC_switch_channel(CH_DS2);
                    distance_sensor_read_ADC(DS2_ID, adc_val);
                    current_ADC_channel = CH_DS1;
                    break;
                default:
                    break;
            }
        }

        /**
         * @brief
         */
        ISR(TIMER0_OVF_vect){  
            static uint8_t cnt = 0;
            if(cnt >= 5){
                /* Trigger ADC conversion */
                ADCSRA |= 1<<ADSC;
                cnt = 0;
            } else {
                ++cnt;
            }
        }
    #endif
    
    #ifdef MCU2
        #include "drive_ctrl.h"

        /**
         * @brief Interrupt routine to control PWM of motor drivers
         */
        ISR(TIMER0_OVF_vect){  
            drive_ctrl_PWM_processing();
        }  
    #endif

    /**
     * @brief Interrupt routine for USART receive complete bit
     * Reads a single character from UDR and calls function handle this data
     */
    ISR(USART_RXC_vect){
        char c = (char)UDR;
        serial_on_receive(c);
    }

    /**
     * @brief Interrupt routine for ICCM RX pin (INT0)
     * Rising edge on RX pin triggers sampling of RX pin signal. Routine will read defined number of bits equal to the size of data frame. 
     * Then data (one character) will be copied to rx_buffer in ICCM module.
     */
    ISR(INT0_vect){
        ICCM_on_rx_trigger();
    }
    
#endif /* ISR_GUARD */
