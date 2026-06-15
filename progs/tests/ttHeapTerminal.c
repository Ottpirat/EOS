//-------------------------------------------------
//          TestSuite: Heap Terminal
//-------------------------------------------------
// Performs some simple allocations and prints the
// resulting heap on the terminal interface
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_HEAP_TERMINAL

#include "../../os_core.h"
#include "../../memory/os_memory.h"
#include "../../memory/os_memheap_drivers.h"
#include "../../lib/buttons.h"

// Choose heap driver to work on
#define DRIVER				intHeap

// Choose allocation strategy
#define STRATEGY			OS_MEM_FIRST

// First allocation allocates START_SIZE. Size is duplicated in each round
#define ALLOC_START_SIZE	32

// Constants for prints
const char strAsFF[] PROGMEM = "First";
const char strAsNF[] PROGMEM = "Next";
const char strAsBF[] PROGMEM = "Best";
const char strAsWF[] PROGMEM = "Worst";
PGM_P const strAs[] PROGMEM = {strAsFF, strAsNF, strAsBF, strAsWF};

// Forward declarations
void printHeap();
bool allocateMem(size_t allocSize);

PROGRAM(1, AUTOSTART)
{
	INFO("Testtask Heap Terminal");
	lcd_writeProgString(PSTR(" ttHeapTerminal"));
	lcd_line2();
	lcd_writeProgString(PSTR("> Use Terminal <"));
	
	// Memory parameters
	INFO("= HEAP PARAMTERS ==========================");
	INFO("  OFFSET:        %d Byte", HEAP_OFFSET);
	INFO("  MAP:           0x%04X -> 0x%04X (%d Byte)", DRIVER->mapStart, DRIVER->mapStart + DRIVER->mapSize - 1, DRIVER-> mapSize);
	INFO("  USE:           0x%04X -> 0x%04X (%d Byte)", DRIVER->useStart, DRIVER->useStart + DRIVER->useSize - 1, DRIVER-> useSize);
	INFO("= STACK PARAMTERS =========================");
	INFO("  Lowest address of stack: 0x%04X", PROCESS_STACK_BOTTOM(MAX_NUMBER_OF_PROCESSES) + 1);	
	for (int8_t i = MAX_NUMBER_OF_PROCESSES - 1; i > 0; i--) { INFO("  PROCESS_STACK_BOTTOM(%d): 0x%04X", i, PROCESS_STACK_BOTTOM(i)); }
	INFO("  BOTTOM_OF_PROCS_STACK:   0x%04X", BOTTOM_OF_PROCS_STACK);
	INFO("  BOTTOM_OF_ISR_STACK:     0x%04X", BOTTOM_OF_ISR_STACK);	
	INFO("  BOTTOM_OF_MAIN_STACK:    0x%04X", BOTTOM_OF_MAIN_STACK);
	INFO("===========================================");
	
	// Set allocation strategy
	os_setAllocationStrategy(DRIVER, STRATEGY);
	INFO("Allocation strategy set to: %S-Fit", strAs[STRATEGY]);
	
	INFO("Press SELECT to print initial heap state...");
	buttons_waitForPressed(BTN_SELECT);
	buttons_waitForReleased(BTN_SELECT);
	
	// Heap before doing anything
	INFO("Initial state of heap:");
	printHeap();
	
	INFO("Press SELECT to allocate memory...");
	buttons_waitForPressed(BTN_SELECT);
	buttons_waitForReleased(BTN_SELECT);

	while (1)
	{
		process_id_t pidAllocateProc = os_exec(2, DEFAULT_PRIORITY);
		if (pidAllocateProc == INVALID_PROCESS) { os_error("Could not start allocation proc"); }
		INFO("Starting Process %d to allocate memory...", pidAllocateProc);
		
		// Wait for the process to terminate
		while (os_getNumberOfActiveProcs() > 2) { os_yield(); }
		
		// Print heap after termination
		INFO("Allocation process terminated. Check if the Garbage Collection has freed allocations...", pidAllocateProc);
		printHeap();
		
		INFO("Press SELECT to start a new allocation process...");
		buttons_waitForPressed(BTN_SELECT);
		buttons_waitForReleased(BTN_SELECT);
	}
}

PROGRAM(2, DONTSTART)
{
	size_t allocSize = ALLOC_START_SIZE;
	
	// Allow user to allocate more memory
	while (1)
	{
		// Allocate until user wants us to terminate
		if (!allocateMem(allocSize)) { break; }
		
		// Increase allocation size for next round
		allocSize *= 2;
	}
	
	// Goodbye
	INFO("Allocation process terminating...");	
}

bool allocateMem(size_t allocSize)
{
	bool ret = true;
	
	// Allocate some memory
	INFO("Allocating %d Byte of memory...", allocSize);
	mem_addr_t alloc = os_malloc(DRIVER, allocSize);
	
	INFO("Heap state after allocation:");
	printHeap();
	
	if (alloc == 0)
	{
		ERROR("Could not allocate...");
		INFO("Press SELECT to terminate allocation process and continue...");
		buttons_waitForPressed(BTN_SELECT);
		buttons_waitForReleased(BTN_SELECT);
		return false;
	}
	else
	{
		INFO("os_malloc returned address 0x%04X", alloc);
		INFO("DRIVER->lastChunk == 0x%04X", DRIVER->lastChunk);				
	}
		
	INFO("Press SELECT to write to use data...");
	buttons_waitForPressed(BTN_SELECT);
	buttons_waitForReleased(BTN_SELECT);
		
	// Write increasing numbers to the allocated memory
	INFO("Writing %d Byte of memory...", allocSize);
	for (mem_addr_t i = 0; i < allocSize; i++)	{ DRIVER->driver->write(alloc + i, i); }
	
	INFO("Heap state after write:");
	printHeap();
	
	// Ask to allocate more or to leave
	INFO("Select an option:");
	INFO("- Press RIGHT to allocate more");
	INFO("- Press LEFT to free lastly allocated memory");
	INFO("- Press DOWN to terminate process to test Garbage Collection");
	while (!buttons_pressed(BTN_LEFT) && !buttons_pressed(BTN_RIGHT) && !buttons_pressed(BTN_DOWN));
	
	// LEFT -> call os_free on lastly allocated memory
	if (buttons_pressed(BTN_LEFT))
	{
		INFO("Freeing %d Byte of memory at address 0x%04X...", allocSize, alloc);
		os_free(DRIVER, alloc);
		INFO("Heap state after os_free:");
		printHeap();
		
		INFO("Press SELECT to allocate more...");
		buttons_waitForPressed(BTN_SELECT);
		buttons_waitForReleased(BTN_SELECT);
	}
	// DOWN -> Terminate
	else if (buttons_pressed(BTN_DOWN))	{ ret = false; }

	buttons_waitForReleased(BTN_LEFT);
	buttons_waitForReleased(BTN_RIGHT);
	buttons_waitForReleased(BTN_DOWN);
	
	return ret;
}

/*
 * Prints heap map and use on the terminal interface
 */
void printHeap()
{
	terminal_writeProgString(PSTR("MAP ==================================== USE ==================================================\n"));
	
	uint16_t useOffset = 0;
	uint8_t data, i;
	while (useOffset < DRIVER->useSize)
	{
		// Get map nibbles and print
		terminal_writeProgString(PSTR("0x"));
		terminal_writeHexWord(DRIVER->mapStart + useOffset / 2);
		terminal_writeChar(' ');		
		for (i = 0; i < 8; i++)
		{
			// We need to pad the last mem row in order to not destroy table layout
			if (useOffset + i >= DRIVER->useSize) { terminal_writeProgString(PSTR("    ")); continue; }
			
			data = *((uint8_t*)DRIVER->mapStart + useOffset / 2 + i);
			terminal_writeHexNibble(data >> 4);
			terminal_writeChar(' ');
			terminal_writeHexNibble(data & 0xF);
			terminal_writeChar(' ');
		}
		
		// Get use byte and print
		terminal_writeProgString(PSTR("  0x"));
		terminal_writeHexWord(DRIVER->useStart + useOffset);
		terminal_writeChar(' ');
		for (i = 0; i < 16; i++)
		{
			data = *((uint8_t*)DRIVER->useStart + useOffset + i);
			terminal_writeHexByte(data);
			terminal_writeChar(' ');
			
			// Delay each row a bit for nicer printing
			delayMs(1);
			
			// We can just quit the last memory row
			if (useOffset + i == DRIVER->useSize) { break; }
		}
		
		// Next memory row
		useOffset += i;		
		terminal_writeChar('\n');
	}
}
#endif