/*
 * gpio.c
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#include <app_gpiote.h>

#include "gpio.h"

void gpio_init(void)
{
	APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}

