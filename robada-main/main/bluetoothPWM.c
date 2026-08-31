#include "bluetoothPWM.h"

static const char* LOG_TAG = "ble_pwm";
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static const uint8_t uuidBase = 0x01;


pwm_motor_handle_t baseMotor; // yes this feels dumb.
pwm_motor_handle_t shoulderMotor;

ble_uuid128_t base_char_uuid;
ble_uuid128_t shoulder_char_uuid; 


static struct ble_gatt_svc_def gatt_svr_svcs[] = 
{
    /* Automation IO service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[])
            {
                { // Base motor characteristic
                    .uuid = (ble_uuid_t*)&base_char_uuid,
                    .access_cb = blepwm_on_access,
                    .arg = &baseMotor, // don't modify these
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                },
                 { // Shoulder motor characteristic
                    .uuid = (ble_uuid_t*)&shoulder_char_uuid,
                    .access_cb = blepwm_on_access,
                    .arg = &shoulderMotor,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                },
                {0}
            },
    },

    {
        0, /* No more services. */
    },
};



void bluetooth_pwm_initialize()
{
    // this feels very dumb.
    ble_uuid128_t* uuid = bluetooth_create_uuid(uuidBase, 0);
    base_char_uuid = *uuid;
    free(uuid);

    uuid = bluetooth_create_uuid(uuidBase, 1);
    shoulder_char_uuid = *uuid;
    free(uuid);


    baseMotor = BASE_MOTOR;
    shoulderMotor = SHOULDER_MOTOR;

    bluetooth_add_services(gatt_svr_svcs);
}


int blepwm_on_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *context, void *arg)
{
    pwm_motor_handle_t motor = *(pwm_motor_handle_t*)arg;
    float speed;

    switch (context->op) 
    {
    /* Write characteristic event */
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* Verify access buffer length */
        if (context->om->om_len != sizeof(float)) {
            ESP_LOGE(LOG_TAG, "Wrong length for test characteristic callback. opcode: %d, got: %d, expected: 1", context->op, context->om->om_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        
        }
    
        // actually write the data.
        speed = *(float*)(context->om->om_data);

        // char str[9];
        // spri5ntf(str, "%08lX", *(uint32_t*)temp);
        ESP_LOGI(LOG_TAG, "Motor %d speed write: %f.");

        PWM_claim_motor(motor, true);
        PWM_set_motor_speed(motor, speed);
        PWM_release_motor(motor, false);
       
        return 0;
    
    case BLE_GATT_ACCESS_OP_READ_CHR:
    
        speed = PWM_get_motor_speed(motor);
        void* temp = &speed;

        char str[9];
        sprintf(str, "%08lX", *(uint32_t*)temp);
        ESP_LOGI(LOG_TAG, "Motor %d Speed Read: %f. (%s)", motor, speed, str);
    
        

        int error = os_mbuf_append(context->om, &speed,
                                sizeof speed);

        return error == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  

    /* Unknown event */
    default:
        ESP_LOGE(LOG_TAG, "unexpected access operation to test characteristic, opcode: %d", context->op);
    }

    return BLE_ATT_ERR_UNLIKELY;
}

