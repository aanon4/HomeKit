/*
 * gap.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <string.h>

#include <nordic_common.h>
#include <ble.h>
#include <nrf.h>
#include <ble_gap.h>
#include <app_error.h>
#include <app_util.h>

#include "gap.h"

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_init(void)
{
    uint32_t err_code;

    ble_gap_addr_t mac;
    err_code = sd_ble_gap_address_get(&mac);
    APP_ERROR_CHECK(err_code);

    static const ble_gap_conn_sec_mode_t sec_mode = { 1, 1 };
    const uint8_t id = mac.addr[0];
    const uint8_t name[] = { PRODUCT_NAME,  (id < 0xA0 ? '0' : 'A' - 10) + (id >> 4),  ((id & 0xF) < 0xA ? '0' : 'A' - 10) + (id & 0xF) };
    err_code = sd_ble_gap_device_name_set(&sec_mode,  name, sizeof(name));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    static const ble_gap_conn_params_t gap_conn_params =
    {
      .min_conn_interval = MIN_CONN_INTERVAL,
      .max_conn_interval = MAX_CONN_INTERVAL,
      .slave_latency     = SLAVE_LATENCY,
      .conn_sup_timeout  = CONN_SUP_TIMEOUT
    };

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}
