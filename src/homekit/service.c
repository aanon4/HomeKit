/*
 * service.c
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#include <string.h>

#include <ble.h>
#include <ble_gatts.h>
#include <app_error.h>
#include <nordic_common.h>


#include "homekit-config.h"
#include "session.h"
#include "service.h"
#include "buffer.h"

static struct
{
  const service_characteristic_t*   current_characteristic;
  uint16_t                          connection_handle;
  uint16_t                          length;
  struct
  {
    uint16_t                        handle;
    const service_characteristic_t* characteristic;
  } map[HOMEKIT_CONFIG_SERVICE_MAX_CHARACTERISTICS];
  uint8_t           mapfree;
} service_state;

static const service_characteristic_t* service_findCharacteristicByHandle(uint16_t handle);


void service_addService(const service_service_t* service, const service_characteristic_t characteristics[])
{
  uint32_t err_code;

  uint16_t service_handle;
  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service->uuid, &service_handle);
  APP_ERROR_CHECK(err_code);

  for (unsigned i = 0; characteristics[i].name; i++)
  {
    if (service_state.mapfree >= HOMEKIT_CONFIG_SERVICE_MAX_CHARACTERISTICS)
    {
      APP_ERROR_CHECK(NRF_ERROR_INTERNAL);
    }

    ble_gatts_attr_md_t metadata =
    {
      .read_perm = { !!characteristics[i].read, !!characteristics[i].read },
      .write_perm = { !!characteristics[i].write, !!characteristics[i].write },
      .rd_auth = !!characteristics[i].read,
      .wr_auth = !!characteristics[i].write,
      .vlen = 1,
      .vloc = BLE_GATTS_VLOC_USER
    };
    const ble_gatts_attr_t attr =
    {
      .p_uuid = (ble_uuid_t*)&characteristics[i].uuid,
      .p_attr_md = &metadata,
      .max_len = SESSION_CIPHER_BUFFERLEN(characteristics[i].max_length ? characteristics[i].max_length : characteristics[i].length),
      .p_value = buffer_buffer
    };
    const ble_gatts_char_md_t character =
    {
      .char_props =
      {
        .read = !!characteristics[i].read,
        .write = !!characteristics[i].write,
        //.indicate = characteristics[i].notify
      },
      .p_char_user_desc = (uint8_t*)characteristics[i].name,
      .char_user_desc_max_size = strlen(characteristics[i].name),
      .char_user_desc_size = strlen(characteristics[i].name)
    };

    ble_gatts_char_handles_t newhandle;
    err_code = sd_ble_gatts_characteristic_add(service_handle, &character, &attr, &newhandle);
    APP_ERROR_CHECK(err_code);
    service_state.map[service_state.mapfree].handle = newhandle.value_handle;
    service_state.map[service_state.mapfree].characteristic = &characteristics[i];
    service_state.mapfree++;
  }
}

void service_ble_event(ble_evt_t* event)
{
  uint32_t err_code;

  switch (event->header.evt_id)
  {
  case BLE_GAP_EVT_CONNECTED:
    service_state.connection_handle = event->evt.gap_evt.conn_handle;
    break;

  case BLE_GAP_EVT_DISCONNECTED:
    service_state.connection_handle = BLE_CONN_HANDLE_INVALID;
    break;

  case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
  {
    switch (event->evt.gatts_evt.params.authorize_request.type)
    {
    case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
    {
      const service_characteristic_t* characteristic = service_findCharacteristicByHandle(event->evt.gatts_evt.params.authorize_request.request.write.handle);
      if (characteristic)
      {
        switch (event->evt.gatts_evt.params.authorize_request.request.write.op)
        {
        case BLE_GATTS_OP_WRITE_REQ:
          if (characteristic->write && (characteristic->plain || session_isEncrypted()))
          {
            uint16_t blength = 0;
            if (SESSION_PLAIN_BUFFERLEN(event->evt.gatts_evt.params.authorize_request.request.write.len) <= sizeof(buffer_buffer) && session_writeData(event->evt.gatts_evt.params.authorize_request.request.write.data, event->evt.gatts_evt.params.authorize_request.request.write.len, buffer_buffer, &blength))
            {
              characteristic->write(buffer_buffer, blength, characteristic->ctx);
            }
          }
          service_state.current_characteristic = NULL;
          break;

        case BLE_GATTS_OP_PREP_WRITE_REQ:
          if (!service_state.current_characteristic)
          {
            service_state.current_characteristic = characteristic;
            service_state.length = 0;
          }
          if (service_state.current_characteristic == characteristic)
          {
            if (event->evt.gatts_evt.params.authorize_request.request.write.offset + event->evt.gatts_evt.params.authorize_request.request.write.len <= sizeof(buffer_buffer))
            {
              memcpy(buffer_buffer + service_state.length, event->evt.gatts_evt.params.authorize_request.request.write.data, event->evt.gatts_evt.params.authorize_request.request.write.len);
              service_state.length = service_state.length + event->evt.gatts_evt.params.authorize_request.request.write.len;
            }
          }
          else
          {
            service_state.current_characteristic = NULL;
          }
          break;

        default:
          break;
        }

        static const ble_gatts_rw_authorize_reply_params_t reply =
        {
          .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
          .params.write.gatt_status = BLE_GATT_STATUS_SUCCESS
        };
        err_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
        APP_ERROR_CHECK(err_code);
        break;
      }
      else if (event->evt.gatts_evt.params.authorize_request.request.write.handle == 0 && event->evt.gatts_evt.params.authorize_request.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW && service_state.current_characteristic)
      {
        if (service_state.current_characteristic->write && (service_state.current_characteristic->plain || session_isEncrypted()))
        {
          uint16_t blength = 0;
          if (session_writeData(buffer_buffer, service_state.length, buffer_buffer, &blength))
          {
            service_state.current_characteristic->write(buffer_buffer, blength, service_state.current_characteristic);
          }
        }
        service_state.current_characteristic = NULL;

        static const ble_gatts_rw_authorize_reply_params_t reply =
        {
          .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
          .params.write.gatt_status = BLE_GATT_STATUS_SUCCESS
        };
        err_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
        APP_ERROR_CHECK(err_code);
      }
      break;
    }

    case BLE_GATTS_AUTHORIZE_TYPE_READ:
    {
      const service_characteristic_t* characteristic = service_findCharacteristicByHandle(event->evt.gatts_evt.params.authorize_request.request.read.handle);
      if (characteristic)
      {
        if (event->evt.gatts_evt.params.authorize_request.request.read.offset == 0)
        {
          uint16_t blength = 0;
          uint16_t length = 0;
          uint8_t* buffer = NULL;
          if (characteristic->read && (characteristic->plain || session_isEncrypted()))
          {
            characteristic->read(&buffer, &length, characteristic);
            if (buffer && length)
            {
              session_readData(buffer, length, buffer_buffer, &blength);
            }
          }
          ble_gatts_rw_authorize_reply_params_t reply =
          {
            .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
            .params.read =
            {
              .gatt_status = BLE_GATT_STATUS_SUCCESS,
              .update = 1,
              .offset = 0,
              .len = blength,
              .p_data = buffer_buffer
            }
          };
          err_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
          APP_ERROR_CHECK(err_code);
        }
        else
        {
          static const ble_gatts_rw_authorize_reply_params_t reply =
          {
            .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
            .params.read =
            {
              .gatt_status = BLE_GATT_STATUS_SUCCESS,
            }
          };
          err_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
          APP_ERROR_CHECK(err_code);
        }
        break;
      }
      break;
    }

    default:
      break;
    }
    break;
  }

  default:
    break;
  }
}

static const service_characteristic_t* service_findCharacteristicByHandle(uint16_t handle)
{
  for (unsigned i = service_state.mapfree; i--; )
  {
    if (service_state.map[i].handle == handle)
    {
      return service_state.map[i].characteristic;
    }
  }
  return NULL;
}

static uint16_t service_findHandleFromCharacteristic(const service_characteristic_t* characteristic)
{
  for (unsigned i = service_state.mapfree; i--; )
  {
    if (service_state.map[i].characteristic == characteristic)
    {
      return service_state.map[i].handle;
    }
  }
  return BLE_CONN_HANDLE_INVALID;
}

void service_notify(const service_characteristic_t* characteristic)
{
  uint32_t err_code;

  if (characteristic->notify)
  {
    const ble_gatts_hvx_params_t params =
    {
      .handle = service_findHandleFromCharacteristic(characteristic),
      .type = BLE_GATT_HVX_INDICATION,
    };
    err_code = sd_ble_gatts_hvx(service_state.connection_handle, &params);
    APP_ERROR_CHECK(err_code);
  }
}

void service_read_string(uint8_t** p_data, uint16_t* p_length, const service_characteristic_t* characteristic)
{
  *p_data = (uint8_t*)characteristic->ctx;
  *p_length = strlen((char*)characteristic->ctx);
}
