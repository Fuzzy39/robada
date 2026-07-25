#include "main.h"
static const char* LOG_TAG = "main";

void app_main(void)
{
    // Okay, so, what does main need to do?
    // Register all the buttons, setup io for outputs
    // say hi!
    ESP_LOGI(LOG_TAG, "Robada init!\n");

    // create the command queue. 
    commandQueue = xQueueCreate(4, sizeof(uint32_t));
    if(commandQueue == NULL)
    {
        ESP_LOGE(LOG_TAG, "Couldn't create command queue.\n");
    }

    // Setup buttons
    register_gpio_button(MOTOR_SELECT_BUTTON_PIN , commandQueue, 0);

    if(!initialize_debounce_task())
    {
        return;
    }

    // Start our two tasks.
    if(xTaskCreate(main_task, "robada main", 4096, NULL, DEFAULT_PRIORITY, NULL ) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Couldn't create main task. Error Code %d.\n", errorCode);
        return;
    }


    ESP_LOGI(LOG_TAG, "Initialization Complete.\n");
}

void main_task(void* args)
{
    // some initialization: Motor pins and the led we want to use.
    gpio_reset_pin(MOTOR_SELECT_LED_PIN);
    gpio_set_direction(MOTOR_SELECT_LED_PIN, GPIO_MODE_OUTPUT);

    bool blinkLedState = false;
    gpio_set_level(MOTOR_SELECT_LED_PIN, blinkLedState);
    
    while(true)
    {
       
        uint32_t command;
        
        // block until something's in the queue.
        xQueueReceive(commandQueue, &command, portMAX_DELAY);

        blinkLedState = !blinkLedState;
        gpio_set_level(MOTOR_SELECT_LED_PIN, blinkLedState);
        
    }
          
}