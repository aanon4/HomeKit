/*
 * pairing.h
 *
 *  Created on: Jun 8, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_PAIRING_H_
#define HOMEKIT_PAIRING_H_

#include <ble.h>

typedef enum
{
  PAIRING_EVENT_SETUP_READ = 1,
  PAIRING_EVENT_SETUP_WRITE,
  PAIRING_EVENT_VERIFY_READ,
  PAIRING_EVENT_VERIFY_WRITE,
  PAIRING_EVENT_PAIRINGS_READ,
  PAIRING_EVENT_PAIRINGS_WRITE,
  PAIRING_EVENT_UNKNOWN
} Pairing_Event;

typedef enum
{
  PAIRING_TAG_TYPE = 0, // 1-byte
  PAIRING_TAG_CLIENTNAME = 1, //
  PAIRING_TAG_SALT = 2, // 16-bytes
  PAIRING_TAG_SRP_A = 3, // 384-bytes
  PAIRING_TAG_SRP_B = 3, // 384-bytes
  PAIRING_TAG_PUBLICKEY = 3, // 32-bytes
  PAIRING_TAG_SRP_M1 = 4, // 32-bytes
  PAIRING_TAG_SRP_M2 = 4, // 32-bytes
  PAIRING_TAG_MSG = 5, // Variable
  PAIRING_TAG_STATE = 6, // 1-byte
  PAIRING_TAG_ERROR = 7
} Pairing_Tag;

typedef enum
{
  PAIRING_MSG_CLIENTNAME = 1, // 36 bytes (UUID)
  PAIRING_MSG_LTPK = 3, // 32 bytes
  PAIRING_MSG_SIGNATURE = 10 // 64 bytes
} Pairing_Msg;

typedef enum
{
  PAIRING_STATUS_OK = 0,
  PAIRING_STATUS_ERROR,
  PAIRING_STATUS_BADLENGTH
} Pairing_Status;

extern void pairing_init(void);
extern void pairing_ble_event(ble_evt_t* event);
extern void pairing_execute(void);

#endif /* HOMEKIT_PAIRING_H_ */
