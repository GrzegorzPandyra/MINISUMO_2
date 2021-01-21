#ifndef DRIVE_CONTROL_GUARD
#define DRIVE_CONTROL_GUARD
/*! @file drive_ctrl.h
    @brief API to control the movement of the robot
*/
void drive_ctrl_init(void);
void drive_ctrl_go_forward(void);
void drive_ctrl_go_backward(void);
void drive_ctrl_turn_right(void);
void drive_ctrl_turn_left(void);
void drive_ctrl_stop(void);
void drive_ctrl_enable_PWM(void);
void drive_ctrl_disable_PWM(void);
void drive_ctrl_PWM(void);
void drive_ctrl_set_PWM(const uint8_t pwm);
#endif /* DRIVE_CONTROL_GUARD */




