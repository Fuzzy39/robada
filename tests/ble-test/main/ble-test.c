#include <stdio.h>
#include "esp_log.h"

#include "nvs_flash.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"


static const char* TAG = "ble-test";
const uint16_t BLE_GAP_APPEARANCE_GENERIC_INDUSTRIAL_TOOL = 0x14C0;


// This function is in nimble but isn't in the header.
// https://github.com/espressif/esp-nimble/issues/56
// What does it do? Why do we want it? I have no idea. We do call it as part of initialization, though.
void ble_store_config_init(void);
// I have no idea what half of these functions do. They aren't documented.
// this is frustrating.

// stolen from the example. Takes in a bluetooth address and spits out a string.
// seems... unsafe but I'm not going to bother fixing it at the moment.
inline static void format_addr(char *addr_str, uint8_t addr[]) 
{
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1],
            addr[2], addr[3], addr[4], addr[5]);
}


static void on_stack_reset(int reason)
{
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

static void on_stack_sync(void) 
{
    // the bluetooth host and controller need to 'sync' I guess.
    // We shouldn't do any bluetooth operations until this has happened.
    // this callback is for when that happens, so we'll do our advertising here.

    // Ensure that the device has a bluetooth address. https://github.com/apache/mynewt-nimble/blob/1e8ed60276f35a80ed4d4b4f8bb9d9c6fee53845/nimble/host/util/include/host/util/util.h#L40
    int error = ble_hs_util_ensure_addr(false);
    // I've come to the realization that nimble functionally only has documentation in the form of doxygen comments inside of it's headers.
    // Which is better than nothing, but the actual documentation page it has is basically useless.

    if(error)
    {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    uint8_t addressType; // will either be  BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM. I think.
    // SHOCKINGLY - this is the first nimble function that is in the documentation so far. All of main and everywhere else - undocumented.
    error = ble_hs_id_infer_auto(false, &addressType); // Not sure what this number it returns is, but it claims to be an address type.
                                                         // there are 4 address types as I understand. Public, random, resolvable random private and unresolvable random private.
                                                         // Random is probably what we're using?
    if (error)
    {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", error);
        return;
    }

    // get the 48-bit bluetooth address that we'll be using.
    uint8_t bluetoothAddr[6];
    error = ble_hs_id_copy_addr(addressType, bluetoothAddr, NULL);
    if (error)
    {
        ESP_LOGE(TAG, "failed to copy device address, error code: %d", error);
        return;
    }

    char addrString[18];
    format_addr(addrString, bluetoothAddr);
    ESP_LOGI(TAG, "device address: %s", addrString);

    // Now we can actually try to advertise...
    // Well, we need to set up a bunch of data structures first actually.

    // This is the data we're sending in the advertisement.

    struct ble_hs_adv_fields adv_fields = {0};
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    const char *name;
    name = ble_svc_gap_device_name(); // I guess we're getting the name from when we set it earlier? Okay, I guess.
    adv_fields.name = (uint8_t *)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    /* Set device tx power */
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO; // Auto power level..
    adv_fields.tx_pwr_lvl_is_present = 1;

    /* Set device appearance */
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_INDUSTRIAL_TOOL; 
    adv_fields.appearance_is_present = 1;

    /* Set device LE role */
    adv_fields.le_role = MYNEWT_VAL_BLE_ROLE_PERIPHERAL; // this isn't what the example has, but gcc suggested it, as what the example has apparently doesn't exist.
    adv_fields.le_role_is_present = 1;

    // So all of this stuff will be sent when we advertise, I guess.
    error = ble_gap_adv_set_fields(&adv_fields);
    if (error) 
    {
        ESP_LOGE(TAG, "failed to set advertising data, error code: %d", error);
        return;
    }

    struct ble_gap_adv_params adv_params = {0};
    /* Set non-connetable and general discoverable mode to be a beacon */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    /* Start advertising */
    error = ble_gap_adv_start(addressType, NULL, BLE_HS_FOREVER, &adv_params,
                           NULL, NULL);
    if (error)
    {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", error);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");


}

static void nimble_host_task(void *param) 
{
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}



void app_main(void)
{
    printf("Hello, world!\n");
    
    // Init nvs flash. I guess it's a key value store inside of flash (where the bootloader and application code live.) It's a more lightweight alternative to a filesystem on the flash.
    // Our code doesn't use it, but some part of the bluetooth stack does, so we enable it.
    ESP_ERROR_CHECK(nvs_flash_init());

    // Magic undocumented function that initializes nimble I guess.
    // At this point it's not running in a seperate thread. We'll do some more initialization before setting that up.
    ESP_ERROR_CHECK(nimble_port_init());

    // GAP (Generic Access Profile) is a Profile(? API/thing) that provides for device discovery.
    // A device can either announce itself or scan for announcments. If the scanning device decides to initiate a connection,
    // the announcer becomes the periphrial and the scanner becomes the central device.
    
    // Since the eventual goal is that the robot is a periphrial controlled by the pcApp, we intend to announce.
    // Either way, we need to set up and configure GAP.

    ble_svc_gap_init(); // undocumented mystery function that initializes GAP I guess? 
                        // thanks, expressif, I guess
    
    // Set GAP device name... this function is also not documented. ble_svc_gap functions aren't.
    // I did figure out svc stands for services, though. is that useful?
    int error = ble_svc_gap_device_name_set("ESP_Test"); // Does this get sent in the announce packet? If so, where?
    // the function itself checks the string length then does a memcpy to somewhere... presumably that variable gets used.
    // okay, I guess?
    if (error) 
    {
        ESP_LOGE(TAG, "failed to set device name. error code: %d",
                 error);
       abort();
    }



    /* Set GAP device appearance */
    error = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_INDUSTRIAL_TOOL);
    if (error)
    {
        ESP_LOGE(TAG, "failed to set device appearance, error code: %d", error);
        abort();
    }


    // Apparently the host and controller have to be synced, and might not always be. So we have functions we can call if that's the case.
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset; // the bluetooth stack is reset if a significant error occurs.
    ble_hs_cfg.sync_cb = on_stack_sync; 
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr; // Litterally no idea what this is...
    // Ah https://github.com/apache/mynewt-nimble/blob/1e8ed60276f35a80ed4d4b4f8bb9d9c6fee53845/nimble/host/include/host/ble_hs.h#L272
    // I guess, for a connection or something, bluetooth needs to store stuff. If we run out of space or something like that, this function is called.
    // Idk what the function we gave it is but I guess that's fine??


    /* Store host configuration */
    ble_store_config_init();
    // litterally no idea what this does, as I said before. Looking it up seems to indicate it maybe has something to do with the nvs from before?

    // Oh well, we're almost done with initialization.

    /* Start NimBLE host task thread and return */
    xTaskCreate(nimble_host_task, "NimBLE Host", 4*1024, NULL, 5, NULL);
}
