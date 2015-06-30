/*
 * connparams.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <app_util.h>
#include <app_timer.h>
#include <app_error.h>

#include "timer.h"
#include "connparams.h"


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    uint32_t err_code;

    static const ble_conn_params_init_t conparam =
    {
        .first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY,
        .next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY,
        .max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT,
        .start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID,
    };
    err_code = ble_conn_params_init(&conparam);
    APP_ERROR_CHECK(err_code);
}
