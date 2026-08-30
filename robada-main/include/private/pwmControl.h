#ifndef PWM_PRV_H
#define PWM_PRV_H

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


typedef struct PwmMotor
{
    // We never expose this type to any users of pwm.
    mcpwm_oper_handle_t pwm_operator;
    mcpwm_cmpr_handle_t pwm_comparator;
    mcpwm_gen_handle_t pwm_gen_clockwise;
    mcpwm_gen_handle_t pwm_gen_counterclockwise;

    SemaphoreHandle_t owner_mutex; // mutex to keep track of which task owns the motor (and is allowed to set the speed of it)
    SemaphoreHandle_t read_write_mutex; // mutual exclusion when reading/writing speed. Non-owners can read the speed of the motor, but not write it.

    float speed; // From -1 to 1. [-1, 0) is counterclockwise, 0 is stopped, and (0, 1] is clockwise.
} PwmMotor;


// Private function to initialize a single motor.
void PWM_setup_motor(const MotorConfig* config, PwmMotor* motor);

#endif