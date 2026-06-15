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
#include "dcf.h"
#include "../os_core.h"

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//! Information about the success of decoding the signal
decodeStatus_t dcf_status = DS_WAITING;

//! Global variable to store the current date and time
dcfDateTime_t dcfDateTime =
{
    .minutes = 0,
    .hours = 0,
    .day = 1,
    .month = 1,
    .year = 2000,
    .dayOfWeek = 1,
    .systemTimeOffset = 0
};

//! Weekday strings
const char strNN[] PROGMEM = "??";
const char strMo[] PROGMEM = "Mo";
const char strTu[] PROGMEM = "Di";
const char strWe[] PROGMEM = "Mi";
const char strTh[] PROGMEM = "Do";
const char strFr[] PROGMEM = "Fr";
const char strSa[] PROGMEM = "Sa";
const char strSu[] PROGMEM = "So";
PGM_P const strWeekday[] = {strNN, strMo, strTu, strWe, strTh, strFr, strSa, strSu};

//----------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------
void static dcf_readPulses(void);

//----------------------------------------------------------------------------
// Helpers
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------------------

/*! 
 * Initializes the DCF module pins
 */
void dcf_init(void)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Get the current state of the DCF signal pin
 */
bool dcf_getSignal(void)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Watch the DCF signal for a sync signal.
 * After sync, call readPulses() to receive and parse the signal.
 * Has to be called frequently to detect the signal.
 */
void dcf_signalHandler(void)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Read a minute of DCF pulses and stores it in the buffer.
 * Then calls dcf_decodeTime() to decode the signal.
 */
void static dcf_readPulses(void)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Decodes the signal buffer into time and date.
 * Checks the parity of minute, hour and date.
 * If the parity is incorrect, do not update the dateTime.
 * Also sets the dcf_status accordingly.
 *
 * \param signalBuffer The buffer containing the DCF signal
 */
void dcf_decodeTime(uint8_t *signalBuffer)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Returns pointer to current date time
 */
dcfDateTime_t *dcf_getDateTime()
{
	return &dcfDateTime;
}

/*!
 * Writes the current time to LCD
 * Uses format:
 * 
 * HH:MM:SS
 * dd. DD.MM.YYYY e
 * 
 * Where:
 * - HH:MM:SS is the current time
 * - dd is the short form of the day of the week (Mo, Di, Mi, Do, Fr, Sa, So)
 * - DD.MM.YYYY is the current date
 * - e is displayed as an exclamation mark if the last decode was unsuccessful.
 */
void lcd_writeDateTime(dcfDateTime_t *dateTime)
{
	#warning [Praktikum 5] Implement here
}
