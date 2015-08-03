/*
 * service.h
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_SERVICE_H_
#define HOMEKIT_SERVICE_H_


#define HOMEKIT_SERVICE_FAN                 0x0040
#define HOMEKIT_SERVICE_GARAGE_DOOR         0x0041
#define HOMEKIT_SERVICE_LIGHTBULB           0x0043
#define HOMEKIT_SERVICE_LOCK_MNGMT          0x0044
#define HOMEKIT_SERVICE_LOCK_MECH           0x0045
#define HOMEKIT_SERVICE_OUTLET              0x0047
#define HOMEKIT_SERVICE_SWITCH              0x0049
#define HOMEKIT_SERVICE_THERMOSTAT          0x004A
#define HOMEKIT_SERVICE_AIR_QUALITY         0x008D
#define HOMEKIT_SERVICE_ALARM               0x007E
#define HOMEKIT_SERVICE_CO_SENSOR           0x007F
#define HOMEKIT_SERVICE_CONTACT_SENSOR      0x0080
#define HOMEKIT_SERVICE_DOOR                0x0081
#define HOMEKIT_SERVICE_HUMIDITY_SENSOR     0x0082
#define HOMEKIT_SERVICE_LEAK_SENSOR         0x0083
#define HOMEKIT_SERVICE_LIGHT_SENSOR        0x0084
#define HOMEKIT_SERVICE_MOTION_SENSOR       0x0085
#define HOMEKIT_SERVICE_OCCUPANCY_SENSOR    0x0086
#define HOMEKIT_SERVICE_SMOKE_SENSOR        0x0087
#define HOMEKIT_SERVICE_STATEFUL_SWITCH     0x0088
#define HOMEKIT_SERVICE_STATELESS_SWITCH    0x0089
#define HOMEKIT_SERVICE_TEMP_SENSOR         0x008A
#define HOMEKIT_SERVICE_WINDOW              0x008B
#define HOMEKIT_SERVICE_WINDOW_COVERING     0x008C


struct service_characteristic_t;

typedef void (*service_read_t)(uint8_t** p_data, uint16_t* p_length, const struct service_characteristic_t* characteristic);
typedef void (*service_write_t)(uint8_t* data, uint16_t length, const struct service_characteristic_t* characteristic);

typedef struct
{

  ble_uuid_t    uuid;

} service_service_t;

typedef struct service_characteristic_t
{
  ble_uuid_t      uuid;
  service_read_t  read;
  service_write_t write;
  uint16_t        length;
  uint16_t        max_length;
  void*           ctx;
  uint8_t         plain:1;
  uint8_t         notify:1;

} service_characteristic_t;

extern void service_ble_event(ble_evt_t* event);
extern void service_addService(const service_service_t* service, const service_characteristic_t characteristic[]);

// Helpers
extern void service_notify(const service_characteristic_t* characteristic);
extern void service_read_string(uint8_t** p_data, uint16_t* p_length, const service_characteristic_t* characteristic);

#define SERVICE_STRING(STR)   .ctx = (STR), .length = sizeof(STR) - 1, .read = service_read_string

#endif /* HOMEKIT_SERVICE_H_ */
