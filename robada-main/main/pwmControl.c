#include "defines.h"
#include "pwmControl.h"
#include <limits.h>

static const char* LOG_TAG = "PWM";

static const uint32_t TIMER_HZ = 10000000; // 10 MHz
static const uint32_t PERIOD_TICKS = 1000; // 10    khz 

static mcpwm_timer_handle_t timer;

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

    esp_err_t err = mcpwm_new_timer(&timerConfig, &timer);
    if(err!=ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Couldn't make timer. Got error code %d\n", err);
        return false;    
    }
    ESP_LOGI(LOG_TAG, "Set up the timer.\n");


    for(int i = 0; i<numMotors; i++)
    {   
        motors[i]
        if(!PWM_setup_motor(MotorConfig[i]))
        {
            return false;
        }
    }

    return true;
}


void PWM_setup_motor(MotorConfig config, PwmMotor* motor)
{

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