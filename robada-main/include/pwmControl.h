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
#include "hal/mcpwm_types.h"


const uint32_t PWM_TICK_HZ = 10000000; // 10 MHz
const uint32_t PWM_PERIOD_TICKS = 1000; // 10 khz 
const uint32_t PWM_PERIOD_HZ = PWM_TICK_HZ/PWM_PERIOD_TICKS;

typedef unsigned int pwm_motor_handle_t;

typedef struct PwmMotor
{
    // We never expose this type to any users of pwm.
    mcpwm_oper_handle_t pwm_operator;
    mcpwm_cmpr_handle_t pwm_comparator;
    mcpwm_gen_handle_t pwm_gen_clockwise;
    mcpwm_gen_handle_t pwm_gen_counterclockwise;

    SemaphoreHandle_t owner_semaphore; // Binary sempaphore to keep track of which task owns the motor (and is allowed to set the speed of it)
    SemaphoreHandle_t read_write_mutex; // mutual exclusion when reading/writing speed. Non-owners can read the speed of the motor, but not write it.

    int32_t speed; // From PWM_SPEED_MIN to PWM_SPEED_MAX
} PwmMotor;


typedef struct MotorConfig
{
    pwm_motor_handle_t motor;
    int clockwiseGpioNum;
    int counterclockwiseGpioNum;

} MotorConfig;


/// @brief Initialize PWM motor control.
/// @param motorEnable the gpio_num_t of the pin connected to the motor controller enable lines.
/// @param motorPinouts An array of MotorPinout structs. 
/// @param numMotors The number of elements in motorPinouts. Should be no greater than 3.
void PWM_initialize(gpio_num_t motorEnable, MotorConfig* motors, size_t numMotors);

// Private function to initialize a single motor.
void PWM_setup_motor(const MotorConfig* config, PwmMotor* motor);

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