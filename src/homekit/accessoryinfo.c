/*
 * accessoryinfo.c
 *
 *  Created on: Jun 8, 2015
 *      Author: tim
 */

#include <string.h>

#include <ble.h>
#include <ble_gatts.h>
#include <app_error.h>
#include <nordic_common.h>

#include "homekit-config.h"
#include "uuids.h"
#include "accessoryinfo.h"
#include "service.h"


static void accessoryinfo_identify(uint8_t* data, uint16_t length, const service_characteristic_t* characteristic)
{
  HOMEKIT_CONFIG_IDENTIFY_FUNCTION(data, length, ctx);
}

void accessoryinfo_init(void)
{
  static const service_service_t service =
  {
    .uuid = { .type = BLE_UUID_TYPE_BLE, .uuid = HOMEKIT_SERVICE_UUID_ACCESSORY_INFO }
  };
  static const service_characteristic_t characteristics[] =
  {
    { .name = "2;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_SERVICE_ID }, SERVICE_STRING(HOMEKIT_CONFIG_SERVICE_ID), .plain = 1 },
    { .name = "3;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_NAME }, SERVICE_STRING(HOMEKIT_CONFIG_NAME) },
    { .name = "4;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_MANUFACTURER }, SERVICE_STRING(HOMEKIT_CONFIG_MANUFACTURER) },
    { .name = "5;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_MODEL }, SERVICE_STRING(HOMEKIT_CONFIG_MODEL) },
    { .name = "6;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_SERIAL_NR }, SERVICE_STRING(HOMEKIT_CONFIG_SERIAL_NR) },
    { .name = "7;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_IDENTITY }, .length = 1, .write = accessoryinfo_identify, .plain = 1 },
#if defined(HOMEKIT_CONFIG_FIRMWARE_REV)
    { .name = "8;", .uuid = { .type = HOMEKIT_BASE_TYPE, .uuid = HOMEKIT_FIRMWARE_REV }, SERVICE_STRING(HOMEKIT_CONFIG_FIRMWARE_REV) },
#endif
    {}
  };
  service_addService(&service, characteristics);
}
