/*
 * statistics.h
 *
 *  Created on: Apr 25, 2015
 *      Author: tim
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#if defined(INCLUDE_STATISTICS)

struct statistics
{
  uint32_t  pairing_ms[7];
  uint32_t  verify_ms[5];
};

struct statistics_timer
{
  uint32_t  pairing_ms[7];
  uint32_t  verify_ms[5];
};

extern struct statistics stats;
extern struct statistics_timer stats_timer;

extern void statistics_init(void);
extern uint32_t statistics_get_time(void);

#define	STAT_RECORD_SET(FIELD, VALUE)	stats.FIELD = (VALUE)
#define	STAT_RECORD_ADD(FIELD, VALUE)	stats.FIELD += (VALUE)
#define	STAT_RECORD_INC(FIELD)			stats.FIELD++
#define	STAT_TIMER_START(FIELD)			stats_timer.FIELD = statistics_get_time()
#define	STAT_TIMER_END(FIELD)			do { if (stats_timer.FIELD) stats.FIELD += statistics_get_time() - stats_timer.FIELD; stats_timer.FIELD = 0; } while(0)

#else

#define	STAT_RECORD_SET(FIELD, VALUE)
#define	STAT_RECORD_ADD(FIELD, VALUE)
#define	STAT_RECORD_INC(FIELD)
#define STAT_TIMER_START(FIELD)
#define STAT_TIMER_END(FIELD)

#endif

#endif /* STATISTICS_H_ */
