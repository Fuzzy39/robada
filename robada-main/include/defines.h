#ifndef DEFINES_H
#define DEFINES_H

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "driver/gpio.h"


// an include gaurd prevents a header from being included multiple times
// in a single compilation unit (c file), which prevents the same function
// being declared twice. However, if you define a function or variable
// this definition will happen multiple times in seperate compilation units.
// This is not a problem when declaring, but defining the same thing multiple
// times results in a linker error. Therefore, headers 
// should only contain declarations, not definitions. You can get around this
// by statically declaring everything (making it local to a single compilation unit)
// but this results in copies of the same constants every time they're used.
// not wise, especially if we want to save on program space.

// You could also use defines I suppose...


// User Interface pins

extern const gpio_num_t MOTOR_SELECT_LED_PIN;
extern const gpio_num_t MOTOR_SELECT_BUTTON_PIN;        
extern const gpio_num_t MOTOR_CLOCKWISE_BUTTON_PIN; 
extern const gpio_num_t MOTOR_COUNTERCLOCKWISE_BUTTON_PIN;

// Motor stuff
typedef unsigned int pwm_motor_handle_t;

typedef struct MotorConfig
{
    pwm_motor_handle_t motor;
    int clockwiseGpioNum;
    int counterclockwiseGpioNum;

} MotorConfig;


extern const pwm_motor_handle_t BASE_MOTOR;
extern const pwm_motor_handle_t SHOULDER_MOTOR;
extern const pwm_motor_handle_t ELBOW_MOTOR;

extern const MotorConfig motorConfigs[];
extern const size_t numMotors;

// task priorities

// I was too lazy to make this one extern as well. If we set up actual priorities later I will.
static const UBaseType_t DEFAULT_PRIORITY = 1;

#endif
