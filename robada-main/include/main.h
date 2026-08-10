#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "defines.h"
#include "pwmControl.h"
#include "buttonDebounce.h"





// main stuff
// ------------
extern QueueHandle_t commandQueue;

void app_main(void);

// app_main is already a task so this isn't strictly needed. Oh well, I felt like it.
void main_task(void* args);
void changeSpeed(pwm_motor_handle_t motor, bool clockwise);

#endif