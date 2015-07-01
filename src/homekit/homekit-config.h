/*
 * homekit-config.h
 *
 *  Created on: Jun 24, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_HOMEKIT_CONFIG_H_
#define HOMEKIT_HOMEKIT_CONFIG_H_

#define HOMEKIT_CONFIG_VERSION                          0x01, 0x00

#define HOMEKIT_CONFIG_DEVICE_NAME_STRING               "41:42:43:44:45:46"
#define HOMEKIT_CONFIG_DEVICE_NAME_BYTES                0x41, 0x42, 0x43, 0x44, 0x45, 0x46

#define HOMEKIT_CONFIG_PINCODE                          "987-83-082"

#define HOMEKIT_CONFIG_MANUFACTURER                     "Demo manufacturer"
#define HOMEKIT_CONFIG_MODEL                            "Demo model"
#define HOMEKIT_CONFIG_NAME                             "Demo name"
#define HOMEKIT_CONFIG_SERIAL_NR                        "0123"
#define HOMEKIT_CONFIG_SERVICE_ID                       "1"

#define HOMEKIT_CONFIG_SERVICE_BUFFERSIZE               256
#define HOMEKIT_CONFIG_SERVICE_MAX_CHARACTERISTICS       16

#define HOMEKIT_CONFIG_IDENTIFY_FUNCTION(DATA, LEN, CTX)

#endif /* HOMEKIT_HOMEKIT_CONFIG_H_ */
