/*
 * dis.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <ble_dis.h>
#include <app_error.h>

#include "devinfo.h"


/**@brief Function for initializing Device Information Service.
 */
void devinfo_init(void)
{
    uint32_t err_code;

    ble_dis_init_t discfg =
    {
    	.manufact_name_str.p_str = MANUFACTURER_NAME,
    	.manufact_name_str.length = sizeof(MANUFACTURER_NAME),
    	.dis_attr_md.read_perm = { 1, 1 },
    	.dis_attr_md.write_perm = { 0, 0 }
    };

    err_code = ble_dis_init(&discfg);
    APP_ERROR_CHECK(err_code);
}
