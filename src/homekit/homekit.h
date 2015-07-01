/*
 * homekit.h
 *
 *  Created on: Jun 27, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_HOMEKIT_H_
#define HOMEKIT_HOMEKIT_H_

extern void homekit_init(void);
extern void homekit_ble_event(ble_evt_t* p_ble_evt);
extern void homekit_execute(void);

#endif /* HOMEKIT_HOMEKIT_H_ */
