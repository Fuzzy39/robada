#ifndef MAIN_H
#define MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "driver/gpio.h"
#include "esp_log.h"


// main stuff
// ------------
extern QueueHandle_t commandQueue;

void app_main(void);

// app_main is already a task so this isn't strictly needed. Oh well, I felt like it.
void main_task(void* args);
void init_motor(MotorPinout motor);
void drive_motor(MotorPinout motor, int motorSpeed); // very primitive.
int changeSpeed(int prev, bool clockwise);


#endif