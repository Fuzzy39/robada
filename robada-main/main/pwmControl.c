#include "pwmControl.h"
#include <limits.h>
#include <math.h>

static const char* LOG_TAG = "PWM";

// static const uint32_t TIMER_HZ = 10000000; // 10 MHz
// static const uint32_t PERIOD_TICKS = 1000; // 10    khz 


const uint32_t PWM_TICK_HZ = 10000000; // 10 MHz
const uint32_t PWM_PERIOD_TICKS = 1000; // 10 khz 
const uint32_t PWM_PERIOD_HZ = PWM_TICK_HZ/PWM_PERIOD_TICKS;

static mcpwm_timer_handle_t timer;

static PwmMotor motors[3];


void PWM_initialize(gpio_num_t motorEnable, const MotorConfig* configs, size_t numMotors)
{
    // start by setting up the timer.

    mcpwm_timer_config_t timerConfig = {
        .group_id = 0, // 0, 1, 2, We've got 3 of these guys on board.
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = PWM_TICK_HZ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = PWM_PERIOD_TICKS,
        .intr_priority = 0, // Default, low priority
        .flags = {
            .update_period_on_empty = true,
            .update_period_on_sync = false,
            .allow_pd = false, // We never go to sleep, so this is irrelevant.      
        }
    };

    ESP_ERROR_CHECK(mcpwm_new_timer(&timerConfig, &timer));
    // ESP_ERROR_CHECK checks for an error (duh) and prints out a message if there is one.
    // It'll also call 'abort()' which I assume resets the processor... looks like by default, yes.
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/fatal-errors.html

    // actually initialize the motors
    for(int i = 0; i<numMotors; i++)
    {   
        PWM_setup_motor(&configs[i], &motors[configs[i].motor]);
    }


    // finally, start the timer and enable the gpio enable

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // gpio_reset_pin(motorEnable);
    // gpio_set_direction(motorEnable, GPIO_MODE_OUTPUT);
    // gpio_set_level(motorEnable, true);  // this should only go low when the program aborts(???), which I've configured to halt the system/
    //                                     // TEST AND MAKE SURE THE IO GOES LOW WHEN abort() is called. This is a safety thing.

    ESP_LOGI(LOG_TAG, "Initialized PWM controller.\n");
}


void PWM_setup_motor(const MotorConfig* config, PwmMotor* motor)
{
    // a PwmMotor needs ....
    // An operator, comparator, 2 generators, and 2 semaphores.
    // Okay, lets get cracking, then.


    // operator first, since it contains everything.
    mcpwm_operator_config_t operatorConfig = {
        .group_id = 0, //config->motor,
        .intr_priority = 0
    };

    ESP_ERROR_CHECK(mcpwm_new_operator(&operatorConfig, &(motor->pwm_operator)));
    // connect the timer to the operator so we can get something done.
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(motor->pwm_operator, timer));

    // next, the comparator.
    mcpwm_comparator_config_t comparatorConfig = {
        .intr_priority = 0,
        .flags = {
            .update_cmp_on_tez = true,
            .update_cmp_on_tep = false,
            .update_cmp_on_sync = false
        }
    };

    ESP_ERROR_CHECK(mcpwm_new_comparator(motor->pwm_operator, &comparatorConfig, &(motor->pwm_comparator)));

    // We will set the compare value when we set the motor speed by the end of this.

    // Generator time!
    mcpwm_generator_config_t generatorConfig = {
        .gen_gpio_num = config->clockwiseGpioNum, // Motor one clockwise
        .flags = {
            .invert_pwm = false,
        },
    };

    ESP_ERROR_CHECK(mcpwm_new_generator(motor->pwm_operator, &generatorConfig, &(motor->pwm_gen_clockwise)));

    generatorConfig.gen_gpio_num = config->counterclockwiseGpioNum;
    ESP_ERROR_CHECK(mcpwm_new_generator(motor->pwm_operator, &generatorConfig, &(motor->pwm_gen_counterclockwise)));

    // generator events...
    mcpwm_gen_timer_event_action_t timerAction =
    {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_LOW
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(motor->pwm_gen_clockwise,  timerAction));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(motor->pwm_gen_counterclockwise,  timerAction));

    // And initialize our semaphore and mutex.
    
    // the owner semaphore is a binary sempaphore and not a mutex because semaphores don't have the 'priority inheritance mechanism'
    // that mutexes do. We don't want a higher priority task to hijack the motors from a lower priority one.
    // We want the controlling task to release the motor first.
    
    // We can stop the motors externally by triggering the software fault, which should work in cases where the controlling task isn't cooperating.
   
    motor->owner_mutex = xSemaphoreCreateMutex(); // Note that the calling task owns this semaphore when we create it (We have to release it later)
    if(motor->owner_mutex == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to create semaphore for motor %i.\n", config->motor);
        abort(); // bring down the whole program.
    }

    motor->read_write_mutex = xSemaphoreCreateMutex();
    if(motor->read_write_mutex == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to create mutex for motor %i.\n", config->motor);
        abort(); // bring down the whole program.
    }
  
    // Stop (thereby properly setting up the comparator and generators) and release the semaphore we have on the motor.
    PWM_claim_motor(config->motor, true);
    PWM_release_motor(config->motor, true);

}


bool PWM_claim_motor(pwm_motor_handle_t handle, bool shouldBlock)
{
    // Claiming the motor should be relatively easy.
    return xSemaphoreTake(motors[handle].owner_mutex, shouldBlock?portMAX_DELAY:0);
} 

static void assert_current_task_owns_motor(pwm_motor_handle_t handle)
{
    TaskStatus_t taskStatus;
    vTaskGetInfo( NULL, &taskStatus, pdTRUE, eRunning ); // null is to get the current task, pdTrue and eRunning are just to avoid
                                                         // having the function figure out remaining stack space and the task's state,
                                                         // which are expensive operations.
    TaskHandle_t currentTask = taskStatus.xHandle;
    TaskHandle_t motorOwner = xSemaphoreGetMutexHolder(motors[handle].owner_mutex);

    if(motorOwner != currentTask)
    {
        // The task which controls the motor isn't us, so somebody made a mistake somewhere.
        TaskStatus_t ownerTaskStatus;
        vTaskGetInfo( motorOwner, &ownerTaskStatus, pdTRUE, eRunning );

        ESP_LOGE(LOG_TAG, "PWM_set_motor_speed: task '%s' can't set motor %i's speed because task '%s' controls this motor.\n",
            taskStatus.pcTaskName, handle, ownerTaskStatus.pcTaskName);
        abort();
    }
}


void PWM_release_motor(pwm_motor_handle_t handle, bool stopMotor)
{
    assert_current_task_owns_motor(handle);
    if(stopMotor) PWM_set_motor_speed(handle, 0);

    PwmMotor* m = &(motors[handle]);
    // Now we release the motor.
    if(!xSemaphoreGive(m->owner_mutex))
    {
        ESP_LOGE(LOG_TAG, "Failed to release semaphore for motor %i.\n", handle);
        abort();   
    }
}   



void PWM_set_motor_speed(pwm_motor_handle_t handle, float speed)
{
    // this will probably be the second most complicated part, aside from initialization.
    bool isClockwise = speed>0;
    // First, check that we have actually claimed the motor.
    assert_current_task_owns_motor(handle);

    // get the read_write_mutex before we actually do anything...
    if(!xSemaphoreTake(motors[handle].read_write_mutex, portMAX_DELAY))
    {
        ESP_LOGE(LOG_TAG, "PWM_get_motor_speed: Failed to take mutex for motor %i.\n", handle);
        abort();  
    }

    // Now we start doing the annoying stuff.

    mcpwm_gen_compare_event_action_t highAction 
        = MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, motors[handle].pwm_comparator, MCPWM_GEN_ACTION_HIGH);
    mcpwm_gen_compare_event_action_t lowAction 
        = MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, motors[handle].pwm_comparator, MCPWM_GEN_ACTION_LOW);

    mcpwm_gen_compare_event_action_t clockwiseAction = lowAction;
    mcpwm_gen_compare_event_action_t counterclockwiseAction = lowAction;

    if(speed>0) clockwiseAction = highAction;
    if(speed<0) counterclockwiseAction = highAction;

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(motors[handle].pwm_gen_clockwise,  clockwiseAction));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(motors[handle].pwm_gen_counterclockwise,  counterclockwiseAction));

    // set the operator based on the magnitude of the speed.
    speed = fabsf(speed);
    if(speed>1) speed = 1;

    // we set the pwm line low whenever the timer resets, so we want the comparator value to be closer to zero the greater
    // the speed value is.
    uint32_t comparatorTickValue = (1-speed)*PWM_PERIOD_TICKS; 
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(motors[handle].pwm_comparator, comparatorTickValue));

    // finally update the speed we set the motor to. this really is only for reading later.
    ESP_LOGI(LOG_TAG, "Set motor %d speed to %f", handle, speed);
    motors[handle].speed = speed * (isClockwise?1:-1);


    if(!xSemaphoreGive(motors[handle].read_write_mutex))
    {
        ESP_LOGE(LOG_TAG, "PWM_get_motor_speed: Failed to give mutex for motor %i.\n", handle);
        abort();  
    }


}


float PWM_get_motor_speed(pwm_motor_handle_t handle)
{
    if(!xSemaphoreTake(motors[handle].read_write_mutex, portMAX_DELAY))
    {
        ESP_LOGE(LOG_TAG, "PWM_get_motor_speed: Failed to take mutex for motor %i.\n", handle);
        abort();  
    }

    float toReturn = motors[handle].speed;

    if(!xSemaphoreGive(motors[handle].read_write_mutex))
    {
        ESP_LOGE(LOG_TAG, "PWM_get_motor_speed: Failed to give mutex for motor %i.\n", handle);
        abort();  
    }

    
    return toReturn;
}


