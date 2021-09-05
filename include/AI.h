#ifndef AI_GUARD
#define AI_GUARD

/*! @file AI.h
    @brief Header for AI module
*/

typedef enum AI_Status_Tag{
    AI_IDLE = 0,
    AI_ARMED,       /* Set to activate in few secconds (button trigger) */
    AI_SEARCH,      /* Actively searching for target*/
    AI_TRACKING,    /* Distance sensors triggered, no confirmation on target*/
    AI_ATTACK,      /* Target confirmed, moving in */
    AI_RETURN       /* Line detected, returning inside the ring */
} AI_Status_T;

void AI_run();
void AI_init(void);
AI_Status_T AI_get_status(void);
void AI_force_stop(void);
void print_AI_status(void);

#endif /* AI_GUARD */
