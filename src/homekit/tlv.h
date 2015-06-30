/*
 * tlv.h
 *
 *  Created on: Jun 9, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_TLV_H_
#define HOMEKIT_TLV_H_

extern uint8_t tlv_decode_next(uint8_t** data, uint16_t* dlength, uint8_t* type, uint16_t* length, uint8_t** value);
extern uint8_t tlv_encode_next(uint8_t** data, uint16_t* dlength, uint8_t type, uint16_t length, const uint8_t* value);

#endif /* HOMEKIT_TLV_H_ */
