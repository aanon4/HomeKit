/*
 * scheduler.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <softdevice_handler.h>
#include <app_timer.h>

#include "scheduler.h"

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, BLE_STACK_HANDLER_SCHED_EVT_SIZE) //  Maximum size of scheduler events.


void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
