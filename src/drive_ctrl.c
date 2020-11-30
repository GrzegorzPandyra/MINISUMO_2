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

/**
 * PWM signal is used only to enable/disable the motor driver, not to control the motor speed 
 */
#define ENABLE_M1_PWM() (PORTC|=SB(M1_PWM))
#define ENABLE_M2_PWM() (PORTC|=SB(M2_PWM))
#define ENABLE_M3_PWM() (PORTD|=SB(M3_PWM))
#define ENABLE_M4_PWM() (PORTB|=SB(M4_PWM))
#define DISABLE_M1_PWM() (PORTC&=CB(M1_PWM))
#define DISABLE_M2_PWM() (PORTC&=CB(M2_PWM))
#define DISABLE_M3_PWM() (PORTD&=CB(M3_PWM))
#define DISABLE_M4_PWM() (PORTB&=CB(M4_PWM))


/**
 * Set of functions to control the direction (CW or CCW) of every wheel
 */

static void wheel_1_cw(){
    PORTC |= SB(M1_IN1);
    PORTC &= CB(M1_IN2);
}

static void wheel_1_ccw(){
    PORTC &= CB(M1_IN1);
    PORTC |= SB(M1_IN2);
}

static void wheel_2_cw(){
    PORTC |= SB(M2_IN1);
    PORTC &= CB(M2_IN2);
}

static void wheel_2_ccw(){
    PORTC &= CB(M2_IN1);
    PORTC |= SB(M2_IN2);
}

static void wheel_3_cw(){
    PORTD |= SB(M3_IN1);
    PORTD &= CB(M3_IN2);
}

static void wheel_3_ccw(){
    PORTD &= CB(M3_IN1);
    PORTD |= SB(M3_IN2);
}

static void wheel_4_cw(){
    PORTD |= SB(M4_IN1);
    PORTB &= CB(M4_IN2);
}

static void wheel_4_ccw(){
    PORTD &= CB(M4_IN1);
    PORTB |= SB(M4_IN2);
}

/**
 * API functions
 */
void drive_ctrl_disable_PWM(){
    DISABLE_M1_PWM();
    DISABLE_M2_PWM();
    DISABLE_M3_PWM();
    DISABLE_M4_PWM();
}

void drive_ctrl_enable_PWM(){
    ENABLE_M1_PWM();
    ENABLE_M2_PWM();
    ENABLE_M3_PWM();
    ENABLE_M4_PWM();
}

void drive_ctrl_stop(){
    PORTC &= CB(M1_IN1);
    PORTC &= CB(M1_IN2);
    PORTC &= CB(M2_IN1);
    PORTC &= CB(M2_IN2);
    PORTD &= CB(M3_IN1);
    PORTD &= CB(M3_IN2);
    PORTD &= CB(M4_IN1);
    PORTB &= CB(M4_IN2);
}

void drive_ctrl_go_forward(){
    wheel_1_cw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_cw();
}

void drive_ctrl_go_backward(){
    wheel_1_ccw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_ccw();
}

void drive_ctrl_turn_right(){
    wheel_1_cw();
    wheel_2_ccw();
    wheel_3_ccw();
    wheel_4_cw();
}

void drive_ctrl_turn_left(){
    wheel_1_ccw();
    wheel_2_cw();
    wheel_3_cw();
    wheel_4_ccw();
}

void drive_ctrl_init(){ 
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