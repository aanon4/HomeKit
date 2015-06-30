/*
 * i2c.h
 *
 *  Created on: Mar 16, 2015
 *      Author: tim
 */

#ifndef I2C_H_
#define I2C_H_

#include <twi_master.h>

static bool __INLINE i2c_init(void)
{
	return twi_master_init();
}

static bool __INLINE i2c_read(uint8_t address, uint8_t *data, uint8_t data_length, bool issue_stop_condition)
{
	return twi_master_transfer(address | TWI_READ_BIT, data, data_length, issue_stop_condition);
}

static bool __INLINE i2c_write(uint8_t address, uint8_t *data, uint8_t data_length, bool issue_stop_condition)
{
	return twi_master_transfer(address, data, data_length, issue_stop_condition);
}

#endif /* I2C_H_ */
