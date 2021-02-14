#ifndef DRIVE_CONTROL_GUARD
#define DRIVE_CONTROL_GUARD
/*! @file drive_ctrl.h
    @brief API to control the movement of the robot
*/
void drive_ctrl_init(void);
void drive_ctrl_PWM_processing(void);
void drive_ctrl_run(void);
void drive_ctrl_set_pwm_cbk(const void *data, size_t data_len);
void drive_ctrl_set_movement(const void *data, size_t data_len);
#endif /* DRIVE_CONTROL_GUARD */




