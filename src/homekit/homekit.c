/*
 * homekit.c
 *
 *  Created on: Jun 27, 2015
 *      Author: tim
 */

#include <ble.h>

#include "uuids.h"
#include "accessoryinfo.h"
#include "pairing.h"
#include "service.h"
#include "session.h"


void homekit_init(void)
{
  uuids_init();
  accessoryinfo_init();
  pairing_init();
}

void homekit_ble_event(ble_evt_t* p_ble_evt)
{
  pairing_ble_event(p_ble_evt);
  service_ble_event(p_ble_evt);
}

void homekit_execute(void)
{
  pairing_execute();
}
