/*
 * buffer.h
 *
 *  Created on: Jul 12, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_BUFFER_H_
#define HOMEKIT_BUFFER_H_

#include <nordic_common.h>

#include "homekit-config.h"
#include "pairing.h"
#include "session.h"

#define BUFFER_SIZE   MAX(PAIRING_BUFFER_SIZE, SESSION_CIPHER_BUFFERLEN(HOMEKIT_CONFIG_SERVICE_BUFFERSIZE))

extern uint8_t buffer_buffer[BUFFER_SIZE];

#endif /* HOMEKIT_BUFFER_H_ */
