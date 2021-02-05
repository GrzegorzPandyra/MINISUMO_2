/*! @file AI.c
    @brief This module is responsible for decision-making based on sensor input
*/

#include "config.h"
#include "common_const.h"
#include "serial_tx.h"
#include "ICCM.h"
#include "ICCM_message_catalog.h"
#include "AI.h"
#include <util/delay.h>

#define DS_TRIGGER_LEVEL 100
#define DS_TRIGGER_VECTOR_INDEX 8

static void stop(void);
static void LS1_triggered(void);
static void LS2_triggered(void);
static void LS3_triggered(void);
static void LS4_triggered(void);
static void LS1_LS2_triggered(void);
static void LS2_LS3_triggered(void);
static void LS3_LS4_triggered(void);
static void LS4_LS1_triggered(void);
static void DS_triggered(void);
static void no_sensor_input(void);

typedef enum AI_Vector_ID_Tag {
    STOP = 0,
    LS1_TRIGGERED = 1,
    LS2_TRIGGERED = 2,
    LS3_TRIGGERED = 4,
    LS4_TRIGGERED = 8,
    LS1_LS2_TRIGGERED = 3,
    LS2_LS3_TRIGGERED = 6,
    LS3_LS4_TRIGGERED = 12,
    LS4_LS1_TRIGGERED = 9,
    DS_TRIGGERED = 20,
    NO_SENSOR_INPUT = 21
} AI_Vector_ID_T;

typedef void (*Vector_Cbk)(void);

typedef struct AI_Vector_Tag {
    AI_Vector_ID_T id;
    Vector_Cbk cbk;
} AI_Vector_T;

static const AI_Vector_T AI_VECTORS[] = {
    {LS1_TRIGGERED, LS1_triggered},
    {LS2_TRIGGERED, LS2_triggered},
    {LS3_TRIGGERED, LS3_triggered},
    {LS4_TRIGGERED, LS4_triggered},
    {LS1_LS2_TRIGGERED, LS1_LS2_triggered},
    {LS2_LS3_TRIGGERED, LS2_LS3_triggered},
    {LS3_LS4_TRIGGERED, LS3_LS4_triggered},
    {LS4_LS1_TRIGGERED, LS4_LS1_triggered},
    {DS_TRIGGERED, DS_triggered},
    {NO_SENSOR_INPUT, no_sensor_input},
    {STOP, stop}
};

static void stop(void){
    ICCM_send(MOTORS_STOP);
}

static void LS1_triggered(void){
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(200);
    ICCM_send(MOTORS_TURN_RIGHT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS2_triggered(void){
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(200);
    ICCM_send(MOTORS_TURN_LEFT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS3_triggered(void){
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(200);
    ICCM_send(MOTORS_TURN_LEFT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_BACKWARD);
}

static void LS4_triggered(void){
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(200);
    ICCM_send(MOTORS_TURN_RIGHT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_BACKWARD);
}

static void LS1_LS2_triggered(void){
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    _delay_ms(400);
}

static void LS2_LS3_triggered(void){
    ICCM_send(MOTORS_TURN_LEFT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS3_LS4_triggered(void){
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS4_LS1_triggered(void){
    ICCM_send(MOTORS_TURN_RIGHT);
    _delay_ms(200);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void DS_triggered(void){
    ICCM_send(MOTORS_GO_FORWARD);
    ICCM_send(MOTORS_PWM_100);
}

static void no_sensor_input(void){
    ICCM_send(MOTORS_PWM_50);
}

static AI_Vector_T calculate_vector(uint8_t ls_reading, uint16_t ds_reading){
    AI_Vector_T result;
    bool vector_calculated = false;
    for(uint8_t i = 0; i<arr_length(AI_VECTORS); i++){
        if(AI_VECTORS[i].id == (AI_Vector_ID_T)ls_reading){
            result = AI_VECTORS[i];
            vector_calculated = true;
            break;
        }
    }
    if(!vector_calculated && (ds_reading > DS_TRIGGER_LEVEL)){
        result = AI_VECTORS[DS_TRIGGER_VECTOR_INDEX];
        vector_calculated = true;
    }
    return result;
}

void AI_run(uint8_t ls_readings, uint16_t ds_reading){
    const AI_Vector_T vect = calculate_vector(ls_readings, ds_reading);
    vect.cbk();
}

void AI_init(void){
    ICCM_send(MOTORS_PWM_50);
}