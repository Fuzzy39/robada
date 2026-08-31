#include "bluetoothPWM.h"

static const char* LOG_TAG = "ble_pwm";
static const uint8_t uuidBase = 0x01;




static const struct ble_gatt_svc_def gatt_svr_svcs[] = 
{
    /* Automation IO service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[])
            {
                { // Base motor characteristic
                    .uuid = bluetooth_create_uuid(uuidBase, 0),
                    .access_cb = blepwm_on_access,
                    .arg = &BASE_MOTOR,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                },
                 { // Shoulder motor characteristic
                    .uuid = bluetooth_create_uuid(uuidBase, 1),
                    .access_cb = blepwm_on_access,
                    .arg = &SHOULDER_MOTOR,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                },
                {0}
            },
    },

    {
        0, /* No more services. */
    },
};



void initialize_bluetooth_pwm()
{
    bluetooth_add_services(gatt_svr_svcs);
}


int blepwm_on_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *context, void *arg)
{
    pwm_motor_handle_t motor = (pwm_motor_handle_t)*arg;

    switch (context->op) 
    {
    /* Write characteristic event */
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* Verify access buffer length */
        if (context->om->om_len != sizeof(float)) {
            ESP_LOGE(TAG, "Wrong length for test characteristic callback. opcode: %d, got: %d, expected: 1", context->op, context->om->om_len);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        
        }
    
        // actually write the data.
        float speed = context->om->om_data[0];

        ESP_LOGI(TAG, "Motor %d speed write: %f.");

        PWM_claim_motor(motor, true);
        PWM_set_motor_speed(motor, speed);
        PWM_release_motor(motor, false);
       
        return 0;
    
    case BLE_GATT_ACCESS_OP_READ_CHR:
    
        float speed = PWM_get_motor_speed(motor);

        ESP_LOGI(TAG, "Motor %d Speed Read: %f.", motor, speed);

        int error = os_mbuf_append(context->om, &speed,
                                sizeof speed);

        return error == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  

    /* Unknown event */
    default:
        ESP_LOGE(TAG, "unexpected access operation to test characteristic, opcode: %d", context->op);
    }

    return BLE_ATT_ERR_UNLIKELY;
}

