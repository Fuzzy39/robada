#include "defines.h"

const gpio_num_t MOTOR_SELECT_LED_PIN               = GPIO_NUM_12;
const gpio_num_t MOTOR_SELECT_BUTTON_PIN            = GPIO_NUM_13;
const gpio_num_t MOTOR_CLOCKWISE_BUTTON_PIN         = GPIO_NUM_14;
const gpio_num_t MOTOR_COUNTERCLOCKWISE_BUTTON_PIN  = GPIO_NUM_27;

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
};

const size_t numMotors = 2;
