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
#include "distance_sensor.h"
#include "line_sensor.h"

/* Disable debug logs if AI_DEBUG is not defined during build */
#ifndef AI_DEBUG
    #undef log_info_P
    #define log_info_P(str)
    #undef log_info
    #define log_info(str)
    #undef log_warn
    #define log_warn(str)
    #undef log_warn_P
    #define log_warn_P(str)
    #undef log_err
    #define log_err(str)
    #undef log_err_P
    #define log_err_P(str)
    #undef log_raw_string
    #define log_raw_string(str)
#endif

#define DS_TRIGGER_LEVEL_1 10
#define DS_TRIGGER_LEVEL_2 100
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
static void DS_target_locked(void);
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
    DS_TARGET_LOCKED = 21,
    NO_SENSOR_INPUT = 22,
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
    {DS_TARGET_LOCKED, DS_target_locked},
    {NO_SENSOR_INPUT, no_sensor_input},
    {STOP, stop},
    {UNKNOWN, NULL}
};

/**
 * @brief Time of 90 deg rotation is dependant on the speed of motors which is dependant on PWM value.
 * This table holds PWM vs time delays to achieve 90 deg rotation.
 */
static const Rotation_Adjustment_Record_T ROTATION_ADJUSTMENT_TABLE[] = {
    /* {PWM, delay_us} */
    {0,0}, /* Unavailable */
    {0,0}, /* Unavailable */
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
static uint16_t DS1_reading = 0;
static uint16_t DS2_reading = 0;
static uint8_t LS_readings = 0;

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

/**********************************************************************
* Implementation of AI vectors 
***********************************************************************/
static void stop(void){
    ICCM_send(MOTORS_STOP);
}

static void LS1_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
}

static void LS2_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
}

static void LS3_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
}

static void LS4_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_FORWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
}

static void LS1_LS2_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_BACKWARD);
    _delay_ms(100);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(2*get_rotation_delay());
}

static void LS2_LS3_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_TURN_LEFT);
    variable_delay_ms(get_rotation_delay());
}

static void LS3_LS4_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void LS4_LS1_triggered(void){
    current_PWM = DEFAULT_PWM_VALUE;
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_TURN_RIGHT);
    variable_delay_ms(get_rotation_delay());
}

static void DS_triggered(void){
    // uint16_t initial_ds_reading = distance_sensor_get_status();
    // ICCM_send(MOTORS_PWM_70);
    // ICCM_send(MOTORS_TURN_LEFT);
    // _delay_ms(100);
    // uint16_t new_ds_reading = distance_sensor_get_status();
    // if(new_ds_reading > initial_ds_reading){
    //     log_info("qq");
    //     DS_target_locked();
    // } else {
    //     ICCM_send(MOTORS_TURN_RIGHT);
    //     _delay_ms(200);
    //     new_ds_reading = distance_sensor_get_status();
    //     if(new_ds_reading > initial_ds_reading){
    //         DS_target_locked();
    //     } else {
    //         ICCM_send(MOTORS_TURN_LEFT);
    //         _delay_ms(100);
    //         ICCM_send(MOTORS_PWM_50);
    //         ICCM_send(MOTORS_GO_FORWARD);
    //     }
    // }
}

static void DS_target_locked(void){
    ICCM_send(MOTORS_PWM_100);
    ICCM_send(MOTORS_GO_FORWARD);
}

static void no_sensor_input(void){
    ICCM_send(MOTORS_PWM_50);
    ICCM_send(MOTORS_GO_FORWARD);
}

/**********************************************************************
* Determining the vector 
***********************************************************************/
static AI_Vector_T get_vector_by_ID(AI_Vector_ID_T id){
    AI_Vector_T result = AI_VECTORS[11];
    for(uint8_t i = 0; i<arr_length(AI_VECTORS); i++){
        if(AI_VECTORS[i].id == id){
            result = AI_VECTORS[i];
            break;
        }
    }
    return result;
}

static AI_Vector_T calculate_vector(uint8_t ls_reading, uint16_t ds_reading){
    // AI_Vector_T result = get_vector_by_ID((AI_Vector_ID_T)ls_reading);
    AI_Vector_T result = {UNKNOWN, NULL};
    AI_Status_T previous_AI_status = AI_status;
    if(result.id == UNKNOWN){
        if(ds_reading > DS_TRIGGER_LEVEL_1 && ds_reading < DS_TRIGGER_LEVEL_2){
            result = get_vector_by_ID(DS_TRIGGERED);
            AI_status = AI_TRIGGERED;
        } else if(ds_reading >= DS_TRIGGER_LEVEL_2){
            result = get_vector_by_ID(DS_TARGET_LOCKED);
            AI_status = AI_ATTACK;
        } else {
            // result = get_vector_by_ID(NO_SENSOR_INPUT);
            log_info("no input");
            AI_status = AI_SEARCH;
        }
    } else {
        AI_status = AI_RETURN;
    }
    if(previous_AI_status != AI_status){
        print_AI_status();
    }
    return result;
}

/**********************************************************************
* Public functions 
***********************************************************************/
void print_AI_status(void){
    switch (AI_status){
        case AI_IDLE:
            log_info_P(PROGMEM_AI_STATUS_IDLE);
            break;
        case AI_ARMED:
            log_info_P(PROGMEM_AI_STATUS_ARMED);
            break;
        case AI_TRIGGERED:
            log_info_P(PROGMEM_AI_STATUS_TRIGGERED);
        case AI_SEARCH:
            log_info_P(PROGMEM_AI_STATUS_SEARCH);
            break;
        case AI_ATTACK:
            log_info_P(PROGMEM_AI_STATUS_ATTACK);
            break;
        case AI_RETURN:
            log_info_P(PROGMEM_AI_STATUS_RETURN);
            break;
        default:
            log_err("AI status unknown");
            break;
    }
}

AI_Status_T AI_get_status(void){
    return AI_status;
}

void AI_run(void){
    /* Read button to Start/Stop AI */
    if((PINB & (1<<MASTER_INIT)) == 0){
        if(AI_get_status() == AI_IDLE){
            AI_init();
        } else {
            AI_force_stop();
            print_AI_status();
        }
    }
    /* Process AI status */
    switch (AI_get_status()){
        case AI_SEARCH:
        case AI_ATTACK:
        case AI_RETURN:
            DS1_reading = distance_sensor_get_status(DS1_ID);
            DS2_reading = distance_sensor_get_status(DS2_ID);
            log_data_2("DS1=%d DS2=%d",DS1_reading, DS2_reading);
            LS_readings = line_sensor_get_status();
            // const AI_Vector_T vect = calculate_vector(LS_readings, DS_reading);
            // vect.cbk();
            _delay_ms(5);
            break;
        case AI_IDLE:
        case AI_ARMED:
            /* Do nothing */
            break;
        default:
            break;
    }
}

void AI_init(void){
    AI_status = AI_ARMED;
    log_info_P(PROGMEM_AI_STATUS_ARMED);
    log_info_P(PROGMEM_AI_INIT_IN);
    // log_raw_string("5..\n");
    // _delay_ms(INIT_DELAY_MS);
    // log_raw_string("4..\n");
    // _delay_ms(INIT_DELAY_MS);
    // log_raw_string("3..\n");
    // _delay_ms(INIT_DELAY_MS);
    // log_raw_string("2..\n");
    // _delay_ms(INIT_DELAY_MS);
    // log_raw_string("1..\n");
    // _delay_ms(INIT_DELAY_MS);
    log_info_P(PROGMEM_AI_STATUS_SEARCH);
    AI_status = AI_SEARCH;
    // ICCM_send(MOTORS_GO_FORWARD);
    ICCM_send(MOTORS_PWM_50);
}

void AI_force_stop(void){
    get_vector_by_ID(STOP).cbk();
    AI_status = AI_IDLE;
    log_info_P(PROGMEM_AI_FORCED_STOP);
    _delay_ms(FORCE_STOP_DELAY_MS);
}