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


// const uint32_t PWM_TICK_HZ = 10000000; // 10 MHz
// const uint32_t PWM_PERIOD_TICKS = 1000; // 10 khz 
// const uint32_t PWM_PERIOD_HZ = PWM_TICK_HZ/PWM_PERIOD_TICKS;

// const uint32_t PWM_SPEED_MAX = PWM_PERIOD_TICKS;
// const uint32_t PWM_SPEED_MIN = -PWM_SPEED_MAX;

// typedef struct Motor
// {
//     //uint32_t clockwise_pin_number;
//     //uint32_t counterclockwise_pin_number;
//     mcpwm_oper_handle_t pwm_operator;
//     mcpwm_gen_handle_t pwm_generator;
//     mcpwm_cmpr_handle_t pwm_comparator;

//     int32_t speed; // Defined as 
// } Motor;




void init();


#endif