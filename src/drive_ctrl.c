/** @file drive_ctrl.c
*   @brief Interface for controlling the motors (direction and power)
*   Motors are numbered as follows:
*   |1 2|
*   |4 3|
*
*   @note PWM control works in range of 20% - 100% with step every 10%. Values below 20% are insufficient for turning around, therefore should not be used.
*   @note Functions ended with _cbk suffix are for debugging only!
*/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "common_const.h"
#include "serial_tx.h"
#include "ICCM.h"
#include "ICCM_message_catalog.h"
#include <util/delay.h>

#define CB(x) (~(1<<x))
#define SB(x) (1<<x)
#define ENABLE_M1_PWM() (PORTC|=SB(M1_PWM))
#define ENABLE_M2_PWM() (PORTC|=SB(M2_PWM))
#define ENABLE_M3_PWM() (PORTD|=SB(M3_PWM))
#define ENABLE_M4_PWM() (PORTB|=SB(M4_PWM))
#define DISABLE_M1_PWM() (PORTC&=CB(M1_PWM))
#define DISABLE_M2_PWM() (PORTC&=CB(M2_PWM))
#define DISABLE_M3_PWM() (PORTD&=CB(M3_PWM))
#define DISABLE_M4_PWM() (PORTB&=CB(M4_PWM))
#define MAX_INT_CNT 100
#define MAX_PWM MAX_INT_CNT
#define TCNT0_INITIAL 254
#define DRVPWM_CMD_ARGUMENT_OFFSET 7
#define DRVTR_ARGUMENT_OFFSET 6
#define ASCII_NUM_OFFSET 48

static uint8_t PWM = 80;

static void timer0_init(void){
    /* Set timer clk source and prescaler(256) */
    TCCR0 |= (1<<CS02); 
    /* Enable timer0 overflow interrupt */
    TIMSK |= (1<<TOIE0); 
}

/**
 * Set of functions to control the direction (CW or CCW) of every wheel
 */
static void wheel_1_cw(void){
    PORTC |= SB(M1_IN1);
    PORTC &= CB(M1_IN2);
}

static void wheel_1_ccw(void){
    PORTC &= CB(M1_IN1);
    PORTC |= SB(M1_IN2);
}

static void wheel_2_cw(void){
    PORTC |= SB(M2_IN1);
    PORTC &= CB(M2_IN2);
}

static void wheel_2_ccw(void){
    PORTC &= CB(M2_IN1);
    PORTC |= SB(M2_IN2);
}

static void wheel_3_cw(void){
    PORTD |= SB(M3_IN1);
    PORTD &= CB(M3_IN2);
}

static void wheel_3_ccw(void){
    PORTD &= CB(M3_IN1);
    PORTD |= SB(M3_IN2);
}

static void wheel_4_cw(void){
    PORTD |= SB(M4_IN1);
    PORTB &= CB(M4_IN2);
}

static void wheel_4_ccw(void){
    PORTD &= CB(M4_IN1);
    PORTB |= SB(M4_IN2);
}

/* Set of functions to combine wheel movements into robot movement*/
static void stop(void){
    PORTC &= CB(M1_IN1);
    PORTC &= CB(M1_IN2);
    PORTC &= CB(M2_IN1);
    PORTC &= CB(M2_IN2);
    PORTD &= CB(M3_IN1);
    PORTD &= CB(M3_IN2);
    PORTD &= CB(M4_IN1);
    PORTB &= CB(M4_IN2);
}

static void go_forward(void){
    wheel_1_cw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_cw();
}

static void go_backward(void){
    wheel_1_ccw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_ccw();
}

static void turn_right(void){
    wheel_1_cw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_cw();
}

static void turn_left(void){
    wheel_1_ccw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_ccw();
}

static void set_PWM(const uint8_t pwm){
    PWM = (pwm > MAX_PWM)?MAX_PWM:pwm;
}

/* Public control of PWM */
void drive_ctrl_disable_PWM(void){
    DISABLE_M1_PWM();
    DISABLE_M2_PWM();
    DISABLE_M3_PWM();
    DISABLE_M4_PWM();
}

void drive_ctrl_enable_PWM(void){
    ENABLE_M1_PWM();
    ENABLE_M2_PWM();
    ENABLE_M3_PWM();
    ENABLE_M4_PWM();
}

/**
 * @brief Used in ISR to provide smooth PWM switching (without visible motor turn-on/off)
 */
void drive_ctrl_PWM_processing(void){
    static uint8_t cnt = 0;
    if(cnt < PWM){
        drive_ctrl_enable_PWM();
    } else {
        drive_ctrl_disable_PWM();
    }
    if(cnt >= MAX_INT_CNT){
        cnt = 0;
    } else {
        ++cnt;
    }
    TCNT0 = TCNT0_INITIAL;
}

/* High-level functions*/
void drive_ctrl_init(void){
    timer0_init(); 

    /*motor 1*/
    DDRC |= SB(M1_IN1);
    DDRC |= SB(M1_IN2);
    DDRC |= SB(M1_PWM);
    /*motor 2*/
    DDRC |= SB(M2_IN1);
    DDRC |= SB(M2_IN2);
    DDRC |= SB(M2_PWM);
    /*motor 3*/
    DDRD |= SB(M3_IN1);
    DDRD |= SB(M3_IN2);
    DDRD |= SB(M3_PWM);
    /*motor 4*/
    DDRD |= SB(M4_IN1);
    DDRB |= SB(M4_IN2);
    DDRB |= SB(M4_PWM);
    
    drive_ctrl_enable_PWM();
}

void drive_ctrl_run(void){
    static char rx_buff[10];
    static uint8_t data_length = 0;
    if(ICCM_is_data_available()){
        ICCM_read_rx_buffer(rx_buff, &data_length);
        switch (rx_buff[0])
        {
        case MOTORS_GO_FORWARD:
            go_forward();
            break;
        case MOTORS_GO_BACKWARD:
            go_backward();
            break;
        case MOTORS_TURN_RIGHT:
            turn_right();
            break;
        case MOTORS_TURN_LEFT:
            turn_left();
            break;
        case MOTORS_STOP:
            stop();
            break;
        case MOTORS_PWM_20:
            set_PWM(20);
            break;
        case MOTORS_PWM_30:
            set_PWM(30);
            break;
        case MOTORS_PWM_40:
            set_PWM(40);
            break;
        case MOTORS_PWM_50:
            set_PWM(50);
            break;
        case MOTORS_PWM_60:
            set_PWM(60);
            break;
        case MOTORS_PWM_70:
            set_PWM(70);
            break;
        case MOTORS_PWM_80:
            set_PWM(80);
            break;
        case MOTORS_PWM_90:
            set_PWM(90);
            break;
        case MOTORS_PWM_100:
            set_PWM(100);
            break;
        default:
            break;
        }
    } 
}

/* Debug callbacks */
/**
 * @brief Debug function for serial module to allow PWM setting via UART
 * @param data cmd with parameter: drvpwm xxx, where xxx is PWM value in range of 0-100
 * @param data_len size of @data
 */
void drive_ctrl_set_pwm_cbk(const void *data, size_t data_len){
    uint8_t arg_value = atoi(((const char*)data)+DRVPWM_CMD_ARGUMENT_OFFSET);
    set_PWM(arg_value);
}

/**
 * @brief Debug function for motor control
 * @param data data in format: drvctrl f/b/l/r/s, example: drvctrl f
 * @param data_len size of @data
 */
void drive_ctrl_set_movement_cbk(const void *data, size_t data_len){
    char subcmd = ((const char*)data)[8];
    switch (subcmd){
    case 'f':
        go_forward();
        break;
    case 'b':
        go_backward();
        break;
    case 'l':
        turn_left();
        break;
    case 'r':
        turn_right();
        break;
    case 's':
        stop();
        break;
    default:
        break;
    }
}

void drive_ctrl_turn_right_cbk(const void *data, size_t data_len){
    const uint8_t *data_ptr = (const uint8_t*)data;
    uint16_t arg_value = (data_ptr[DRVTR_ARGUMENT_OFFSET]-ASCII_NUM_OFFSET)*100;
    arg_value += (data_ptr[DRVTR_ARGUMENT_OFFSET+1]-ASCII_NUM_OFFSET)*10;
    arg_value += (data_ptr[DRVTR_ARGUMENT_OFFSET+2]-ASCII_NUM_OFFSET);
    turn_right();
    switch (arg_value){
    case 100:
        _delay_ms(100);
        break;
    case 500:
        _delay_ms(500);
        break;
    case 999:
        _delay_ms(999);
        break;
    default:
        _delay_ms(100);
        break;
    }
    stop();
}