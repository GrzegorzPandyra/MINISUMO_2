#ifndef AI_GUARD
#define AI_GUARD

/*! @file AI.h
    @brief Header for AI module
*/

typedef enum AI_Status_Tag{
    AI_IDLE = 0,
    AI_ARMED,
    AI_SEARCH,
    AI_TRIGGERED,
    AI_ATTACK,
    AI_R2R
} AI_Status_T;

void AI_run();
void AI_init(void);
AI_Status_T AI_get_status(void);
void AI_force_stop(void);
void AI_turn_right_cbk(const void *data, size_t data_len);
void print_AI_status(void);

#endif /* AI_GUARD */
