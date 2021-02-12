#ifndef AI_GUARD
#define AI_GUARD

/*! @file AI.h
    @brief 
*/

typedef enum AI_Status_Tag{
    AI_IDLE = 0,
    AI_ARMED,
    AI_SEARCH,
    AI_ATTACK,
    AI_R2R
} AI_Status_T;

void AI_run(uint8_t ls_reading, uint16_t ds_reading);
void AI_init(void);
AI_Status_T AI_get_status(void);
void AI_force_stop(void);

#endif /* AI_GUARD */
