#include "defines.h"
#include "freertos/task.h"
#include "main.h"
#include "buttonDebounce.h"
#include "pwmControl.h"


static const char* LOG_TAG = "main";
QueueHandle_t commandQueue;

enum IOCommand
{
    buttonSwitchMotor,
    buttonClockwise,
    buttonCounterclockwise
};

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
    register_gpio_button(MOTOR_SELECT_BUTTON_PIN , commandQueue, buttonSwitchMotor);
    register_gpio_button(MOTOR_CLOCKWISE_BUTTON_PIN, commandQueue, buttonClockwise);
    register_gpio_button(MOTOR_COUNTERCLOCKWISE_BUTTON_PIN, commandQueue, buttonCounterclockwise);

    PWM_initialize(0, motorConfigs, sizeof(motorConfigs)/sizeof(MotorConfig));


    if(!initialize_debounce_task())
    {
        return;
    }



    // Start our two tasks.
    if(xTaskCreate(main_task, "robada main", 4096, NULL, DEFAULT_PRIORITY, NULL ) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Couldn't create main task.\n");
        return;
    }


    ESP_LOGI(LOG_TAG, "Initialization Complete.\n");
}


void main_task(void* args)
{
    bool currentMotor = false; // false for M1, true for M2.

    // register the motors with us.
    if(!PWM_claim_motor(BASE_MOTOR, false))
    {
        ESP_LOGE(LOG_TAG, "Couldn't claim base motor.\n");
    }
    if(!PWM_claim_motor(SHOULDER_MOTOR, false))
    {
        ESP_LOGE(LOG_TAG, "Couldn't claim shoulder motor.\n");
    }

    gpio_reset_pin(MOTOR_SELECT_LED_PIN);
    gpio_set_direction(MOTOR_SELECT_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MOTOR_SELECT_LED_PIN, currentMotor);

    
    while(true)
    {
       
        uint32_t command;

        // block until something's in the queue.
        xQueueReceive(commandQueue, &command, portMAX_DELAY);
        ESP_LOGI(LOG_TAG, "Got Command %d.\n", command);    
        switch(command)
        {
            case buttonSwitchMotor:
                currentMotor = !currentMotor;
                ESP_LOGI(LOG_TAG,"Setting led to %d.\n", currentMotor);
                gpio_set_level(MOTOR_SELECT_LED_PIN, currentMotor);
                break;
            case buttonClockwise:
                if(currentMotor)
                {
                    changeSpeed(SHOULDER_MOTOR, true);
                    break;
                }
                changeSpeed(BASE_MOTOR, true);
                break;
            case buttonCounterclockwise:
                if(currentMotor)
                {
                    changeSpeed(SHOULDER_MOTOR, false);
                    break;
                }
                changeSpeed(BASE_MOTOR, false);
                break;
            
        }
        
    }
          
}


void changeSpeed(pwm_motor_handle_t motor, bool clockwise)
{
    float prev = PWM_get_motor_speed(motor);
    float addTo = .2f*((int)clockwise*2-1);
    ESP_LOGI(LOG_TAG,"Setting motor %d to speed %f.\n", motor, prev+addTo);

    PWM_set_motor_speed(motor, prev+addTo); // the set speed function caps our speed to the max/min so we should be good.

}
