idf_component_register(SRCS "main.c" "buttonDebounce.c" "pwmControl.c" "defines.c" "bluetooth.c"
                    PRIV_REQUIRES bt nvs_flash esp_driver_mcpwm esp_driver_gpio
                    INCLUDE_DIRS "../include")
