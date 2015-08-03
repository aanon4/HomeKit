/*
 * switch.c
 *
 *  Created on: Jun 30, 2015
 *      Author: tim
 */

#include <string.h>

#include <ble.h>
#include <ble_gatts.h>
#include <app_error.h>
#include <nordic_common.h>
#include <nrf_gpio.h>

#include "homekit/homekit-config.h"
#include "homekit/uuids.h"
#include "homekit/service.h"
#include "switch.h"

#define NAME  "Demo Switch"
#if defined(NRF52)
#define LED   17 // P0.17
#else
#define LED   21 // P0.21
#endif
static uint8_t switch_state = 0;

static void service_switch_state(uint8_t** p_data, uint16_t* p_length, const service_characteristic_t* characteristic)
{
  *p_data = &switch_state;
  *p_length = 1;
}

static void service_switch_onoff(uint8_t* data, uint16_t length, const service_characteristic_t* characteristic)
{
  if (length > 0 && data[0] != switch_state)
  {
    switch_state = !!data[0];
    nrf_gpio_pin_write(LED, !switch_state);
#if !defined(NRF52)
    service_notify(characteristic);
#endif
  }
}


void service_switch_init(void)
{
  static const service_service_t service =
  {
    .uuid = { .type = BLE_UUID_TYPE_BLE, .uuid = HOMEKIT_SERVICE_UUID_SWITCH }
  };
  static const service_characteristic_t characteristics[] =
  {
    { .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_NAME }, SERVICE_STRING(NAME) },
    { .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_SWITCH_ON }, .length = 1, .read = service_switch_state, .write = service_switch_onoff, .notify = 1 },
    {}
  };
  service_addService(&service, characteristics);

  nrf_gpio_cfg_output(LED);
  nrf_gpio_pin_write(LED, !switch_state);
}
