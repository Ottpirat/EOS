//-------------------------------------------------
//       TestSuite: UnitTest Init Scheduler
//-------------------------------------------------
// Performs tests on initScheduler without
// requiring the OS to be started
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_UNIT_INIT_SCHEDULER

#include "../../os_core.h"
#include <string.h>

//! Dummy program
PROGRAM(1, AUTOSTART) {}

/*!
 * Output a message on the LCD and halt the system
 */
void message(const char* text)
{
    cli();
    lcd_clear();
    lcd_writeProgString(text);
    while (1);
}

/*!
 * Test condition. If false, output message and halt the system
 */
void test_assert(bool condition, const char* error)
{
    if (!condition) { message(error); }
}

/*!
 * Unit test function which is declared with "__attribute__ ((constructor))"
 * such that it is executed before the main function.
 */
void __attribute__((constructor)) test_os_initScheduler()
{
	// We have to initialize everything on our own since this code is run before main()
    initSystemTime();
    lcd_init();
    
    delayMs(100);
	lcd_clear();
    lcd_writeProgString(PSTR("Unittest os_initScheduler"));
    delayMs(1000);

    // Variable initialization has to be done by us
    extern process_t os_processes[MAX_NUMBER_OF_PROCESSES];
    memset(os_processes, 0, sizeof(os_processes));

    extern program_t* os_programs[MAX_NUMBER_OF_PROGRAMS];
    memset(os_programs, 0, sizeof(os_programs));

    extern void registerProgram0();
    extern void registerProgram1();
    registerProgram0();
    registerProgram1();

	// Explicitly call function and see what it did
    os_initScheduler();

    // Check that idle and program 1 were started with the correct priority
    test_assert(os_processes[0].state    == OS_PS_READY,      PSTR("Idle not ready"));
    test_assert(os_processes[0].priority == DEFAULT_PRIORITY, PSTR("Idle not defaultpriority"));
    test_assert(os_processes[1].state    == OS_PS_READY,      PSTR("Program 1 not started"));
    test_assert(os_processes[1].priority == DEFAULT_PRIORITY, PSTR("Program 1 not   default priority"));
    
	// Other slots should be unaffected
	for (uint8_t i = 2; i < MAX_NUMBER_OF_PROCESSES; i++) 
	{
        test_assert(os_processes[i].state == OS_PS_UNUSED, PSTR("Other slots not OS_PS_UNUSED"));
    }

	// Good job
    message(PSTR("  TEST  PASSED"));
	
    // Note that the main function is not executed because message contains an infinite loop
}

#endif