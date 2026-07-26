#include "main.h"
static const char* LOG_TAG = "main";

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
    bool currentMotor = false; // false for M1, true for M2.
    int M1Speed = 0; // -1, 0, 1.
    int M2Speed = 0;

    // some initialization: Motor pins and the led we want to use.
    init_motor(M1_PINOUT);
    init_motor(M2_PINOUT);

    gpio_reset_pin(MOTOR_SELECT_LED_PIN);
    gpio_set_direction(MOTOR_SELECT_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MOTOR_SELECT_LED_PIN, currentMotor);

    
    while(true)
    {
       
        uint32_t command;

        // block until something's in the queue.
        xQueueReceive(commandQueue, &command, portMAX_DELAY);
        switch(command)
        {
            case buttonSwitchMotor:
                currentMotor = !currentMotor;
                gpio_set_level(MOTOR_SELECT_LED_PIN, currentMotor);
                break;
            case buttonClockwise:
                if(currentMotor)
                {
                    M2Speed = changeSpeed(M2Speed, true);
                    drive_motor(M2_PINOUT, M2Speed);
                    break;
                }
                M1Speed = changeSpeed(M1Speed, true);
                drive_motor(M1_PINOUT, M1Speed);
            case buttonCounterclockwise:
                // yes, this code is bad, but it's extremely temporary so It'll do for now.
                if(currentMotor)
                {
                    M2Speed = changeSpeed(M2Speed, false);
                    drive_motor(M2_PINOUT, M2Speed);
                    break;
                }
                M1Speed = changeSpeed(M1Speed, false);
                drive_motor(M1_PINOUT, M1Speed);
            
        }
        
    }
          
}

// temp code, almost certainly.
void init_motor(MotorPinout motor)
{
    gpio_reset_pin(motor.clockwisePin);
    gpio_set_direction(motor.clockwisePin, GPIO_MODE_OUTPUT);
    gpio_reset_pin(motor.counterclockwisePin);
    gpio_set_direction(motor.counterclockwisePin, GPIO_MODE_OUTPUT);

    drive_motor(motor, 0);
}

// also definitely temp, as soon as we get pwm this is gone.
void drive_motor(MotorPinout motor, int motorSpeed)
{
   switch (motorSpeed)
   {
    case -1:
        gpio_set_level(motor.clockwisePin,        false);
        gpio_set_level(motor.counterclockwisePin, true);
        break;
    case 1:
        gpio_set_level(motor.clockwisePin,        true);
        gpio_set_level(motor.counterclockwisePin, false);
        break;
    default:
        gpio_set_level(motor.clockwisePin,        false);
        gpio_set_level(motor.counterclockwisePin, false);
        break;
   }
}

// also definitely temp code. and bad code, to boot.
int changeSpeed(int prev, bool clockwise)
{
    int toAdd = 1*(clockwise?1:-1);
    prev+=toAdd;
    prev = prev>1?1:prev;
    prev = prev<-1?-1:prev;
    return prev;
}
