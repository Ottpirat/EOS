/*! \file
 *  \brief DCF77 module
 *  
 *  This module provides functions to decode the DCF77 signal and extract the
 *  current date and time.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#ifndef _DCF_H
#define _DCF_H

#include "util.h"

//----------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------

// Pin definitions
#define DCF_SIG_PIN			PA1
#define DCF_GND_PIN			PA3
#define DCF_DDRx			DDRA
#define DCF_PORTx			PORTA
#define DCF_PINx			PINA

// DEBUG
#define DCF_DEBUG_PIN		PA2

// Signal definitions
#define DCF_SYNC_MIN_MS		1700	// Minimum width of the synchronization pulse
#define DCF_SYNC_MAX_MS		2000	// Maximum width of the synchronization pulse
#define DCF_2ND_BIT_OFFSET	1150	// Offset for the second bit in ms
#define DCF_SEP_TICKS		1000	// Time between two bits in ms
#define DCF_SIG_LENGTH_BITS	59		// Length of the DCF77 signal in bits

#define DCF_LOW_MIN_MS		155
#define DCF_LOW_MS			200
#define DCF_LOW_MAX_MS		260

#define DCF_HIGH_MIN_MS		60
#define DCF_HIGH_MS			100
#define DCF_HIGH_MAX_MS		145

// Bit definitions
#define DCF_START_BIT		20
#define DCF_MINUTE_START	21
#define DCF_MINUTE_END		27
#define DCF_HOUR_START		29
#define DCF_HOUR_END		34
#define DCF_DAY_START		36
#define DCF_DAY_END			41
#define DCF_DAYOFWEEK_START	42
#define DCF_DAYOFWEEK_END	44
#define DCF_MONTH_START		45
#define DCF_MONTH_END		49
#define DCF_YEAR_START		50
#define DCF_YEAR_END		57
#define DCF_UNKNOWN_BIT		255

// Constants
#define SECONDS_PER_MINUTE	60
#define MINUTES_PER_HOUR	60
#define HOURS_PER_DAY		24
#define DAYS_PER_WEEK		7
#define DAYS_PER_MONTH		31
#define MONTHS_PER_YEAR		12

//----------------------------------------------------------------------------
// Typedefs & Global variables
//----------------------------------------------------------------------------

typedef struct DcfDateTime
{
    uint8_t  minutes;			// Minutes
    uint8_t  hours;				// Hours

    uint8_t  day;				// Day of month
    uint8_t  month;				// Number of month in current year
    uint16_t year;				// Current year

    uint8_t  dayOfWeek;			// 1=Monday, ..., 7=Sunday

    time_t systemTimeOffset;	// Equivalent timestamp in SystemTime
} dcfDateTime_t;

typedef enum DecodeStatus
{
    DS_WAITING,					// no signal yet
    DS_FAIL_START,				// start bit incorrect
    DS_FAIL_MINUTE_PARITY,		// minute parity incorrect
    DS_FAIL_HOUR_PARITY,		// hour parity incorrect
    DS_FAIL_DATE_PARITY,		// data parity incorrect
    DS_SUCCESS					// success
} decodeStatus_t;

//! Weekday strings
extern PGM_P const strWeekday[];

//! Global variable to store the current date and time
extern dcfDateTime_t dcfDateTime;

//! Information about the success of decoding the signal
extern decodeStatus_t dcf_status;

//----------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------

//! Initializes the DCF module pins
void dcf_init(void);

//! Get the current state of the DCF signal pin
bool dcf_getSignal(void);

//! Receives the DCF signal and decodes it
void dcf_signalHandler(void);

//! Decodes the signal buffer into time and date
void dcf_decodeTime(uint8_t *signalBuffer);

//! Returns pointer to current date time
dcfDateTime_t* dcf_getDateTime();

//! Writes the current time to LCD
void lcd_writeDateTime(dcfDateTime_t *dateTime);

#endif
