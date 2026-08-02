#ifndef DEFINES_H
#define DEFINES_H

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "driver/gpio.h"

// io definitions

// static in a header results in different copies of the same variable for any file that includes the header. these are const, so it should be fine.

// User Interface pins
static const gpio_num_t MOTOR_SELECT_LED_PIN               = GPIO_NUM_12;
static const gpio_num_t MOTOR_SELECT_BUTTON_PIN            = GPIO_NUM_13;
static const gpio_num_t MOTOR_CLOCKWISE_BUTTON_PIN         = GPIO_NUM_14;
static const gpio_num_t MOTOR_COUNTERCLOCKWISE_BUTTON_PIN  = GPIO_NUM_27;

// Motor pins
typedef struct MotorPinout
{
    gpio_num_t clockwisePin;
    gpio_num_t counterclockwisePin;
} MotorPinout;

static const MotorPinout M1_PINOUT = {
     .clockwisePin = GPIO_NUM_26, 
     .counterclockwisePin = GPIO_NUM_25  
};

static const MotorPinout M2_PINOUT = { 
    .clockwisePin = GPIO_NUM_33, 
    .counterclockwisePin = GPIO_NUM_32 
};


// task priorities
static const UBaseType_t DEFAULT_PRIORITY = 1;
#endif
