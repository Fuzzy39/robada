#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/projdefs.h"

#include "driver/gpio.h"


// We want to do some freertos stuff!
// I'd reccomend looking through this as a primer:
// https://www.freertos.org/Documentation/01-FreeRTOS-quick-start/01-Beginners-guide/01-RTOS-fundamentals

// I'll more or less replicate the blinky example from here. We're blinking an led in an overcomplicated way.
// https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS/Demo/AVR_ATMega4809_Atmel_Studio/RTOSDemo/main_blinky.c

// The idea is that we have two tasks and a queue. 
// One task puts an int int the queue and delays for a given time.
// the other task blocks until the queue has an item, checks if the int is a certain value, 
// and blinks the led if it is.

// sounds simple enough to start out.

// you might notice that example is for a completely different processor than the esp, that's because the esp
// dosen't actually have any freertos examples, at least not from freeRTOS.
// This is because esp idf takes freeRTOS and changes it to work on two cores, so you can, you know, use all the
// processors in the device. not sure 100% how that works, though.

// Overview:   https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/freertos.html
// Actually, this basically explains in detail how it works: 
// https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-guides/freertos-smp.html


// I think next I intend to add another command to blink another led, tie it to a button press. that's why the enum is there.
enum QueueCommand
{
    blinkTimerLed,
    blinkButtonLed
};

// the pin the led should be attached to.
static const gpio_num_t TIMER_LED_PIN = GPIO_NUM_13;
static const gpio_num_t BUTTON_LED_PIN = GPIO_NUM_14;
static const gpio_num_t BUTTON_PIN = GPIO_NUM_27;


// Queue is a horrible word, typographically speaking.
// those letters, in that order? upsetting.
static QueueHandle_t Queue;
// the example I'm looking at declares this as static, which I suppose is fine.
// it's probably good practice to declare any top level variables as static unless
// you need them in the global scope... which you probably want to avoid usually.


// Function pre-declarations, since we're not using a header.
// not sure what the prv is supposed to mean, but that's what the examples do.
// update: it means private: these functions are static.
// idk if we should replicate that in actual code or not.
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

// task priorities bigger is more important.
static const UBaseType_t RECEIVE_PRIORITY = 2;
static const UBaseType_t SEND_PRIORITY = 1;

// how often we toggle the led.
static const UBaseType_t QUEUE_SEND_FREQUENCY_MS =  250 / portTICK_PERIOD_MS;
// Debouncing ignore timeframe.
const static UBaseType_t MS_TO_IGNORE = 50 / portTICK_PERIOD_MS;

// this is our interrupt handler. Later we register this handler with
// a changing edge of our button pin, so this will happen on a button press.
// because a physical button bounces on its contacts, multiple edges will happen
// on button press and release. This means we need to lock out after hearing an edge for some time.
// the level of the gpio input on this first edge will tell us if this is 
// a button press or release. We only want to change the led when the button is depressed initially.

// What does IRAM_ATTR mean? I'm not super sure.
// I need to do more research into the memory map and onboard memory on the esp32, but
// here's what I understand right now:
// There are, broadly, two ways a computer archecture can be constructed:
// Von Nuemann, where there is only one memory space that holds data and code
// and Harvard, where code and data are in two seperate address spaces.
// Most computers I've worked with are Von Nuemann, but the Xtensa cpus on the esp32, according to the docs,
// is Harvard. Kinda, at least. This makes a certain amount of sense for a microcontroller I guess.
// Anyways, it says that, but it has only one memory map (the memory map is the same between both cores btw)
// I'm not really clear on all of the memory that's on the esp, but some of it is IRAM
// I'm pretty sure that means 'Instruction Ram'.
// It's apparently important that instruction handlers live in IRAM, possibly for speed reasons.
// There might be a different or more important reason though, no idea really.
// need to look into the memory on the esp more.
static void IRAM_ATTR gpio_isr_handler(void* arg)
{

    static TickType_t lastInterruptTick = 0;

    TickType_t now = xTaskGetTickCountFromISR();
    if(lastInterruptTick+MS_TO_IGNORE > now)
    {
        // we are inside of the window where we ought to ignore new edges.
        return;
    }

    // time to do the buisness.
    lastInterruptTick = now;

 

    // send the command to blink the led. 
    // Note: printf (or logging, presumably) should not be used in an interrupt handler.
    // printing is a very complex and expensive operation, and interrupt handlers need to be fast.
    // it also uses a lot of stack memory. I have no idea what task's context we end up using for the stack
    // in an interrupt handler but probably best not to do anything super complicated.
    // Also printf might start a new task for UART comms. Doing that from inside an interrupt would probably not work very well.
    uint32_t gpio_num = blinkButtonLed;
    xQueueSendFromISR(Queue, &gpio_num, NULL);
}



void app_main(void)
{
    printf("Hello! this is rtos-test! Main running on core %d.\n", xPortGetCoreID());
    // I should note, apparently there's some sort of logging api. In the future we should use that.
    // it apparently works off of stdio which by default works off of uart.
    // I imagine in the future we should setup a bluetooth stdio, so we can use the logging api and 
    // get the debug info over the air, either in addition to uart or instead of with a single line of code or menuconfig or something.
    // Also, note, the esp led blink example has a menuconfig option, we can look at that to see how you set it up
    // https://github.com/espressif/esp-idf/tree/v6.0.2/examples/get-started/blink


    // Now, an important deviation from 'vanilla' freeRTOS. The implementation
    // used by esp idf calls vTaskStartScheduler before we get to app main (in the bootloader, I guess?)
    // so app_main is actually its own task. When this function returns it exits, which should be okay.
    // that means we just do our setup here and life should be good.


    Queue = xQueueCreate(
        4,                  // Max number of items in the queue. only one item, since we're not actually using this much like a queue.
                            // If you're being good, this should be a constant or define, and not a magic number.
        sizeof(uint32_t)    // size of queue items. We're storing ints.
                            // Queue items will be directly copied into the Queue's associated memory, which will be on the heap.
                            // esp idf provides its own heap implementation which freertos is configured to use.
                            // apparently we can call a static version of this function if we want to avoid using the heap. 
                            // I don't think there's any reason to do so, though.
        
    );

    if(Queue == NULL)
    {
        // As I understand, NULL is returned when the queue can't be created
        // which is probably when the heap functions can't allocate the memory.
        // should never happen in this example, but it's definitely a good idea to check.
        printf("Couldn't create queue.\n");
        return;
    }

    // now we can create our tasks.

    // note that this task could run on any core, as I understand it.
    // there is a function 'create task pinned to core' to force a task on one
    // core or the other.
    BaseType_t errorCode = xTaskCreate(
        prvQueueReceiveTask,    // function pointer to task.
        "receive",              // Task Name, for debugging I guess. max 16 characters.

        4096,                   // the size of the stack (in bytes, not words), for this task.
                                // I am not sure what happens if there's a stack overflow.
                                // could be you start corrupting memory. there's some way
                                // freeRTOS can detect it it seems.
                                // maybe idf implements the hooks for that and yells at us? hopefully.
                                // https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-Memory-management/02-Stack-usage-and-stack-overflow-checking
        NULL,                   // a pointer to pass parameters to the task. We aren't using it here.
        RECEIVE_PRIORITY,       // task priority. bigger number means more important, I think.
        NULL                    // Pass the address of a variable of type TaskHandle_t to get a hold of the task.

    );

    if(errorCode != pdPASS)
    {
        printf("Couldn't create receive task. Error Code %d.\n", errorCode);
        // if we weren't completely aborting we might want to delete the queue here.
        return;
    }

    // make the transmit task similarly.

    if( xTaskCreate(prvQueueSendTask, "Send", 4096, NULL, SEND_PRIORITY, NULL) != pdPASS)
    {
        printf("Couldn't create receive task. Error Code %d.\n", errorCode);
        // if we weren't completely aborting we might want to delete the queue here.
        // and I guess the other task as well.
        return;
    }

    // Finally, configure our button.
    // Note, there is another function which can set all of these things at once,
    // you fill a struct with all of your options.
    // it seems more useful for setting several inputs at once, though.
    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLDOWN_ONLY);

    // setup the interupt.

    // not sure if these are the flags we want. the example didn't set any but that seems wrong?
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED|ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, NULL);
    // gpio_isr_register also exists, which can be used to setup an interrupt 
    // for all gpio interrupts instead of one for a specific pin.

    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_ANYEDGE);
    gpio_intr_enable(BUTTON_PIN);

    printf("Started okay! Main exiting.\n");

}


static void prvQueueReceiveTask( void *pvParameters )
{
    // the core can change over the course of execution but I'm curious.
    printf("receive started on core %d!\n", xPortGetCoreID());
    // This print straight up doesn't happen??
    // no understanding there. I tried flushing, doesn't help.
    
    // initialize the led to off.
    gpio_reset_pin(TIMER_LED_PIN);
    gpio_set_direction(TIMER_LED_PIN, GPIO_MODE_OUTPUT);

    bool blinkLedState = false;
    gpio_set_level(TIMER_LED_PIN, blinkLedState);

    gpio_reset_pin(BUTTON_LED_PIN);
    gpio_set_direction(BUTTON_LED_PIN, GPIO_MODE_OUTPUT);

    bool buttonLedState = false;
    gpio_set_level(BUTTON_LED_PIN, buttonLedState);

    while(true)
    {
        // the main loop of this task: we get a command from the queue, we do something with it.

        uint32_t command;
        if( xQueueReceive(
                Queue,
                &command,       // Pointer to memory to fill. needs to be of the correct size.
                portMAX_DELAY   // number of ticks to wait (by default for esp they are 10ms apart)
                                // setting to portMAX_DELAY should mean we block until there's something
                                // in the queue. 
                )
            == errQUEUE_EMPTY)
        {
            // this shouldn't happen, but if freeRTOS is configured with vTaskSuspend == 0 it could.
            // idk if IDF forces it to be that value or if it's configurable still. Could probably skip this check 
            // in real code.
            printf("Queue Empty?\n");
            continue;
        }

        // if the queue contains the id for the blinkTimerLed command, then we toggle the led.
        switch(command)
        {
            case blinkTimerLed:
                printf("Blink!\n");
                blinkLedState = !blinkLedState;
                gpio_set_level(TIMER_LED_PIN, blinkLedState);
                break;
            case blinkButtonLed:
                printf("Button Blink!\n");  

                // our interrupt handler catches the first 'bounce' and we need to wait until its stable.
                // I think that doing a software only debounce kinda sucks tbh.
                // although maybe I am just doing this in a silly way.

                vTaskDelay(MS_TO_IGNORE);

                // only respond to a rising edge.
                if(!gpio_get_level(BUTTON_PIN)) break;

                buttonLedState = !buttonLedState;
                gpio_set_level(BUTTON_LED_PIN, buttonLedState);
                break;
            default:
                printf("Got unknown command id: %ld.\n", command);
        }

       
    }
}

static void prvQueueSendTask( void *pvParameters )
{
    // the core can change over the course of execution but I'm curious.
    printf("Send started on core %d!\n", xPortGetCoreID());
    fflush(stdout); // the print statement doesn't print. No idea why.

    // So, every 10ms by default (configurable in the freeRtos settings in menuconfig)
    // there's a scheduler tick. This returns the number of ticks since the scheduler started.
    // we'll be using this to time when we should blink from.
    TickType_t lastWakeTime = xTaskGetTickCount();
    // as an aside, I have no idea how this scheduler tick is accomplished. I assume an interrupt, but how?
    // the watchdog? the cpu has an internal timer maybe?
    // apparently each cpu's ticks aren't syncronized, so that's a clue.

    while(true)
    {

        // block until the time has elapsed.
        // you could do this with vTaskDelay, but this function will
        // ensure that the correct amount of time has passed.
        // (current - benchmark) == Queue_send_frequency, basically.
        // as opposed to just delaying for Queue_send_frequency. That way this will
        // account for the time from begining to execute until delaying again.

        xTaskDelayUntil( &lastWakeTime, QUEUE_SEND_FREQUENCY_MS );
        // this function returns true if it skipped a period, but I don't think there's anything
        // we'd want to do in that case.
        // I guess provide child support but we're not doing that this time.
        
        // put a value in the queue.
        uint32_t toSend = blinkTimerLed;

        if( xQueueSendToBack(
                Queue,
                &toSend,        // Pointer to memory to fill. needs to be of the correct size.
                portMAX_DELAY   // number of ticks to wait (by default for esp they are 10ms apart)
                                // setting to portMAX_DELAY should mean we block until there's something
                                // in the queue. 
                )
            == errQUEUE_EMPTY)
        {
            // this shouldn't happen, but if freeRTOS is configured with vTaskSuspend == 0 it could.
            // idk if IDF forces it to be that value or if it's configurable still. Could probably skip this check 
            // in real code.
            printf("Queue Full?\n");
            continue;
        }

        

    }

}