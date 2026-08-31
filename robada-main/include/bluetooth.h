#ifndef BLE_H
#define BLE_H   


#include "host/ble_gatt.h"
#include "host/ble_uuid.h"
#include "defines.h"


void bluetooth_initialize();

// Creates a UUID on the heap. Caller is responsible for freeing the memory.
ble_uuid_t* bluetooth_create_uuid(uint8_t file, uint8_t obj);

// Add a ble service.
void bluetooth_add_services(struct ble_gatt_svc_def* service_definitions);






#endif