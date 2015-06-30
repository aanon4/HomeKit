/*
 * gap.h
 *
 *  Created on: Mar 11, 2015
 *      Author: tim
 */

#ifndef GAP_H_
#define GAP_H_

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS) // Minimum connection interval
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS) // Maximum connection interval
#define SLAVE_LATENCY                   0                               // Slave latency.
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(300, UNIT_10_MS)  // Connection supervisory timeout

#define PRODUCT_NAME                    'H','K', '#'

extern void gap_init(void);

#endif /* GAP_H_ */
