/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

gpio_num_t pins[] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26,
                     GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13 };


void config_pins(void)
{
    for(int i = 0; i<8; i++)
    {
        gpio_reset_pin(pins[i]);
        gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
    }
}


void set_num(uint8_t num)
{
    for(int i = 0; i<8; i++)
    {
        bool bit = (num & 0x80)?true:false;
        gpio_set_level(pins[i], bit);
        num = num << 1;
    }

}

void app_main(void)
{
    printf("Hello world!\n");

    uint8_t num = 3;
    config_pins();


    while(true)
    {
        printf("Outputting: %d\n", num);

        set_num(num);
        num+=3;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
