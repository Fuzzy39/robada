#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "driver/gpio.h"

// io definitions

// static in a header results in different copies of the same variable for any file that includes the header. these are const, so it should be fine.

// User Interface pins
static const gpio_num_t MOTOR_SELECT_LED_PIN               = GPIO_NUM_13;
static const gpio_num_t MOTOR_SELECT_BUTTON_PIN            = GPIO_NUM_13;
static const gpio_num_t MOTOR_CLOCKWISE_BUTTON_PIN         = GPIO_NUM_13;
static const gpio_num_t MOTOR_COUNTERCLOCKWISE_BUTTON_PIN  = GPIO_NUM_13;

// Motor pins
typedef struct MotorPinout = 
{
    gpio_num_t clockwisePin;
    gpio_num_t counterclockwisePin;
};

static const MotorPinout M1_PINOUT = {
     .clockwisePin = GPIO_NUM_13, 
     .counterclockwisePin = GPIO_NUM_13 
};

static const MotorPinout M2_PINOUT = { 
    .clockwisePin = GPIO_NUM_13, 
    .counterclockwisePin = GPIO_NUM_13 
};


// task priorities
static const UBaseType_t DEFAULT_PRIORITY = 1;


// main stuff
// ------------
QueueHandle_t commandQueue;

void app_main(void);

// app_main is already a task so this isn't strictly needed. Oh well, I felt like it.
void main_task(void* args);

// button debouncing
// ------------

static const UBaseType_t MS_DEBOUNCE = 50 / portTICK_PERIOD_MS;

void IRAM_ATTR gpio_button_isr_handler(void* arg); // arguably shouldn't be in the header?
bool initialize_debounce_task(void); // returns whether an error occured.
bool register_gpio_button(gpio_num_t buttonPin, QueueHandle_t queue, uint32_t command);
void gpio_button_task(void* args);

#endif