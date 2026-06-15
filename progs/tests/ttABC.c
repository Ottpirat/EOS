//-------------------------------------------------
//          TestSuite: ABC
//-------------------------------------------------
// Prints A B C on the LCD. Do not wonder why they
// are not in order or follow the same sequence.
// Scheduling does not mean every process gets time
// to print one char. Scheduling pauses processes
// whenever it wants to. This is not synchronized
// with your delay of 100 ms...
//
// What should not be on the screen? The idle
// process should be inactive, so no dots. Program
// 4 is on DONTSTART, so we expect no Ds as well.
// 
// Have fun!
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_ABC

#include "../../os_process.h"
#include "../../lib/defines.h"
#include "../../lib/lcd.h"

PROGRAM(1, AUTOSTART)
{
	while (1)
	{
		lcd_writeChar('A');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

PROGRAM(2, AUTOSTART)
{
	while (1)
	{
		lcd_writeChar('B');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

PROGRAM(3, AUTOSTART)
{
	while (1)
	{
		lcd_writeChar('C');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

//! No autostart!
PROGRAM(4, DONTSTART)
{
	while (1)
	{
		lcd_writeChar('D');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

#endif