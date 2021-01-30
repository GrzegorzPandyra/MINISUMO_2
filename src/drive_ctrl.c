/** @file drive_ctrl.c
*   @brief Interface for controlling the motors
*   Motors are numbered as follows:
*   |1 2|
*   |4 3|
*/
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "common_const.h"
#include "serial_tx.h"
#include "iccm.h"

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

/**
 * API functions
 */
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

void drive_ctrl_stop(void){
    PORTC &= CB(M1_IN1);
    PORTC &= CB(M1_IN2);
    PORTC &= CB(M2_IN1);
    PORTC &= CB(M2_IN2);
    PORTD &= CB(M3_IN1);
    PORTD &= CB(M3_IN2);
    PORTD &= CB(M4_IN1);
    PORTB &= CB(M4_IN2);
}

void drive_ctrl_go_forward(void){
    wheel_1_cw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_cw();
}

void drive_ctrl_go_backward(void){
    wheel_1_ccw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_ccw();
}

void drive_ctrl_turn_right(void){
    wheel_1_cw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_cw();
}

void drive_ctrl_turn_left(void){
    wheel_1_ccw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_ccw();
}

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
}

void drive_ctrl_PWM(void){
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

void drive_ctrl_set_PWM(const uint8_t pwm){
    PWM = (pwm > MAX_PWM)?MAX_PWM:pwm;
}