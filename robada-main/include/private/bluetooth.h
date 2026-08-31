#ifndef BLE_PRV_H
#define BLE_PRV_H


#include "esp_log.h"

#include "nvs_flash.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include <string.h>

// This function is in nimble but isn't in the header.
// https://github.com/espressif/esp-nimble/issues/56
// What does it do? Why do we want it? I have no idea. We do call it as part of initialization, though.
void ble_store_config_init(void);

// initialization functions
void gap_init();
void gatt_server_init();
void start_advertising();
void nimble_host_task(void *param);


// Bluetooth callbacks
void on_stack_reset(int reason);
void on_stack_sync(void);

int gap_event_handler(struct ble_gap_event *event, void *arg);
int gap_event_connect_handler(struct ble_gap_event *event, void *arg);


#endif