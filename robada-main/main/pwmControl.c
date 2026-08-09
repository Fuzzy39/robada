#include "defines.h"
#include "pwmControl.h"
#include <limits.h>

static const char* LOG_TAG = "PWM";

static const uint32_t TIMER_HZ = 10000000; // 10 MHz
static const uint32_t PERIOD_TICKS = 1000; // 10    khz 

static mcpwm_timer_handle_t timer;
static mcpwm_fault_handle_t softwareFault;

mcpwm_gen_compare_event_action_t comparatorAction; // configuration object we modify frequently.

static PwmMotor motors[3];


void PWM_initialize(gpio_num_t motorEnable, MotorConfig* motors, size_t numMotors)
{
    // start by setting up the timer.

    mcpwm_timer_config_t timerConfig = {
        .group_id = 0, // 0, 1, 2, We've got 3 of these guys on board.
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = TIMER_HZ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = PERIOD_TICKS,
        .intr_priority = 0, // Default, low priority
        .flags = {
            .update_period_on_empty = true,
            .update_period_on_sync = false,
            .allow_pd = false, // We never go to sleep, so this is irrelevant.      
        }
    };

    ESP_ERR_CHECK(mcpwm_new_timer(&timerConfig, &timer));
    // ESP_ERR_CHECK checks for an error (duh) and prints out a message if there is one.
    // It'll also call 'abort()' which I assume resets the processor... looks like by default, yes.
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/fatal-errors.html


    // While we're here, we also want to setup the fault object.
    mcpwm_soft_fault_config_t faultConfig = {}; // this thing has no members! wonder how that works.
    // apparently it's undefined behavior
    // oh well, not my fault.
    ESP_ERR_CHECK(mcpwm_new_soft_fault(&faultConfig, &softwareFault));
   
    ESP_LOGI(LOG_TAG, "Initialized common MCPWM objects.\n");

    for(int i = 0; i<numMotors; i++)
    {   
        motors[i] = {}; // zero initialize the struct. this may be uneeded.
        PWM_setup_motor(&MotorConfig[i], &motors[i]);
    }

    ESP_LOGI(LOG_TAG, "Initialized PWM controller.\n");
    return true;
}


void PWM_setup_motor(const MotorConfig* config, PwmMotor* motor)
{
    // a PwmMotor needs ....
    // An operator, comparator, 2 generators, and 2 semaphores.
    // Okay, lets get cracking, then.


    // operator first, since it contains everything.
    mcpwm_operator_config_t operatorConfig = {
        .group_id = config->motor,
        .intr_priority = 0
    };

    ESP_ERR_CHECK(mcpwm_new_operator(&operatorConfig, &(motor->pwm_operator)));
    // connect the timer to the operator so we can get something done.
    ESP_ERR_CHECK(mcpwm_operator_connect_timer(motor->pwm_operator, timer));

    // next, the comparator.
    mcpwm_comparator_config_t comparatorConfig = {
        .intr_priority = 0,
        .flags = {
            .update_cmp_on_tez = true,
            .update_cmp_on_tep = false,
            .update_cmp_on_sync = false
        }
    };

    ESP_ERR_CHECK(mcpwm_new_comparator(motor->pwm_operator, &comparatorConfig, &(motor->pwm_comparator)));

    // We will set the compare value when we set the motor speed by the end of this.

    // Generator time!
    mcpwm_generator_config_t generatorConfig = {
        .gen_gpio_num = config->clockwiseGpioNum, // Motor one clockwise
        .flags = {
            .invert_pwm = false,
        },
    };

    ESP_ERR_CHECK(mcpwm_new_generator(motor->pwm_operator, &generatorConfig, &(motor->pwm_gen_clockwise)));

    generatorConfig.gen_gpio_num = config->counterclockwiseGpioNum;
    ESP_ERR_CHECK(mcpwm_new_generator(motor->pwm_operator, &generatorConfig, &(motor->pwm_gen_counterclockwise)));

    // generator events...
    mcpwm_gen_timer_event_action_t timerAction =
    {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_LOW
    };
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_timer_event(motor->pwm_gen_clockwise,  timerAction));
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_timer_event(motor->pwm_gen_counterclockwise,  timerAction));

    comparatorAction =  // There are macros for this.
    {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = comparator,
        .action = MCPWM_GEN_ACTION_LOW // We are also setting the comparator low, because the motors will be stopped initially... is this the best way to do this?
    };
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_compare_event(motor->pwm_gen_clockwise,  comparatorAction));
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_compare_event(motor->pwm_gen_counterclockwise,  comparatorAction));

    // don't forget to set the fault stuff!
    mcpwm_gen_fault_event_action_t faultAction = 
    {
        .direction = MCPW_TIMER_DIRECTION_UP,
        .fault = softwareFault,
        .action = MCPWM_GEN_ACTION_LOW
    };
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_fault_event(motor->pwm_gen_clockwise, faultAction));
    ESP_ERR_CHECK(mcpwm_generator_set_action_on_fault_event(motor->pwm_gen_counterclockwise, faultAction));


    // And initialize our semaphore and mutex.
    // TODO!

    // finally, set the speed. We use this 'raw' version to avoid making the task that initializes the pwm claim the motors.
    PWM_set_motor_speed(config->motor, 0);

}


bool PWM_claim_motor(Motor motor, bool shouldBlock); 


void PWM_release_motor(Motor motor, bool stopMotor);


bool PWM_set_motor_speed(Motor motor, float speed);


float PWM_get_motor_speed(Motor motor);


void PWM_stop_all_motors();


void init()
{
    



    mcpwm_oper_handle_t operator;
    mcpwm_operator_config_t operatorConfig = {
        .group_id = 0,
        .intr_priority = 0
    };

    err = mcpwm_new_operator(&operatorConfig, &operator);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't make Operator. Got error code %d\n", err);
        return;    
    }
    ESP_LOGI(LOG_TAG, "Made Operator. yay!\n");


    mcpwm_cmpr_handle_t comparator;
    mcpwm_comparator_config_t comparatorConfig = {
        .intr_priority = 0,
        .flags = {
            .update_cmp_on_tez = true,
            .update_cmp_on_tep = false,
            .update_cmp_on_sync = false
        }
    };

    // connect the timer to the operator so we can get something done.
    err = mcpwm_operator_connect_timer(operator, timer);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't connect timer to operator. Code %d\n", err);
        return;    
    }
    ESP_LOGI(LOG_TAG, "Connected Timer! yay!\n");  

    err = mcpwm_new_comparator(operator, &comparatorConfig, &comparator);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't make Comparator. Got error code %d\n", err);
        return;    
    }


    err = mcpwm_comparator_set_compare_value(comparator, PERIOD_TICKS/2);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't set Comparator. Got error code %d\n", err);
        return;    
    }
    ESP_LOGI(LOG_TAG, "Made Comparator. yay!\n");

    
    mcpwm_gen_handle_t generator;
    mcpwm_generator_config_t generatorConfig = {
        .gen_gpio_num = 26, // Motor one clockwise
        .flags = {
            .invert_pwm = false,
        },
    };

    err = mcpwm_new_generator(operator, &generatorConfig, &generator);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't make Generator. Got error code %d\n", err);
        return;    
    }


    mcpwm_gen_timer_event_action_t timerAction =
    {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .event = MCPWM_TIMER_EVENT_EMPTY,
        .action = MCPWM_GEN_ACTION_LOW
    };
    err = mcpwm_generator_set_action_on_timer_event(generator,  timerAction);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't setup Generator timer event. Got error code %d\n", err);
        return;    
    }

    mcpwm_gen_compare_event_action_t comparatorAction =  // There are macros for this.
    {
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .comparator = comparator,
        .action = MCPWM_GEN_ACTION_HIGH
    };
    err = mcpwm_generator_set_action_on_compare_event(generator,  comparatorAction);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't setup Generator comparator event. Got error code %d\n", err);
        return;    
    }


    ESP_LOGI(LOG_TAG, "Made Generator. yay!\n");

    
    // I think I need to enable the timer?
    if(mcpwm_timer_enable(timer) != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't enable timer properly!\n");
        return;
    }

    if(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP) != ESP_OK)
    {
          ESP_LOGE(LOG_TAG, "Couldn't start timer properly!\n");
        return;
    }

}