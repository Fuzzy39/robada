#ifndef BLE_H
#define BLE_H   


#include "host/ble_gatt.h"
#include "defines.h"


static const uint8_t base_uuid[16] = {0x00, 0x00, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef,
                     0x12, 0x12, 0x25, 0x15, 0x00, 0x00};
                     




// Add a ble service.
void bluetooth_add_services(struct ble_gatt_svc_def* service_definitions);

void bluetooth_initialize();



#endif