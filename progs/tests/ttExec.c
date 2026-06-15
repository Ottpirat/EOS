//-------------------------------------------------
//          TestSuite: Exec
//-------------------------------------------------
// Tests os_exec behavior, setup of os_processes
// stack and CriticalSection
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && (TESTTASK == TT_EXEC_P2 || TESTTASK == TT_EXEC_P3)

#include "../../os_core.h"

// Process context has 32 registers r0 - r31, SREG and PC
#define PROCESS_REGISTER_SIZE (32 + 1)
#define PROCESS_CONTEXT_SIZE (PROCESS_REGISTER_SIZE + 3)

// Required structures and functions
extern uint8_t os_criticalSectionCount;
extern process_t os_processes[];
extern void os_dispatcher(void);
extern uint32_t addressOfProgram(program_t);

//! This program calls os_exec and checks the result.
PROGRAM(1, AUTOSTART)
{
  	os_enterCriticalSection();

	lcd_writeProgString(PSTR("TT_Exec:"));
    INFO("TT_Exec started...");
	
	const uint8_t critSectCountBeforeExec = os_criticalSectionCount;
	const uint8_t programId = 2;

    // run os_exec
	const uint8_t procSlot = os_exec(programId, DEFAULT_PRIORITY);

	if (procSlot == INVALID_PROCESS) { os_error("os_exec returned INVALID_PROCESS"); }
	else { INFO("os_exec returned valid process slot"); delayMs(DEFAULT_OUTPUT_DELAY); }

    // Check CriticalSection
    if (critSectCountBeforeExec != os_criticalSectionCount) { os_error("CriticalSection not closed"); }
    else { INFO("os_exec closed all critical sections it opened"); delayMs(DEFAULT_OUTPUT_DELAY); }

	// Check os_processes
	if (os_processes[procSlot].progID != programId)
	{
		ERROR("Expected progID value: %d", programId);
		ERROR("Actual   progID value: %d", os_processes[procSlot].progID);
		os_error("os_processes.progID");
	}
	else { INFO("os_exec used correct program ID"); delayMs(DEFAULT_OUTPUT_DELAY); }

	if (os_processes[procSlot].state != OS_PS_READY) { os_error("Process state != OS_PS_READY"); }
	else { INFO("os_exec set process state to OS_PS_READY"); delayMs(DEFAULT_OUTPUT_DELAY); }
		
	if (os_processes[procSlot].priority != DEFAULT_PRIORITY) { os_error("Process prio != DEFAULT_PRIORITY"); }
	else { INFO("os_exec set priority to DEFAULT_PRIORITY"); delayMs(DEFAULT_OUTPUT_DELAY); }
	
	// Check stack
	if (!os_isStackInBounds(procSlot)) { os_error("Process stack out of bounds"); }
	else { INFO("Stack pointer is inside its process stack"); delayMs(DEFAULT_OUTPUT_DELAY); }
	
	// Check process context size
    if (os_processes[procSlot].sp.as_int != PROCESS_STACK_BOTTOM(procSlot) - PROCESS_CONTEXT_SIZE)
	{
		ERROR("Expected sp.as_int value: %d", PROCESS_STACK_BOTTOM(procSlot) - PROCESS_CONTEXT_SIZE);
    	ERROR("Actual   sp.as_int value: %d", os_processes[procSlot].sp.as_int);
    	os_error("Unexpected SP");
    }
    else { INFO("Process context put on stack has correct size"); delayMs(DEFAULT_OUTPUT_DELAY); }

#if (TESTTASK == TT_EXEC_P3)
	// Check checksum
    if (os_processes[procSlot].checksum != os_getStackChecksum(procSlot))
	{
		ERROR("Expected checksum value: %d", os_getStackChecksum(procSlot));
		ERROR("Actual   checksum value: %d", os_processes[procSlot].checksum);
     	os_error("Checksum error");
    }
    else { INFO("Stored checksum has correct initial value"); delayMs(DEFAULT_OUTPUT_DELAY); }
#endif

    lcd_line2();
	lcd_writeProgString(PSTR("os_processes  OK"));
	INFO("os_processes OK");
    delayMs(2000);

    // Check function or dispatcher address in Stack
    program_t *fctPtr = os_lookupProgramFunction(2);
	uint32_t addressFct = addressOfProgram(fctPtr);
    uint32_t addressDisp = addressOfProgram(os_dispatcher);
    uint32_t addressFromStack = 0;

	// Check that the register stack is set to 0x00
    for (uint8_t i = 0; i <= PROCESS_REGISTER_SIZE; i++)
	{
		if (*(++os_processes[procSlot].sp.as_ptr) != 0x00)
		{
            ERROR("Register starts at address %d", os_processes[procSlot].sp.as_int - i);
            ERROR("Register stack not set to 0x00 at address %d", os_processes[procSlot].sp.as_int);
            ERROR("Function address should start at %d", os_processes[procSlot].sp.as_int + (PROCESS_REGISTER_SIZE - i));
            os_error("Stack not init- ialized to 0");
        }
	}
	INFO("All process registers correctly initialized to 0x00");
	delayMs(DEFAULT_OUTPUT_DELAY);

	// Read function address from stack
	addressFromStack |= ((uint32_t)*(os_processes[procSlot].sp.as_ptr++)) << 16;
	addressFromStack |= (*(os_processes[procSlot].sp.as_ptr++)) << 8;
	addressFromStack |= (*(os_processes[procSlot].sp.as_ptr++)) << 0;

    if (addressFct != addressFromStack && addressDisp != addressFromStack)
	{
		uint8_t addrHigh;
		uint16_t addrLow;
		
		addrHigh = addressFct >> 16;
		addrLow = addressFct;		
		ERROR("Expected address (program):    0x%02X%04X", addrHigh, addrLow);
		
		addrHigh = addressDisp >> 16;
		addrLow = addressDisp;
		ERROR("Expected address (dispatcher): 0x%02X%04X", addrHigh, addrLow);
		
		addrHigh = addressFromStack >> 16;
		addrLow = addressFromStack;
		ERROR("Actual address:                0x%02X%04X", addrHigh, addrLow);

      	os_error("Function pointer on stack");
    }
		
	if (addressFromStack == addressFct)       { INFO("Function pointer on stack");   }
	else if (addressFromStack == addressDisp) { INFO("Dispatcher pointer on stack"); }
	delayMs(DEFAULT_OUTPUT_DELAY);

    // Test finished successful
	lcd_clear();
	lcd_writeProgString(PSTR("TT_Exec:"));
	lcd_line2();
	lcd_writeProgString(PSTR("Stack FctPtr  OK"));
	INFO("Stack FctPtr OK");
    delayMs(2000);

	lcd_clear();
	lcd_writeProgString(PSTR("TT_Exec:"));
	lcd_line2();
	lcd_writeProgString(PSTR("Test finished"));
    INFO("Exec finished successful");

    // Programs are not yet allowed to terminated
	while (1);
}

//! Simple program that does not terminate
PROGRAM(2, DONTSTART) {	while(1); }

#endif