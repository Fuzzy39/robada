#ifndef DEFINES_H
#define DEFINES_H

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "driver/gpio.h"

#include "pwmControl.h"


// io definitions

// static in a header results in different copies of the same variable for any file that includes the header. these are const, so it should be fine.

// User Interface pins
static const gpio_num_t MOTOR_SELECT_LED_PIN               = GPIO_NUM_12;
static const gpio_num_t MOTOR_SELECT_BUTTON_PIN            = GPIO_NUM_13;
static const gpio_num_t MOTOR_CLOCKWISE_BUTTON_PIN         = GPIO_NUM_14;
static const gpio_num_t MOTOR_COUNTERCLOCKWISE_BUTTON_PIN  = GPIO_NUM_27;


const pwm_motor_handle_t BASE_MOTOR = 0;
const pwm_motor_handle_t SHOULDER_MOTOR = 1;
const pwm_motor_handle_t ELBOW_MOTOR = 2;

const MotorConfig motorConfigs[] =
{
    {
        .motor = BASE_MOTOR, 
        .clockwiseGpioNum = 26, 
        .counterclockwiseGpioNum = 25
    },
    {
        .motor = SHOULDER_MOTOR, 
        .clockwiseGpioNum = 33,
         .counterclockwiseGpioNum = 32
    }
}


// task priorities
static const UBaseType_t DEFAULT_PRIORITY = 1;
#endif
