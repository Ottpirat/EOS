//-------------------------------------------------
//          TestSuite: Range
//-------------------------------------------------
// Checks whether the dynamic memory is correctly
// cleared when processes are terminated.
//
//  NOTE:
//  The heap cleanup must work for this test to run correctly!
//  The task will hang, if you have a bug in the process termination.
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_RANGE

#include "../../lib/lcd.h"
#include "../../lib/util.h"
#include "../../os_core.h"
#include "../../os_scheduler.h"
#include "../../memory/os_memory.h"

#include <avr/interrupt.h>

#define DELAY 1

#define DRIVER intHeap
#define DSIZE (os_getUseSize(DRIVER))

uint16_t size = 8;
volatile uint16_t check_i = 0;

/*!
 * A version of lcd_drawBar without lcd_clear and critical sections
 */
void drawBar(uint8_t percent)
{
	// calculate number of bars
	uint16_t val = ((percent <= 100) ? percent : 100) * 16;
	// draw bars
	for (uint16_t i = 0; i < val; i += 100) { lcd_writeChar(LCD_CHAR_BAR); }
}

/*!
 * This program tests allocation of memory of arbitrary position and size within the heap.
 * To that end it allocates a quarter of the available memory
 * in chunks of increasing size (from 1 to DSIZE).
 */
PROGRAM(1, AUTOSTART)
{
    program_id_t cont = 1;
    lcd_clear();
    lcd_writeProgString(PSTR("[Testing range]"));
    if (size <= DSIZE)
	{
		lcd_clear();
		lcd_line2();
		lcd_writeProgString(PSTR("Mass alloc: "));
        lcd_writeDec(size);
		
		uint16_t numberOfChunks = DSIZE / size;
        // Allocate chunks
        for (uint16_t i = 0; i < numberOfChunks; i++)
		{
            if (!os_malloc(DRIVER, size))
			{
                lcd_clear();
                lcd_writeProgString(PSTR("Could not alloc! (Phase 1)"));
                while (1);
            }

            // Print progress
			lcd_line1();
            drawBar(100ul * i / numberOfChunks);
            lcd_line2();
			lcd_goto(1, 12);
            lcd_writeDec(size);
            check_i = i;
        }
        // Increase the chunk size for next program run
        size *= 2;
    }
	else
	{
        // All chunk sizes done. Switch to second phase (run prog 2).
        cont = 2;
    }

    // Force termination and freeing of allocated memory by heap cleanup
    // before first execution of new program instance.
    os_exec(cont, DEFAULT_PRIORITY);
}

/*!
 * This program asserts if a chunk of memory can be freed
 * by using a pointer to a arbitrary position within the chunk.
 */
PROGRAM(2, DONTSTART)
{
    uint8_t accuracy = 10;
    lcd_clear();
    lcd_writeProgString(PSTR("[Testing range]"));
	delayMs(500);

    // Iterate chunk sizes
    for (size = 1; size <= DSIZE; size *= 2)
	{
		lcd_clear();
        lcd_line2();
        lcd_writeProgString(PSTR("Rand. free: "));
        lcd_writeDec(size);

		uint16_t numberOfChunks = size / accuracy;
        // Try different addresses to free memory
        for (uint16_t i = 0; i < numberOfChunks; i+=2)
		{
            uint16_t startAddress = os_malloc(DRIVER, size);
            if (!startAddress)
			{
                lcd_clear();
                lcd_writeProgString(PSTR("Could not alloc! (Phase 2)"));
                while (1);
            }
			
            // Use the timer counter value as "pseudo-random" number
            os_free(DRIVER, startAddress + (TCNT0 % size));
			lcd_line1();
            drawBar(100ul * i / numberOfChunks);
            lcd_line2();
			lcd_goto(1, 12);
            lcd_writeDec(size);
        }
    }

	// SUCCESS
    while (1)
    {
        lcd_clear();
        lcd_writeProgString(PSTR("ALL TESTS PASSED"));
        delayMs(1000);
        lcd_clear();
        delayMs(1000);
    }
}
#endif
