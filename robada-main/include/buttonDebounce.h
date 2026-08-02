#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "driver/gpio.h"
#include "esp_log.h"


// button debouncing
// ------------
static const UBaseType_t MS_DEBOUNCE = 50 / portTICK_PERIOD_MS;

void IRAM_ATTR gpio_button_isr_handler(void* arg); // arguably shouldn't be in the header?
bool initialize_debounce_task(void); // returns whether an error occured.
bool register_gpio_button(gpio_num_t buttonPin, QueueHandle_t queue, uint32_t command);
void gpio_button_task(void* args);

#endif