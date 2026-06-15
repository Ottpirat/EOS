#include "../progs.h"
#if (defined(USER_PROGRAM_ENABLED) || TESTTASK == TT_DCF_GENERATOR) && USER_PROGRAM == USER_PROGRAM_DCF_CLOCK

#include "../../lib/util.h"
#include "../../lib/tempSensor.h"
#include "../../lib/dcf.h"
#include "../../lib/buttons.h"
#include "../../os_core.h"
#include "../../os_process.h"
#include "../../memory/os_memory.h"

#define PROG_INIT_AND_PRINT_TEMP	1
#define PROG_POLL_DCF				2
#define PROG_PRINT_TIME				3

#define DRIVER						intHeap
#define TEMP_HISTORY_SIZE			512

/*!
 * Init DCF, start other processes. Init ADC and read the temperature.
 * Displays the current temperature or statistics in °C on the LCD.
 */
PROGRAM(PROG_INIT_AND_PRINT_TEMP, AUTOSTART)
{
	#warning [Praktikum 5] Implement here

	// Hint: Use DPRR for priority scheduling

	// Hint: Start the signal handler with high priority

	// Hint: Start the date and time display with normal priority

}

/*!
 * Call the DCF signal handler regularly
 */
PROGRAM(PROG_POLL_DCF, DONTSTART)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Update the LCD with the current time and date every second
 */
PROGRAM(PROG_PRINT_TIME, DONTSTART)
{
	#warning [Praktikum 5] Implement here
}

#endif
