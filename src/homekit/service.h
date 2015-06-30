/*
 * service.h
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_SERVICE_H_
#define HOMEKIT_SERVICE_H_

struct service_characteristic_t;

typedef void (*service_read_t)(uint8_t** p_data, uint16_t* p_length, const struct service_characteristic_t* characteristic);
typedef void (*service_write_t)(uint8_t* data, uint16_t length, const struct service_characteristic_t* characteristic);

typedef struct
{

  ble_uuid_t    uuid;

} service_service_t;

typedef struct service_characteristic_t
{
  char*           name;
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

#endif /* HOMEKIT_SERVICE_H_ */
