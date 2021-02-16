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
#define RUN_DELAY_MS 5
#define INIT_DELAY_MS 1000
#define FORCE_STOP_DELAY_MS 1000
#define DEFAULT_PWM_VALUE 50

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
    UNKNOWN = 0,
    LS1_TRIGGERED = 1,
    LS2_TRIGGERED = 2,
    LS3_TRIGGERED = 4,
    LS4_TRIGGERED = 8,
    LS1_LS2_TRIGGERED = 3,
    LS2_LS3_TRIGGERED = 6,
    LS3_LS4_TRIGGERED = 12,
    LS4_LS1_TRIGGERED = 9,
    DS_TRIGGERED = 20,
    NO_SENSOR_INPUT = 21,
    STOP = 22,
} AI_Vector_ID_T;

typedef void (*Vector_Cbk)(void);

typedef struct AI_Vector_Tag {
    AI_Vector_ID_T id;
    Vector_Cbk cbk;
} AI_Vector_T;

typedef struct Rotation_Adjustment_Record_Tag{
    const uint8_t PWM;
    const uint16_t rotation_time;
}Rotation_Adjustment_Record_T;

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
    {STOP, stop},
    {UNKNOWN, 0}
};

static const Rotation_Adjustment_Record_T ROTATION_ADJUSTMENT_TABLE[] = {
    {0,0},
    {0,0},
    {20, 1286},
    {30, 500},
    {40, 333},
    {50, 237},
    {60, 200},
    {70, 148},
    {80, 136},
    {90, 111},
    {100, 100}
};

static AI_Status_T AI_status = AI_IDLE;
static uint8_t current_PWM = DEFAULT_PWM_VALUE;

/**
 * @brief Get the rotation delay coresponding to current PWM value
 * @return uint16_t delay in milliseconds
 */
static uint16_t get_rotation_delay(void){
    return ROTATION_ADJUSTMENT_TABLE[current_PWM/10].rotation_time;
}

/**
 * @brief Wrapper for _delay_ms() function to allow dynamic delays
 * @param delay Delay in milliseconds
 */
static void variable_delay_ms(uint16_t delay){
    for(uint16_t i = 0; i<delay; i++){
        _delay_ms(1);
    }
}

/* Implementation of AI vectors */
static void stop(void){
    ICCM_send(MOTORS_STOP);
}

static void LS1_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS2_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS3_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_BACKWARD);
}

static void LS4_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_BACKWARD);
}

static void LS1_LS2_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(2*get_rotation_delay());
}

static void LS2_LS3_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS3_LS4_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS4_LS1_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_30);
    _delay_ms(1);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
    ICCM_send(MOTORS_GO_FORWARD);
}

static void DS_triggered(void){
    ICCM_send(MOTORS_PWM_50);
    _delay_ms(1);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void no_sensor_input(void){
    ICCM_send(MOTORS_PWM_30);
}

static AI_Vector_T get_vector_by_ID(AI_Vector_ID_T id){
    AI_Vector_T result = AI_VECTORS[0];
    for(uint8_t i = 0; i<arr_length(AI_VECTORS); i++){
        if(AI_VECTORS[i].id == id){
            result = AI_VECTORS[i];
            break;
        }
    }
    return result;
}

static AI_Vector_T calculate_vector(uint8_t ls_reading, uint16_t ds_reading){
    AI_Vector_T result = get_vector_by_ID((AI_Vector_ID_T)ls_reading);
    if(result.id == UNKNOWN){
        if(ds_reading > DS_TRIGGER_LEVEL){
            result = get_vector_by_ID(DS_TRIGGERED);
        } else {
            result = get_vector_by_ID(NO_SENSOR_INPUT);
        }
    }    
    return result;
}

void AI_run(uint8_t ls_readings, uint16_t ds_reading){
    const AI_Vector_T vect = calculate_vector(ls_readings, ds_reading);
    vect.cbk();
    _delay_ms(5);
}

void AI_init(void){
    AI_status = AI_ARMED;
    log_info_P(AI_STATUS_ARMED);
    log_info_P(AI_INIT_IN);
    serial_log_raw_string("5..");
    _delay_ms(INIT_DELAY_MS);
    serial_log_raw_string("4..");
    _delay_ms(INIT_DELAY_MS);
    serial_log_raw_string("3..");
    _delay_ms(INIT_DELAY_MS);
    serial_log_raw_string("2..");
    _delay_ms(INIT_DELAY_MS);
    serial_log_raw_string("1..");
    _delay_ms(INIT_DELAY_MS);
    AI_status = AI_SEARCH;
    log_info_P(AI_STATUS_SEARCH);
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(1);
    ICCM_send(MOTORS_PWM_30);
}

AI_Status_T AI_get_status(void){
    return AI_status;
}

void AI_force_stop(void){
    get_vector_by_ID(STOP).cbk();
    AI_status = IDLE;
    log_info_P(AI_FORCED_STOP);
    _delay_ms(FORCE_STOP_DELAY_MS);
}