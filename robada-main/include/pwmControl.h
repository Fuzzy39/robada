#ifndef PWM_H
#define PWM_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "esp_log.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_fault.h"
#include "hal/mcpwm_types.h"
#include "driver/gpio.h"

#include "defines.h"



/// @brief Initialize PWM motor control.
/// @param motorEnable the gpio_num_t of the pin connected to the motor controller enable lines.
/// @param motorPinouts An array of MotorPinout structs. 
/// @param numMotors The number of elements in motorPinouts. Should be no greater than 3.
void PWM_initialize(gpio_num_t motorEnable, const MotorConfig* motors, size_t numMotors);


// Claim this motor to be controlled by this task. 
// If shouldBlock is true, blocks until this motor has been released and returns true.
// If false, will return false if the motor is currently claimed by another task, otherwise claims the motor and returns true.
bool PWM_claim_motor(pwm_motor_handle_t motor, bool shouldBlock); 

// Release a motor from control by this task. 
void PWM_release_motor(pwm_motor_handle_t motor, bool stopMotor);

// Set motor speed, where [-1, 0) is counterclockwise, 0 is stopped, and (0, 1] is clockwise.
// Returns false and has no effect if the motor has not been claimed.
void PWM_set_motor_speed(pwm_motor_handle_t motor, float speed);

// Get the current speed that a motor has been set to (not measured). [-1, 0) is counterclockwise, 0 is stopped, and (0, 1] is clockwise.
float PWM_get_motor_speed(pwm_motor_handle_t motor);


#endif