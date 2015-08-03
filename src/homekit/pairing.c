/*
 * pairing.c
 *
 *  Created on: Jun 8, 2015
 *      Author: tim
 */

#include <string.h>

#include <ble.h>
#include <ble_gatts.h>
#include "ble_hci.h"
#include <app_error.h>
#include <nordic_common.h>

#include "services/advertising.h"
#include "homekit-config.h"
#include "uuids.h"
#include "pairing.h"
#include "tlv.h"
#include "session.h"
#include "buffer.h"
#include "crypto/crypto.h"
#include "statistics.h"


static Pairing_Status pairing_process(Pairing_Event event, uint8_t* data, uint16_t olength, uint16_t* rlength);
static void pairing_send_auth_write_reply(void);
static void pairing_error(void);
static Pairing_Event pairing_map_write_event(uint16_t hand);
static Pairing_Event pairing_map_read_event(uint16_t hand);

static const uint8_t pairing_device_name[17] = HOMEKIT_CONFIG_DEVICE_NAME_STRING;

static struct
{
  Pairing_Event pending_event;
  uint8_t sent_reply;
  uint16_t length;
} pairing_data;

static struct
{
  uint16_t  connection;
  uint16_t  pairsetup;
  uint16_t  pairverify;
  uint16_t  pairings;
} pairing_handle;

static uint8_t pairing_state;
static uint8_t pairing_justdisconnected;


void pairing_init(void)
{
  uint32_t err_code;

  crypto_init();

  static const uint8_t features = HOMEKIT_CONFIG_MFI_CERTIFIED;
  static const uint8_t id[] = HOMEKIT_CONFIG_PAIRING_ID;
  static const struct
  {
    const uint16_t  id;
    const char*     name;
    uint8_t*        data;
    const uint16_t  dlen;
    const uint8_t   read:1;
    const uint8_t   write:1;
    const uint8_t   read_auth:1;
    const uint8_t   write_auth:1;
    uint16_t*       handle;
  } init[] =
  {
    { HOMEKIT_SERVICE_ID,   "95;", (uint8_t*)id, sizeof(id) - 1,           .read = 1 },
    { HOMEKIT_PAIR_FEATURES,"96;", (uint8_t*)&features, sizeof(features),  .read = 1 },
    { HOMEKIT_PAIR_SETUP,   "97;", buffer_buffer, PAIRING_SETUP_MAX_SIZE,  .read = 1, .write = 1, .read_auth = 1, .write_auth = 1, .handle = &pairing_handle.pairsetup },
    { HOMEKIT_PAIR_VERIFY,  "98;", buffer_buffer, PAIRING_VERIFY_MAX_SIZE, .read = 1, .write = 1, .read_auth = 1, .write_auth = 1, .handle = &pairing_handle.pairverify },
    { HOMEKIT_PAIRINGS,     "99;", buffer_buffer, PAIRING_PAIRS_MAX_SIZE,  .read = 1, .write = 1, .read_auth = 1, .write_auth = 1, .handle = &pairing_handle.pairings },
    {}
  };

  static const ble_uuid_t pairing_service_uuid =
  {
    .type = BLE_UUID_TYPE_BLE,
    .uuid = HOMEKIT_SERVICE_UUID_PAIRING
  };
  uint16_t pairing_service_handle;
  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &pairing_service_uuid, &pairing_service_handle);
  APP_ERROR_CHECK(err_code);

  for (uint8_t idx = 0; init[idx].id; idx++)
  {
    const ble_gatts_attr_md_t metadata =
    {
      .read_perm = { init[idx].read, init[idx].read },
      .write_perm = { init[idx].write, init[idx].write },
      .rd_auth = init[idx].read_auth,
      .wr_auth = init[idx].write_auth,
      .vlen = init[idx].read_auth ? 1 : 0,
      .vloc = init[idx].data ? BLE_GATTS_VLOC_USER : BLE_GATTS_VLOC_STACK
    };
    const ble_uuid_t uuid =
    {
      .type = HOMEKIT_BASE_TYPE,
      .uuid = init[idx].id
    };
    const ble_gatts_attr_t attr =
    {
      .p_uuid = (ble_uuid_t*)&uuid,
      .p_attr_md = (ble_gatts_attr_md_t*)&metadata,
      .init_len = init[idx].dlen,
      .max_len = init[idx].dlen,
      .p_value = init[idx].data
    };
    const ble_gatts_char_md_t characteristic =
    {
      .char_props.read = init[idx].read,
      .char_props.write = init[idx].write,
      .p_char_user_desc = (uint8_t*)init[idx].name,
      .char_user_desc_max_size = strlen(init[idx].name),
      .char_user_desc_size = strlen(init[idx].name)
    };

    ble_gatts_char_handles_t newhandle;
    err_code = sd_ble_gatts_characteristic_add(pairing_service_handle, &characteristic, &attr, &newhandle);
    APP_ERROR_CHECK(err_code);
    if (init[idx].handle)
    {
      *init[idx].handle = newhandle.value_handle;
    }
  }
}

void pairing_ble_event(ble_evt_t* event)
{
  uint32_t err_code;

  switch (event->header.evt_id)
  {
  case BLE_GAP_EVT_CONNECTED:
    pairing_handle.connection = event->evt.gap_evt.conn_handle;
    pairing_state = 0;
    break;

  case BLE_GAP_EVT_DISCONNECTED:
    pairing_handle.connection = BLE_CONN_HANDLE_INVALID;
    session_setEncryption(0);
    pairing_justdisconnected = 1;
    break;

  case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
    if (pairing_handle.connection == event->evt.gatts_evt.conn_handle)
    {
      switch (event->evt.gatts_evt.params.authorize_request.type)
      {
      case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
        pairing_data.sent_reply = 0;
        Pairing_Event pevent = pairing_map_write_event(event->evt.gatts_evt.params.authorize_request.request.write.handle);
        if (pevent != PAIRING_EVENT_UNKNOWN)
        {
          pairing_data.pending_event = pevent;
          switch (event->evt.gatts_evt.params.authorize_request.request.write.op)
          {
          case BLE_GATTS_OP_WRITE_REQ:
            if (pairing_process(pairing_data.pending_event, event->evt.gatts_evt.params.authorize_request.request.write.data, event->evt.gatts_evt.params.authorize_request.request.write.len, NULL) != PAIRING_STATUS_OK)
            {
              pairing_error();
              break;
            }
            pairing_send_auth_write_reply();
            break;

          case BLE_GATTS_OP_PREP_WRITE_REQ:
            if (event->evt.gatts_evt.params.authorize_request.request.write.offset + event->evt.gatts_evt.params.authorize_request.request.write.len <= sizeof(buffer_buffer))
            {
              memcpy(buffer_buffer + event->evt.gatts_evt.params.authorize_request.request.write.offset, event->evt.gatts_evt.params.authorize_request.request.write.data, event->evt.gatts_evt.params.authorize_request.request.write.len);
              pairing_data.length = event->evt.gatts_evt.params.authorize_request.request.write.offset + event->evt.gatts_evt.params.authorize_request.request.write.len;
              pairing_send_auth_write_reply();
            }
            break;

          default:
            break;
          }
        }
        else if (event->evt.gatts_evt.params.authorize_request.request.write.handle == 0 && event->evt.gatts_evt.params.authorize_request.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)
        {
          if (pairing_process(pairing_data.pending_event, buffer_buffer, pairing_data.length, NULL) != PAIRING_STATUS_OK)
          {
            pairing_error();
            break;
          }
          pairing_send_auth_write_reply();
        }
        break;

      case BLE_GATTS_AUTHORIZE_TYPE_READ:
        pevent = pairing_map_read_event(event->evt.gatts_evt.params.authorize_request.request.read.handle);
        if (pevent != PAIRING_EVENT_UNKNOWN)
        {
          if (event->evt.gatts_evt.params.authorize_request.request.read.offset == 0)
          {
            if (pairing_process(pevent, buffer_buffer, sizeof(buffer_buffer), &pairing_data.length) != PAIRING_STATUS_OK)
            {
              pairing_error();
              break;
            }
          }
          if (pairing_handle.connection != BLE_CONN_HANDLE_INVALID)
          {
            ble_gatts_rw_authorize_reply_params_t reply =
            {
              .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
              .params.read =
              {
                .gatt_status = BLE_GATT_STATUS_SUCCESS,
                .update = 1,
                .offset = 0,
                .len = pairing_data.length,
                .p_data = buffer_buffer
              }
            };
            err_code = sd_ble_gatts_rw_authorize_reply(pairing_handle.connection, &reply);
            APP_ERROR_CHECK(err_code);
          }
        }
        break;

      default:
        break;
      }
    }
    break;

  case BLE_EVT_USER_MEM_REQUEST:
    if (pairing_handle.connection == event->evt.gatts_evt.conn_handle)
    {
      if (event->evt.common_evt.params.user_mem_request.type == BLE_USER_MEM_TYPE_GATTS_QUEUED_WRITES)
      {
        err_code = sd_ble_user_mem_reply(pairing_handle.connection, NULL);
        APP_ERROR_CHECK(err_code);
      }
    }
    break;

  default:
    break;
  }
}

void pairing_execute(void)
{
  if (pairing_justdisconnected)
  {
    pairing_justdisconnected = 0;
    crypto_storeKeys();
    advertising_start();
  }
}

static void pairing_send_auth_write_reply(void)
{
  uint32_t err_code;

  if (!pairing_data.sent_reply && pairing_handle.connection != BLE_CONN_HANDLE_INVALID)
  {
    pairing_data.sent_reply = 1;
    static const ble_gatts_rw_authorize_reply_params_t reply =
    {
      .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
      .params.write.gatt_status = BLE_GATT_STATUS_SUCCESS
    };
    err_code = sd_ble_gatts_rw_authorize_reply(pairing_handle.connection, &reply);
    APP_ERROR_CHECK(err_code);
  }
}

static void pairing_error(void)
{
  uint32_t err_code;

  err_code = sd_ble_gap_disconnect(pairing_handle.connection, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
  APP_ERROR_CHECK(err_code);
  pairing_handle.connection = BLE_CONN_HANDLE_INVALID;
}

static Pairing_Event pairing_map_write_event(uint16_t hand)
{
  if (hand == pairing_handle.pairsetup)
  {
    return PAIRING_EVENT_SETUP_WRITE;
  }
  else if (hand == pairing_handle.pairverify)
  {
    return PAIRING_EVENT_VERIFY_WRITE;
  }
  else if (hand == pairing_handle.pairings)
  {
    return PAIRING_EVENT_PAIRINGS_WRITE;
  }
  else
  {
    return PAIRING_EVENT_UNKNOWN;
  }
}

static Pairing_Event pairing_map_read_event(uint16_t hand)
{
  if (hand == pairing_handle.pairsetup)
  {
    return PAIRING_EVENT_SETUP_READ;
  }
  else if (hand == pairing_handle.pairverify)
  {
    return PAIRING_EVENT_VERIFY_READ;
  }
  else if (hand == pairing_handle.pairings)
  {
    return PAIRING_EVENT_PAIRINGS_READ;
  }
  else
  {
    return PAIRING_EVENT_UNKNOWN;
  }
}

static Pairing_Status pairing_process(Pairing_Event event, uint8_t* data, uint16_t olength, uint16_t* rlength)
{
  Pairing_Status status = PAIRING_STATUS_OK;

  if (rlength)
  {
    *rlength = olength;
  }

  switch (event)
  {
  case PAIRING_EVENT_SETUP_WRITE:
  {
    uint8_t type;
    uint16_t length;
    uint8_t* value;
    while (status == PAIRING_STATUS_OK && tlv_decode_next(&data, &olength, &type, &length, &value))
    {
      switch (type)
      {
      case PAIRING_TAG_TYPE:
        break;

      case PAIRING_TAG_STATE:
        pairing_state = length == 1 ? value[0] : 0;
        STAT_TIMER_START(pairing_ms[pairing_state]);
        if (pairing_state == 1)
        {
          srp_start();
        }
        break;

      case PAIRING_TAG_SRP_A:
        if (!srp_setA(value, length, pairing_send_auth_write_reply))
        {
          status = PAIRING_STATUS_ERROR;
        }
        break;

      case PAIRING_TAG_SRP_M1:
        if (!srp_checkM1(value, length))
        {
          status = PAIRING_STATUS_ERROR;
        }
        break;

      case PAIRING_TAG_MSG:
      {
        uint8_t output_key[64];
        crypto_hkdf(output_key, "Pair-Setup-Encrypt-Salt", 23, "Pair-Setup-Encrypt-Info\001", 24, srp_getK(), 64);
        if (crypto_verifyAndDecrypt(output_key, "PS-Msg05", value, length - 16, value, value + length - 16))
        {
          uint16_t blength = length - 16;
          uint8_t* buf = value;
          uint8_t* client = NULL;
          uint8_t* signature = NULL;
          uint8_t* ltpk = NULL;
          while (status == PAIRING_STATUS_OK && tlv_decode_next(&buf, &blength, &type, &length, &value))
          {
            switch (type)
            {
            case PAIRING_MSG_CLIENTNAME:
              if (length == 36)
              {
                client = value;
              }
              break;

            case PAIRING_MSG_LTPK:
              if (length == 32)
              {
                ltpk = value;
              }
              break;

            case PAIRING_MSG_SIGNATURE:
              if (length == 64)
              {
                signature = value;
              }
              break;

            default:
              status = PAIRING_STATUS_ERROR;
              break;
            }
          }
          if (status == PAIRING_STATUS_OK && client && signature && ltpk)
          {
            uint8_t message[64 + 32 + 36 + 32];
            memcpy(message, signature, 64);
            crypto_hkdf(message + 64, "Pair-Setup-Controller-Sign-Salt", 31, "Pair-Setup-Controller-Sign-Info\001", 32, srp_getK(), 64);
            memcpy(message + 64 + 32, client, 36);
            memcpy(message + 64 + 32 + 36, ltpk, 32);

            uint8_t result[sizeof(message)];
            uint64_t rlen = 0;
            if (crypto_sign_open(result, &rlen, message, sizeof(message), ltpk) != 0)
            {
              status = PAIRING_STATUS_ERROR;
            }
            else
            {
              memcpy(crypto_keys.client.ltpk, ltpk, sizeof(crypto_keys.client.ltpk));
              memcpy(crypto_keys.client.name, client, 36);
              crypto_scheduleStoreKeys();
            }
          }
        }
        else
        {
          // Error
          status = PAIRING_STATUS_ERROR;
        }
        break;
      }
      default:
        status = PAIRING_STATUS_ERROR;
        break;
      }
    }
    STAT_TIMER_END(pairing_ms[pairing_state]);
    break;
  }
  case PAIRING_EVENT_SETUP_READ:
  {
    switch (pairing_state)
    {
    case 0:
      break;

    case 1:
    {
      pairing_state = 2;
      STAT_TIMER_START(pairing_ms[pairing_state]);

      tlv_encode_next(&data, rlength, PAIRING_TAG_STATE, sizeof(pairing_state), &pairing_state);
      tlv_encode_next(&data, rlength, PAIRING_TAG_SALT, 16, srp_getSalt());
      tlv_encode_next(&data, rlength, PAIRING_TAG_SRP_B, 384, srp_getB());
      break;
    }
    case 3:
    {
      pairing_state = 4;
      STAT_TIMER_START(pairing_ms[pairing_state]);

      tlv_encode_next(&data, rlength, PAIRING_TAG_STATE, sizeof(pairing_state), &pairing_state);
      tlv_encode_next(&data, rlength, PAIRING_TAG_SRP_M2, 64, srp_getM2());

      // MFi
      // Looks like this payload gets wrapped in MFi information when MFi is enabled. The packet is prefixed
      // with 0x0C 0xFF and postfixed with extra data, possibly some sort of signing info to prove the device
      // is MFi enabled.
      break;
    }

    case 5:
    {
      pairing_state = 6;
      STAT_TIMER_START(pairing_ms[pairing_state]);

      uint8_t smessage[64 + 32 + sizeof(pairing_device_name) + 32];
      crypto_hkdf(smessage + 64, "Pair-Setup-Accessory-Sign-Salt", 30, "Pair-Setup-Accessory-Sign-Info\001", 31, srp_getK(), 64);
      memcpy(smessage + 64 + 32, pairing_device_name, sizeof(pairing_device_name));
      memcpy(smessage + 64 + 32 + sizeof(pairing_device_name), crypto_keys.sign.public, sizeof(crypto_keys.sign.public));
      uint64_t slen = 0;
      crypto_sign(smessage, &slen, smessage + 64, sizeof(smessage) - 64, crypto_keys.sign.secret);

      uint8_t buffer[2 + sizeof(pairing_device_name) + 2 + 32 + 2 + 64 + 16];
      uint8_t* pbuffer = buffer;
      uint16_t lbuffer = sizeof(buffer);
      tlv_encode_next(&pbuffer, &lbuffer, PAIRING_MSG_CLIENTNAME, sizeof(pairing_device_name), pairing_device_name);
      tlv_encode_next(&pbuffer, &lbuffer, PAIRING_MSG_LTPK, 32, crypto_keys.sign.public);
      tlv_encode_next(&pbuffer, &lbuffer, PAIRING_MSG_SIGNATURE, 64, smessage);

      uint8_t output_key[64];
      crypto_hkdf(output_key, "Pair-Setup-Encrypt-Salt", 23, "Pair-Setup-Encrypt-Info\001", 24, srp_getK(), 64);
      crypto_encryptAndSeal(output_key, "PS-Msg06", buffer, sizeof(buffer) - 16, buffer, buffer + sizeof(buffer) - 16);
      tlv_encode_next(&data, rlength, PAIRING_TAG_STATE, sizeof(pairing_state), &pairing_state);
      tlv_encode_next(&data, rlength, PAIRING_TAG_MSG, sizeof(buffer), buffer);
      break;
    }

    default:
      status = PAIRING_STATUS_ERROR;
      break;
    }
    STAT_TIMER_END(pairing_ms[pairing_state]);
    break;
  }

  case PAIRING_EVENT_VERIFY_READ:
  {
    switch (pairing_state)
    {
    case 0:
      break;

    case 1:
    {
      pairing_state = 2;
      STAT_TIMER_START(verify_ms[pairing_state]);

      uint8_t smessage[64 + sizeof(session_keys.verify.public) + sizeof(pairing_device_name) + sizeof(session_keys.client.public)];
      memcpy(smessage + 64, session_keys.verify.public, sizeof(session_keys.verify.public));
      memcpy(smessage + 64 + sizeof(session_keys.verify.public), pairing_device_name, sizeof(pairing_device_name));
      memcpy(smessage + 64 + sizeof(session_keys.verify.public) + sizeof(pairing_device_name), session_keys.client.public, sizeof(session_keys.client.public));
      uint64_t slen = 0;
      crypto_sign(smessage, &slen, smessage + 64, sizeof(smessage) - 64, crypto_keys.sign.secret);

      uint8_t buffer[2 + sizeof(pairing_device_name) + 2 + 64 + 16];
      uint8_t* pbuffer = buffer;
      uint16_t lbuffer = sizeof(buffer);
      tlv_encode_next(&pbuffer, &lbuffer, PAIRING_MSG_CLIENTNAME, sizeof(pairing_device_name), pairing_device_name);
      tlv_encode_next(&pbuffer, &lbuffer, PAIRING_MSG_SIGNATURE, 64, smessage);

      crypto_hkdf(session_keys.pve, "Pair-Verify-Encrypt-Salt", 24, "Pair-Verify-Encrypt-Info\001", 25, session_keys.shared, sizeof(session_keys.shared));
      crypto_encryptAndSeal(session_keys.pve, "PV-Msg02", buffer, sizeof(buffer) - 16, buffer, buffer + sizeof(buffer) - 16);

      tlv_encode_next(&data, rlength, PAIRING_TAG_STATE, sizeof(pairing_state), &pairing_state);
      tlv_encode_next(&data, rlength, PAIRING_TAG_MSG, sizeof(buffer), buffer);
      tlv_encode_next(&data, rlength, PAIRING_TAG_PUBLICKEY, sizeof(session_keys.verify.public), session_keys.verify.public);
      break;
    }

    case 3:
    {
      pairing_state = 4;
      STAT_TIMER_START(verify_ms[pairing_state]);

      tlv_encode_next(&data, rlength, PAIRING_TAG_STATE, sizeof(pairing_state), &pairing_state);
      break;
    }

    default:
      status = PAIRING_STATUS_ERROR;
      break;
    }
    STAT_TIMER_END(verify_ms[pairing_state]);
    break;
  }

  case PAIRING_EVENT_VERIFY_WRITE:
  {
    uint8_t type;
    uint16_t length;
    uint8_t* value;
    while (status == PAIRING_STATUS_OK && tlv_decode_next(&data, &olength, &type, &length, &value))
    {
      switch (type)
      {
      case PAIRING_TAG_STATE:
        pairing_state = length == 1 ? value[0] : 0;
        STAT_TIMER_START(verify_ms[pairing_state]);
        if (pairing_state == 1)
        {
          session_init();
        }
        break;

      case PAIRING_TAG_PUBLICKEY:
      {
        if (length == 32)
        {
          memcpy(session_keys.client.public, value, sizeof(session_keys.client.public));
          crypto_scalarmult(session_keys.shared, session_keys.verify.secret, session_keys.client.public);
        }
        break;
      }

      case PAIRING_TAG_MSG:
      {
        if (crypto_verifyAndDecrypt(session_keys.pve, "PV-Msg03", value, length - 16, value, value + length - 16))
        {
          uint16_t blength = length - 16;
          uint8_t* buf = value;
          uint8_t* client = NULL;
          uint8_t* signature = NULL;
          while (status == PAIRING_STATUS_OK && tlv_decode_next(&buf, &blength, &type, &length, &value))
          {
            switch (type)
            {
            case PAIRING_MSG_CLIENTNAME:
              if (length == 36)
              {
                client = value;
              }
              break;

            case PAIRING_MSG_SIGNATURE:
              if (length == 64)
              {
                signature = value;
              }
              break;

            default:
              status = PAIRING_STATUS_ERROR;
              break;
            }
          }
          if (status == PAIRING_STATUS_OK && client && signature)
          {
            if (memcmp(crypto_keys.client.name, client, 36) != 0)
            {
              status = PAIRING_STATUS_ERROR;
            }
            else
            {
              uint8_t message[64 + sizeof(session_keys.client.public) + 36 + sizeof(session_keys.verify.public)];

              memcpy(message, signature, 64);
              memcpy(message + 64, session_keys.client.public, sizeof(session_keys.client.public));
              memcpy(message + 64 + sizeof(session_keys.client.public), client, 36);
              memcpy(message + 64 + sizeof(session_keys.client.public) + 36, session_keys.verify.public, sizeof(session_keys.verify.public));

              // Reply now to avoid timeout
              pairing_send_auth_write_reply();

              uint8_t result[sizeof(message)];
              uint64_t rlen = 0;
              if (crypto_sign_open(result, &rlen, message, sizeof(message), crypto_keys.client.ltpk) != 0)
              {
                status = PAIRING_STATUS_ERROR;
              }
              else
              {
                session_setEncryption(1);
                STAT_TIMER_END(verify_ms[pairing_state]);
                pairing_state = 3;
              }
            }
          }
        }
        break;
      }

      default:
        status = PAIRING_STATUS_ERROR;
        break;
      }
    }
    STAT_TIMER_END(verify_ms[pairing_state]);
    break;
  }

  case PAIRING_EVENT_PAIRINGS_WRITE:
  {
    uint16_t blength = 0;
    if (session_isEncrypted() && session_writeData(data, olength, data, &blength))
    {
      uint8_t type;
      uint16_t length;
      uint8_t* value;
      uint8_t op = 0;
      uint8_t* clientname = NULL;

      while (status == PAIRING_STATUS_OK && tlv_decode_next(&data, &blength, &type, &length, &value))
      {
        switch (type)
        {
        case PAIRING_TAG_STATE:
          pairing_state = length == 1 ? value[0] : 0;
          break;

        case PAIRING_TAG_TYPE:
          if (length == 1)
          {
            op = value[0];
          }
          break;

        case PAIRING_TAG_CLIENTNAME:
          if (length == 36)
          {
            clientname = value;
          }
          break;

        default:
          status = PAIRING_STATUS_ERROR;
          break;
        }
      }
      if (status == PAIRING_STATUS_OK && op && clientname)
      {
        switch (op)
        {
        case 4: // Remove entry
          if (memcmp(clientname, crypto_keys.client.name, sizeof(crypto_keys.client.name)) == 0)
          {
            memset(crypto_keys.client.name, 0, sizeof(crypto_keys.client.name));
            memset(crypto_keys.client.ltpk, 0, sizeof(crypto_keys.client.ltpk));
            crypto_scheduleStoreKeys();
          }
          break;

        default:
          break;
        }
      }
    }
    else
    {
      status = PAIRING_STATUS_ERROR;
    }
    break;
  }

  case PAIRING_EVENT_PAIRINGS_READ:
  {
    switch (pairing_state)
    {
    case 0:
      break;

    case 1:
      pairing_state = 2;
      if (session_isEncrypted())
      {
        static const uint8_t status[] = { PAIRING_TAG_STATE, 1, 2 };
        uint16_t slength = 0;
        session_readData((uint8_t*)status, sizeof(status), data, &slength);
        *rlength -= slength;
      }
      break;

    default:
      status = PAIRING_STATUS_ERROR;
      break;
    }
    break;
  }

  default:
    status = PAIRING_STATUS_ERROR;
    break;
  }

  if (rlength)
  {
    *rlength = olength - *rlength;
  }

  return status;
}
