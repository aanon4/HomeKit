/*
 * advertising.h
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#ifndef ADVERTISING_H_
#define ADVERTISING_H_

#define APP_ADV_INTERVAL                   1000 // The advertising interval in ms
#define APP_ADV_TIMEOUT_IN_SECONDS            0 // No advertising timeout

extern void advertising_init(void);
extern void advertising_start(void);
extern void advertising_stop(void);

#endif /* ADVERTISING_H_ */
