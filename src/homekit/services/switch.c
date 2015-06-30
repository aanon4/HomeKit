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

#define ID    "1"
#define NAME  "Demo Switch"
#define LED   21 // P0.21

static void service_switch_state(uint8_t** p_data, uint16_t* p_length, const service_characteristic_t* characteristic)
{
  static uint8_t state;

  state = !nrf_gpio_pin_read(LED);

  *p_data = &state;
  *p_length = 1;
}

static void service_switch_onoff(uint8_t* data, uint16_t length, const service_characteristic_t* characteristic)
{
  if (length > 0 && data[0] != !nrf_gpio_pin_read(LED))
  {
    nrf_gpio_pin_write(LED, !data[0]);
    service_notify(characteristic);
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
    { .name = "Service Instance ID", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_SERVICE_ID }, .length = sizeof(ID) - 1, .ctx = ID, .read = service_read_string, .plain = 1 },
    { .name = "Name", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_NAME }, .length = sizeof(NAME) - 1, .ctx = NAME, .read = service_read_string },
    { .name = "On", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_SWITCH_ON }, .length = 1, .read = service_switch_state, .write = service_switch_onoff, .notify = 1 },
    {}
  };
  service_addService(&service, characteristics);

  nrf_gpio_cfg_output(LED);
  nrf_gpio_pin_write(LED, 1);
}
