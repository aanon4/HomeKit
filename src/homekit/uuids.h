/*
 * uuids.h
 *
 *  Created on: Mar 17, 2015
 *      Author: tim
 */

#ifndef UUIDS_H_
#define UUIDS_H_


#define HOMEKIT_BASE_UUID           0x91, 0x52, 0x76, 0xBB, 0x26, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, ( 0 ), ( 0 ), 0x00, 0x00
#define HOMEKIT_BASE_TYPE           2


#define HOMEKIT_SERVICE_UUID_FAN            0xFECB
#define HOMEKIT_SERVICE_UUID_OUTLET         0xFECC
#define HOMEKIT_SERVICE_UUID_SWITCH         0xFECD
#define HOMEKIT_SERVICE_UUID_GARAGEDOOR     0xFECE
#define HOMEKIT_SERVICE_UUID_LOCK_MNGMNT    0xFECF
#define HOMEKIT_SERVICE_UUID_LOCK_MECH      0xFED0
#define HOMEKIT_SERVICE_UUID_THERMOSTAT     0xFED1
#define HOMEKIT_SERVICE_UUID_LIGHTBULB      0xFED2
#define HOMEKIT_SERVICE_UUID_ACCESSORY_INFO 0xFED3
#define HOMEKIT_SERVICE_UUID_PAIRING        0xFED4

#define HOMEKIT_ADMIN_ONLY                  0x0001
#define HOMEKIT_AUDIO_FEEDBACK              0x0005
#define HOMEKIT_BRIGHTNESS                  0x0008
#define HOMEKIT_COOLING_THRHLD              0x000D
#define HOMEKIT_DOOR_STATE                  0x000E
#define HOMEKIT_HEATING_MODE                0x000F
#define HOMEKIT_HUMIDITY                    0x0010
#define HOMEKIT_TEMPERATURE                 0x0011
#define HOMEKIT_HEATING_THRHLD              0x0012
#define HOMEKIT_HUE                         0x0013
#define HOMEKIT_IDENTITY                    0x0014
#define HOMEKIT_LOCK_CTRL_POINT             0x0019
#define HOMEKIT_LOCK_AUTO_TIMEOUT           0x001A
#define HOMEKIT_LOCK_LAST_ACTION            0x001C
#define HOMEKIT_LOCK_STATE                  0x001D
#define HOMEKIT_LOCK_TARGET_STATE           0x001E
#define HOMEKIT_LOGS                        0x001F
#define HOMEKIT_MANUFACTURER                0x0020
#define HOMEKIT_MODEL                       0x0021
#define HOMEKIT_MOTION_DETECTED             0x0022
#define HOMEKIT_NAME                        0x0023
#define HOMEKIT_OBSTRUCT_DETECTED           0x0024
#define HOMEKIT_SWITCH_ON                   0x0025
#define HOMEKIT_OUTLET_INUSE                0x0026
#define HOMEKIT_ROTATION_DIR                0x0028
#define HOMEKIT_ROTATION_SPEED              0x0029
#define HOMEKIT_SATURATION                  0x002F
#define HOMEKIT_SERIAL_NR                   0x0030
#define HOMEKIT_TARGET_DOOR_STATE           0x0032
#define HOMEKIT_TARGET_HEAT_MODE            0x0033
#define HOMEKIT_TARGET_HUMIDITY             0x0034
#define HOMEKIT_TARGET_TEMPERATURE          0x0035
#define HOMEKIT_TEMPERATURE_UNITS           0x0036
#define HOMEKIT_VERSION                     0x0037

#define HOMEKIT_PAIR_SETUP                  0x004C
#define HOMEKIT_PAIR_VERIFY                 0x004E
#define HOMEKIT_PAIR_FEATURES               0x004F
#define HOMEKIT_PAIRINGS                    0x0050
#define HOMEKIT_SERVICE_ID                  0x0051
#define HOMEKIT_FIRMWARE_REV                0x0052
#define HOMEKIT_HARDWARE_REV                0x0053
#define HOMEKIT_SOFTWARE_REV                0x0054


extern void uuids_init(void);

#endif /* UUIDS_H_ */
