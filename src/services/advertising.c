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
#include "homekit/category.h"
#include "homekit/uuids.h"
#include "homekit/crypto/crypto.h"

void advertising_init(void)
{
    uint32_t      err_code;

    static const ble_uuid_t uuids[] =
    {
        { .type = BLE_UUID_TYPE_BLE, .uuid = HOMEKIT_SERVICE_UUID_PAIRING }
    };
    const uint8_t service_data_data[] =
    {
        HOMEKIT_CONFIG_DEVICE_NAME_BYTES,
        HOMEKIT_CONFIG_CATEGORY,
        HOMEKIT_CONFIG_VERSION,
        crypto_advertise()
    };
    const ble_advdata_service_data_t service_data =
    {
       .service_uuid = HOMEKIT_SERVICE_UUID_PAIRING,
       .data =
       {
         .p_data = (uint8_t*)service_data_data,
         .size = sizeof(service_data_data)
       }
    };
    const ble_advdata_t adv_data =
    {
      .name_type = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
      .flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,
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

    // Reinitialize
    advertising_init();

    // Start advertising
    ble_gap_adv_params_t adv_params =
    {
      .type        = BLE_GAP_ADV_TYPE_ADV_IND,
      .fp          = BLE_GAP_ADV_FP_ANY,
      .interval    = MSEC_TO_UNITS(APP_ADV_INTERVAL, UNIT_0_625_MS),
      .timeout     = APP_ADV_TIMEOUT_IN_SECONDS,
#if 0
      .channel_mask = { .ch_38_off = 1, .ch_39_off = 1 } // HELP DEBUGGING
#endif
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
