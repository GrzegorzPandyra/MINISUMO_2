#ifndef DRIVE_CONTROL_GUARD
#define DRIVE_CONTROL_GUARD
/*! @file drive_ctrl.h
    @brief API to control the movement of the robot
*/
void drive_ctrl_init();
void drive_ctrl_go_forward();
void drive_ctrl_go_backward();
void drive_ctrl_turn_right();
void drive_ctrl_turn_left();
void drive_ctrl_stop();
void drive_ctrl_enable_PWM();
void drive_ctrl_disable_PWM();
#endif /* DRIVE_CONTROL_GUARD */




