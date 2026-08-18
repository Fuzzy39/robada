#include <stdio.h>
#include "nvs_flash.h"

#include "esp_nimble_hci.h"

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
    

}
