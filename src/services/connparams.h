/*
 * connparams.h
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#ifndef CONNPARAMS_H_
#define CONNPARAMS_H_

#include <ble_conn_params.h>

#define	TX_POWER						4

#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) // Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) // Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT     3 // Number of attempts before giving up the connection parameter negotiation.

extern void conn_params_init(void);

#endif /* CONNPARAMS_H_ */
