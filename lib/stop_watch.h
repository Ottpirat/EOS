/*! \file  stop_watch.h
 *  \brief Stopwatch to measure time
 *
 *  It can measure up to 32ms, measuring more will crash the micro controller.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2024
 *  \version  1.0
 */

#ifndef __STOP_WATCH_H__
#define __STOP_WATCH_H__

#include "util.h"

typedef time_t stop_watch_handler_t;

//! Initializes the stop watch
void stopWatch_init(void);

//! Starts the stop watch and returns a handler with that you'd retrieve your measurement later
stop_watch_handler_t stopWatch_start(void);

//! Measures the time since the stop watch was started
time_t stopWatch_measure(stop_watch_handler_t stopWatchHandler);

//! Measures the time in micro seconds that elapsed since the handler was created and stops it
time_t stopWatch_stop(stop_watch_handler_t stopWatchHandler);

#endif // __STOP_WATCH_H__