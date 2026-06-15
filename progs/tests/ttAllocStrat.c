//-------------------------------------------------
//          TestSuite: Alloc Strategies
//-------------------------------------------------
// Test allocation strategies First-Fit, Next-Fit,
// Best-Fit and Worst-Fit
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_ALLOC_STRAT

#include "../../lib/lcd.h"
#include "../../lib/util.h"
#include "../../os_core.h"
#include "../../memory/os_memory.h"
#include "../../os_scheduler.h"

#include <stdlib.h>

#define DELAY 100

#define DRIVER intHeap
/*
 * SELECT HERE!
 * Choose Alloc.Strats, which you want to be tested (1 = will be tested, 0 = wont be tested)
 */
#define FIRST   1
#define NEXT    1
#define BEST    1
#define WORST   1

PROGRAM(1, AUTOSTART)
{

    // Struct-Array with Alloc.Strats
    struct
	{
        alloc_strategy_t strat;
        const char* name;
        uint8_t check;
    }
	
	cycle[] =
	{
        {
            .strat = OS_MEM_FIRST,
            .name = PSTR("First"),
            .check = FIRST
        }, {
            .strat = OS_MEM_NEXT,
            .name = PSTR("Next"),
            .check = NEXT
        }, {
            .strat = OS_MEM_BEST,
            .name = PSTR("Best"),
            .check = BEST
        }, {
            .strat = OS_MEM_WORST,
            .name = PSTR("Worst"),
            .check = WORST
        }
    };

    /*
     * Create following pattern in memory: first big, second small, rest huge
     * X = allocated
     * __________________________________
     * |   |   |   | X |   |   | X |   | ...
     * 0   5   10  15  20  25  30  35
     *
     * malloc and free must be working correctly
     */
    mem_addr_t p[7];
    uint8_t s[] = {15, 5, 10, 5, 1};
    uint8_t lastStrategy;
    uint8_t error = 0;

    // Precheck heap size
    if (os_getUseSize(DRIVER) < 50) { os_error("Heap too small"); }

    os_enterCriticalSection();
    lcd_clear();
    lcd_writeProgString(PSTR("Check strategy.."));
    delayMs(10 * DELAY);
    lcd_clear();
    os_leaveCriticalSection();

    uint16_t i;
    uint16_t start = os_getMapStart(DRIVER);

    // Check if map is clean
    for (i = 0; i < os_getMapSize(DRIVER); i++)
	{
        if (DRIVER->driver->read(start + i))
		{
            os_enterCriticalSection();
            lcd_clear();
            lcd_writeProgString(PSTR("Map not free"));
            while (1);
        }
    }

    // Check overalloc for all strategies
    for (uint8_t strategy = 0; strategy < 4; strategy++)
	{
        if (!cycle[strategy].check) { continue; }
			
        os_setAllocationStrategy(DRIVER, cycle[strategy].strat);
        if (os_malloc(DRIVER, os_getUseSize(DRIVER) + 1) != 0)
		{
            lcd_clear();
            lcd_writeProgString(PSTR("Overalloc"));
            lcd_line2();
            lcd_writeProgString(cycle[strategy].name);
            while (1);
        }
    }

    // The test for next fit depends on not creating the memory pattern with OS_MEM_NEXT
    os_setAllocationStrategy(DRIVER, OS_MEM_FIRST);
    // Create pattern in memory
    for (i = 0; i < 5; i++) { p[i] = os_malloc(DRIVER, s[i]); }
    for (i = 0; i <= 4; i += 2) { os_free(DRIVER, p[i]); }

    // Check strategies
    for (lastStrategy = 0; lastStrategy < 4; lastStrategy++)
	{
        if (!cycle[lastStrategy].check) { continue; }

        lcd_clear();
        lcd_writeProgString(PSTR("Checking strat."));
        lcd_line2();
        lcd_writeProgString(cycle[lastStrategy].name);
        os_setAllocationStrategy(DRIVER, cycle[lastStrategy].strat);
        delayMs(10 * DELAY);
        lcd_clear();

        /*
         * We allocate, and directly free two times (except for Best-Fit)
         * for Next-Fit we should get different addresses
         * for First-Fit, both addresses are equal to first segment
         * for Best-Fit, first address equals second segment an second address equals first segment
         * for Worst-Fit we get the first byte of the rest memory
         * otherwise we found an error
         */
        for (i = 5; i < 7; i++)
		{
            p[i] = os_malloc(DRIVER, s[2]);
            if (cycle[lastStrategy].strat != OS_MEM_BEST) { os_free(DRIVER, p[i]); }
        }


        // Next-Fit
        if (cycle[lastStrategy].strat == OS_MEM_NEXT)
		{
            if (p[5] != p[0] || p[6] != p[2])
			{
                lcd_writeProgString(PSTR("Error Next-Fit"));
                delayMs(10 * DELAY);
                error = 1;
            }
			else
			{
				lcd_writeProgString(PSTR("Next-Fit OK"));
            }
            delayMs(10 * DELAY);
        }

        // First-Fit
        else if (cycle[lastStrategy].strat == OS_MEM_FIRST)
		{
            if (p[5] != p[6] || p[5] != p[0])
			{
                lcd_writeProgString(PSTR("Error First-Fit"));
                delayMs(10 * DELAY);
                error = 1;
            }
			else
			{
                lcd_writeProgString(PSTR("First-Fit OK"));
            }
            delayMs(10 * DELAY);
        }
        // Best-Fit
        else if (cycle[lastStrategy].strat == OS_MEM_BEST)
		{
            if (p[5] != p[2] || p[6] != p[0])
			{
                lcd_writeProgString(PSTR("Error Best-Fit"));
                delayMs(10 * DELAY);
                error = 1;
            }
			else
			{
                lcd_writeProgString(PSTR("Best-Fit OK"));
            }
            delayMs(10 * DELAY);

            // Free manually as it wasn't done before
            if (p[5]) { os_free(DRIVER, p[5]); }
            if (p[6]) { os_free(DRIVER, p[6]); }
        }
        // Worst-Fit
        else if (cycle[lastStrategy].strat == OS_MEM_WORST)
		{
            if (p[4] != p[5] || p[5] != p[6])
			{
                lcd_writeProgString(PSTR("Error Worst-Fit"));
                delayMs(10 * DELAY);
                error = 1;
            }
			else
			{
                lcd_writeProgString(PSTR("Worst-Fit OK"));
            }
            delayMs(10 * DELAY);
        }
		else
		{
            lcd_writeChar('e');
        }
    }

    // Remove pattern
    for (i = 1; i <= 3; i += 2) { os_free(DRIVER, p[i]); }

    // Check if map is clean
    for (i = 0; i < os_getMapSize(DRIVER); i++)
	{
        if (DRIVER->driver->read(start + i))
		{
            os_enterCriticalSection();
            lcd_clear();
            lcd_writeProgString(PSTR("Map not free afterwards"));
            while (1);
        }
    }

    // Special Next-Fit test
    if (NEXT)
	{
        lcd_clear();
        lcd_writeProgString(PSTR("Special Next-Fit test"));
        delayMs(10 * DELAY);
        lcd_clear();
        os_setAllocationStrategy(DRIVER, OS_MEM_NEXT);
        size_t rema = os_getUseSize(DRIVER);
		
        for (i = 0; i < 5; i++) { rema -= s[i]; }
        p[0] = os_malloc(DRIVER, rema);
        os_free(DRIVER, p[0]);
        for (i = 0; i < 3; i++) { p[i] = os_malloc(DRIVER, s[i]); }
        rema = os_getUseSize(DRIVER) - (s[0] + s[1] + s[2]);
        os_malloc(DRIVER, rema);
        for (i = 1; i < 3; i++) { os_free(DRIVER, p[i]); }
        p[1] = os_malloc(DRIVER, s[1]);
        os_free(DRIVER, p[1]);
        if (!os_malloc(DRIVER, s[1] + s[2]))
		{
            os_enterCriticalSection();
            lcd_clear();
            lcd_writeProgString(PSTR("Error Next-Fit (special)"));
            delayMs(10 * DELAY);
            error = 1;
        }
		else
		{
            lcd_writeProgString(PSTR("Spl. Next-Fit OK"));
            delayMs(10 * DELAY);
        }
    }

    lcd_clear();
    if (!error)
	{
        while (1)
        {
            lcd_clear();
            lcd_writeProgString(PSTR("All strategies  passed"));
            delayMs(1000);
            lcd_clear();
            delayMs(1000);
        }
    }
	else
	{
        while (1)
        {
            lcd_clear();
            lcd_writeProgString(PSTR("Check failed"));
            delayMs(1000);
            lcd_clear();
            delayMs(1000);
        }
    }
    delayMs(10 * DELAY);

}
#endif