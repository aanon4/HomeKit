/*
 * statistics.c
 *
 *  Created on: Apr 25, 2015
 *      Author: tim
 */

#include <nordic_common.h>
#include <ble.h>
#include <ble_gatts.h>
#include <ble_hci.h>
#include <app_error.h>
#include <nrf_soc.h>
#include <app_timer.h>

#include "uuids.h"
#include "statistics.h"

#if defined(INCLUDE_STATISTICS)

struct statistics stats;
struct statistics_timer stats_timer;
static app_timer_id_t statistics_timer;

static void statistics_handler_irq(void* dummy)
{
}

void statistics_init(void)
{
  uint32_t err_code;

  // Need at least one timer running so app_timer_cnt_get returns a non-zero value
  err_code = app_timer_create(&statistics_timer, APP_TIMER_MODE_REPEATED, statistics_handler_irq);
  APP_ERROR_CHECK(err_code);
  err_code = app_timer_start(statistics_timer, APP_TIMER_TICKS(60 * 1000, 32 /*APP_TIMER_PRESCALER */), NULL);
  APP_ERROR_CHECK(err_code);
}

uint32_t statistics_get_time(void)
{
	uint32_t err_code;
	uint32_t ticks;
	err_code = app_timer_cnt_get(&ticks);
	APP_ERROR_CHECK(err_code);
	return ticks;
}

#endif

