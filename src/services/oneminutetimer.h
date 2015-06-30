/*
 * oneminutetimer.h
 *
 *  Created on: Apr 2, 2015
 *      Author: tim
 */

#ifndef ONEMINUTETIMER_H_
#define ONEMINUTETIMER_H_

// The one minute timer is use to hang many period things from. This lets us do many
// things when we wake the device up rather than having lots of different timers
// continually waking the device. The timer is 1 minute and SHOULD NOT BE CHANGED
// without checking everyone who uses it.
#define	ONEMINUTE_TIMER_MS		(60 * 1000) // 1 minute

extern void oneminutetimer_init(void);

#endif /* ONEMINUTETIMER_H_ */
