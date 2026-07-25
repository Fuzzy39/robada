#include "main.h"
# define BUTTON_LIMIT 10
static const char* LOG_TAG = "debounce";

typedef struct ButtonInfo{
    gpio_num_t pin;             // gpio pin button is tied to
    QueueHandle_t outputQueue;  // Queue to output if triggered.
    uint32_t output;            // What to output in output queue
    TickType_t lastInterrupted; // tick count when this button last (successfully) triggered an interrupt.
};

static ButtonInfo registeredButtons[BUTTON_LIMIT];
static size_t numButtons = 0;

static QueueHandle_t debounceQueue;
static bool initialized = false;


void IRAM_ATTR gpio_button_isr_handler(void* arg)
{
    ButtonInfo* button = (ButtonInfo)arg;

    TickType_t now = xTaskGetTickCountFromISR();
    if(button->lastInterrupted + MS_DEBOUNCE > now)
    {
        // This button is bouncing. Ignore it.
        return;
    }

    // update the button's last interrupt to now.
    button->lastInterrupted = now;

    // and send that we detected an edge.
    if(xQueueSendFromISR(debounce, &(button->pin), NULL)==errQUEUE_FULL)
    {
        ESP_DRAM_LOGW(LOG_TAG, "Debounce queue full. dropping input from pin %llu.", button->pin);
    }
    
}

bool initialize_debounce_task(void)
{
    initialized = true; // We're doing this in the same task that registers the buttons (app_main)
                        // this means no race conditions to worry about.

    // create the queue
    debounceQueue = xQueueCreate(6, sizeof(gpio_num_t));
    if(debounceQueue == NULL)
    {
        ESP_LOGE(LOG_TAG, "Couldn't create debounce queue.\n");
        return false;
    }
            
    // register button pins. We'll do a bulk registering this time.
    gpio_config_t buttonConf = {};
    buttonConf.intr_type = GPIO_INTR_ANYEDGE;
    buttonConf.mode = GPIO_INPUT;
    buttonConf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    buttonConf.pull_up_en = GPIO_PULLUP_DISABLE;
    buttonConf.pin_bit_mask = 0;

    // setup driver(?) for gpio individual handlers 
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED|ESP_INTR_FLAG_EDGE);

    for(int i = 0; i<numButtons; i++)
    {
        gpio_num_t pin = registeredButtons[i].pin;
        buttonConf.pin_bit_mask |= 1ULL << pin; // the ULL is there because the example did it. I'd think you don't need it but idk.

        //configure interrupt handler...
        gpio_isr_handler_add(pin, gpio_button_isr_handler, &registeredButtons[i]);
    }

    // actually configure gpio for all buttons
    gpio_config(&buttonConf);

    // finally, make the task.
    if(xTaskCreate(gpio_button_task, "debounce", 4096, NULL, DEFAULT_PRIORITY, NULL) != pdPASS)
    {
        ESP_LOGE(LOG_TAG, "Couldn't create debounce task. Error Code %d.\n", errorCode);
        return false;
    }

    return true;
}

// DO NOT CALL once initialize_debounce_task has been called.
bool register_gpio_button(gpio_num_t buttonPin, QueueHandle_t queue, uint32_t command)
{
    if(initialized)
    {
        ESP_LOGW(LOG_TAG, "Button may not be registered after debounce task started. Please initialize in app_main.\n");
        return false;
    }

    if(num_buttons => BUTTON_LIMIT)
    {
        ESP_LOGW(LOG_TAG, "Can't register more than %d buttons. Pin %d ignored.", BUTTON_LIMIT, button_pin);
        return false;
    }

    registeredButtons[numButtons] = {
        .pin = buttonPin, 
        .outputQueue = queue, 
        .output = command,
        .lastInterrupted = xTaskGetTickCount()
    };
    numButtons++;
    return true;
}

void gpio_button_task(void* args)
{
    while(true)
    {
        gpio_num_t pin;
        // block until something's in the queue.
        xQueueReceive( debounceQueue, &pin, portMAX_DELAY);
        
        ButtonInfo* button = NULL;
        for(int i = 0; i<numButtons; i++)
        {
            if(registeredButtons[i].pin == pin)
            {
                button = registeredButtons[i];
                break;
            }
        }

        if(button == NULL)
        {
            // I don't think this will give a meaningful output on which pin this actually was, but oh well.
            ESP_LOGW(LOG_TAG, "Invalid button %llu in debounce queue. Ignoring.", pin);
            continue;
        }

        // if the button is low after a small time, this was a falling edge. discard it.
        vTaskDelay(MS_DEBOUNCE);
        if(!gpio_get_level(button->pin)) continue;

        if(xQueueSendToBack(button->outputQueue, &(button->output), 0)==errQUEUE_FULL)
        {
            // if the recieving queue is full, somebody's dropping the input.
            ESP_LOGW(LOG_TAG, " Button input on pin %llu was dropped. Receiving queue full.", pin);
        }
    }
}