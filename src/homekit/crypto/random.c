/*
 * random.c
 *
 *  Created on: Jun 10, 2015
 *      Author: tim
 */

#include <stdint.h>
#include <string.h>

#include <app_error.h>
#include <nrf_soc.h>

#include "random.h"


void random_create(uint8_t* p_result, uint8_t length)
{
  while (length--)
  {
    *p_result++ = rand(); // Obviously not a good source of random numbers - FIXME
  }
}

void randombytes(uint8_t* p_result, uint64_t length)
{
  random_create(p_result, (uint8_t)length);
}
