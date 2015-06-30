/*
 * advertising.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <ble.h>
#include <nrf.h>
#include <ble_srv_common.h>
#include <ble_advdata.h>
#include <app_error.h>

#include "gap.h"
#include "advertising.h"

#include "homekit/homekit-config.h"
#include "homekit/uuids.h"

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
void advertising_init(void)
{
    uint32_t      err_code;

    static const uint8_t flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    static const ble_uuid_t uuids[] =
    {
        { .type = BLE_UUID_TYPE_BLE, .uuid = HOMEKIT_SERVICE_UUID_PAIRING }
    };
    static const uint8_t service_data_data[] =
    {
        HOMEKIT_CONFIG_DEVICE_NAME_BYTES,
        0x01, 0x00, 0x01
    };
    static const ble_advdata_service_data_t service_data =
    {
       .service_uuid = HOMEKIT_SERVICE_UUID_PAIRING,
       .data =
       {
         .p_data = (uint8_t*)service_data_data,
         .size = sizeof(service_data_data)
       }
    };
    static const ble_advdata_t adv_data =
    {
      .name_type = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
      .flags =
      {
        .p_data = (uint8_t*)&flags,
        .size = sizeof(flags)
      },
      .uuids_complete =
      {
        .uuid_cnt = sizeof(uuids) / sizeof(uuids[0]),
        .p_uuids = (ble_uuid_t*)uuids
      },
      .service_data_count = 1,
      .p_service_data_array = (ble_advdata_service_data_t*)&service_data,
    };

    err_code = ble_advdata_set(&adv_data, NULL);
    APP_ERROR_CHECK(err_code);
}

void advertising_start(void)
{
    uint32_t err_code;

    // Start advertising
    ble_gap_adv_params_t adv_params =
    {
      .type        = BLE_GAP_ADV_TYPE_ADV_IND,
      .fp          = BLE_GAP_ADV_FP_ANY,
      .interval    = MSEC_TO_UNITS(APP_ADV_INTERVAL, UNIT_0_625_MS),
      .timeout     = APP_ADV_TIMEOUT_IN_SECONDS,
      .channel_mask = { .ch_38_off = 1, .ch_39_off = 1 } // HELP DEBUGGING
    };

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);
}

void advertising_stop(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_stop();
    APP_ERROR_CHECK(err_code);
}