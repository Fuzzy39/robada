#ifndef BLE_PWM_H
#define BLE_PWM_H   


#include "bluetooth.h"
#include "pwmControl.h"
#include "defines.h"

// header for implementing Bluetooth control of the motor PWM.

void initialize_bluetooth_pwm();


int blepwm_on_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *context, void *arg); 


#endif