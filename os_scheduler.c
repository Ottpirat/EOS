/*! \file os_scheduler.c
 *
 *  Contains everything needed to realize the scheduling between multiple processes.
 *  Also contains functions to start the execution of programs or killing a process.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.1
 */
#include "os_scheduler.h"
#include "lib/lcd.h"
#include "lib/util.h"
#include "os_core.h"
#include "os_process.h"
#include "os_scheduling_strategies.h"
#include "memory/os_memory.h"

#include <avr/interrupt.h>
#include <stdbool.h>

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//! Array of states for every possible process
process_t os_processes[MAX_NUMBER_OF_PROCESSES];

//! Array of function pointers for every registered program
program_t *os_programs[MAX_NUMBER_OF_PROGRAMS];

//! Index of process that is currently executed (default: idle)
process_id_t os_currentProcess = 0;

//! currently active scheduling strategy
scheduling_strategy_t os_currentSchedulingStrategy = INITIAL_SCHEDULING_STRATEGY;

//! count of currently nested critical sections
uint8_t os_criticalSectionCount = 0;

//! Used to auto-execute programs.
uint16_t os_autostart;

//----------------------------------------------------------------------------
// Private function declarations
//----------------------------------------------------------------------------

//! ISR for timer compare match (scheduler)
ISR(TIMER2_COMPA_vect) __attribute__((naked));

//! Wrapper to encapsulate processes
void os_dispatcher(void);

//! Casts a function pointer without throwing a warning
uint32_t addressOfProgram(program_t program);

//----------------------------------------------------------------------------
// Given functions
//----------------------------------------------------------------------------

/*!
 *  Casts a function pointer without throwing a warning.
 *
 *  \param program program pointer to convert
 *  \return converted uint32_t value
 */
uint32_t addressOfProgram(program_t program)
{
	// Unfortunately, everyone expects function pointers to be 16-Bit, but on
	// ATmega2560 they are 22-Bit... We are kind of confident that we know what
	// we are doing, so we can ignore the warning.
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
	return (uint32_t) program;
	#pragma GCC diagnostic pop
}

/*!
 *  Used to register a function as program. On success the program is written to
 *  the first free slot within the os_programs array (if the program is not yet
 *  registered) and the index is returned. On failure, INVALID_PROGRAM is returned.
 *  Note, that this function is not used to register the idle program.
 *
 *  \param program The function you want to register.
 *  \return The index of the newly registered program.
 */
program_id_t os_registerProgram(program_t *program)
{

	// Just check if idle-proc has been registered already
	assert(os_programs[0] != NULL, "Idle Proc not yet registered");

	uint8_t i = 0;

	// Check if the program is already registered and if yes, return the respective array slot
	for (; i < MAX_NUMBER_OF_PROGRAMS; i++)
	{
		if (program == os_programs[i])
		{
			return i;
		}
	}

	// If program is not yet registered...
	// Look for first free slot within os_programs array
	// Note that we start at 1, because this fct. is not used to register the idle proc
	for (i = 1; i < MAX_NUMBER_OF_PROGRAMS; i++)
	{
		if (os_programs[i] == NULL)
		{
			os_programs[i] = program;
			return i;
		}
	}

	// If the program is neither registered already nor is there a free slot, return INVALID_PROGRAM as an error indication
	return INVALID_PROGRAM;
}

/*!
 *  Used to check whether a certain program ID is to be automatically executed at
 *  system start.
 *
 *  \param programID The program to be checked.
 *  \return True if the program with the specified ID is to be started automatically.
 */
bool os_checkAutostartProgram(program_id_t programID)
{
	return (bool)(os_autostart & (1 << programID));
}

/*!
 * Lookup the main function of a program with id "programID".
 *
 * \param programID The id of the program to be looked up.
 * \return The pointer to the according function, or NULL if programID is invalid.
 */
program_t *os_lookupProgramFunction(program_id_t programID)
{
	// If the function pointer is null or the programID is invalid (i.e. too high)...
	if (programID >= MAX_NUMBER_OF_PROGRAMS)
	{
		return NULL;
	}

	// Else just return the respective function pointer
	return os_programs[programID];
}

/*!
 * Lookup the id of a program.
 *
 * \param program The function of the program you want to look up.
 * \return The id to the according slot, or INVALID_PROGRAM if program is invalid.
 */
program_id_t os_lookupProgramID(program_t *program)
{
	// Search program array for a match
	program_id_t i = 0;
	for (; i < MAX_NUMBER_OF_PROGRAMS; i++)
	{
		if (os_programs[i] == program)
		{
			return i;
		}
	}

	// If no match was found return INVALID_PROGRAM
	return INVALID_PROGRAM;
}

/*!
 *  A simple getter for the slot of a specific process.
 *
 *  \param pid The processID of the process to be handled
 *  \return A pointer to the memory of the process at position pid in the os_processes array.
 */
process_t *os_getProcessSlot(process_id_t pid)
{
	return os_processes + pid;
}

/*!
 *  A simple getter for the slot of a specific program.
 *
 *  \param programID The ProgramID of the process to be handled
 *  \return A pointer to the function pointer of the program at position programID in the os_programs array.
 */
program_t **os_getProgramSlot(program_id_t programID)
{
	return os_programs + programID;
}

/*!
 *  A simple getter to retrieve the currently active process.
 *
 *  \return The process id of the currently active process.
 */
process_id_t os_getCurrentProc(void)
{
	return os_currentProcess;
}

/*!
 *  This function return the the number of currently active process-slots.
 *
 *  \returns The number currently active (not unused) process-slots.
 */
uint8_t os_getNumberOfActiveProcs(void)
{
	uint8_t num = 0;

	process_id_t i = 0;
	do { num += os_getProcessSlot(i)->state != OS_PS_UNUSED; }
	while (++i < MAX_NUMBER_OF_PROCESSES);

	return num;
}

/*!
 *  This function returns the number of currently registered programs.
 *
 *  \returns The amount of currently registered programs.
 */
uint8_t os_getNumberOfRegisteredPrograms(void)
{
	uint8_t i;
	for (i = 0; i < MAX_NUMBER_OF_PROGRAMS && *(os_getProgramSlot(i)); i++) { }
	
	// Note that this only works because programs cannot be unregistered.
	return i;
}

/*!
 *  Sets the current scheduling strategy.
 *
 *  \param strategy The strategy that will be used after the function finishes.
 */
void os_setSchedulingStrategy(scheduling_strategy_t strategy)
{
	os_resetSchedulingInformation(strategy);
	os_currentSchedulingStrategy = strategy;
}

/*!
 *  This is a getter for retrieving the current scheduling strategy.
 *
 *  \return The current scheduling strategy.
 */
scheduling_strategy_t os_getSchedulingStrategy(void)
{
	return os_currentSchedulingStrategy;
}

/*!
 *  Enters a critical code section by disabling the scheduler if needed.
 *  This function stores the nesting depth of critical sections of the current
 *  process (e.g. if a function with a critical section is called from another
 *  critical section) to ensure correct behavior when leaving the section.
 *  This function supports up to 255 nested critical sections.
 */
void os_enterCriticalSection(void)
{
	// 1. Save global interrupt enable bit in local variable
	uint8_t ie = gbi(SREG, 7);

	// 2. Disable global interrupts (could also be done by sth. like SREG &=...)
	cli();

	// 3. Increment nesting depth of critical sections
	//    Throw error if there are already too many critical sections entered
	//    Note that it might be necessary to check for ==254 if os_error opens
	//    another critical section (not the case here...)
	if (os_criticalSectionCount == 255) { os_error("Crit. Section   overflow"); }
	else { os_criticalSectionCount++; }

	// 4. Deactivate OCIE2A bit in TIMSK2 register to deactivate the
	//    TIMER2_COMPA_vect interrupt (i.e. our scheduler)
	cbi(TIMSK2, OCIE2A);

	// 5. Restore global interrupt enable bit: if it was disabled, nothing happens
	//    If it was enabled, this operation does the same thing as "sei()"
	if (ie) { sei(); }
}

/*!
 *  Leaves a critical code section by enabling the scheduler if needed.
 *  This function utilises the nesting depth of critical sections
 *  stored by os_enterCriticalSection to check if the scheduler
 *  has to be reactivated.
 */
void os_leaveCriticalSection(void)
{
	// 1. Save global interrupt enable bit in local variable
	uint8_t ie = gbi(SREG, 7);

	// 2. Disable global interrupts (could also be done by sth. like SREG &=...)
	cli();

	// 3. Decrement nesting depth of critical sections
	//    Throw error if there is no critical Section that could be left
	if (os_criticalSectionCount == 0) { os_error("Crit. Section   underflow"); }
	else { os_criticalSectionCount--; }

	// 4. Activate OCIE2A bit in TIMSK2 register if the last opened critical section
	//    is about to be closed			//OCIE = OutputCompareMatch... 
	if (os_criticalSectionCount == 0) { sbi(TIMSK2, OCIE2A); }

	// 5. Restore global interrupt enable bit: if it was disabled, nothing happens
	//    If it was enabled, this operation does the same thing as "sei()"
	if (ie)	{ sei(); }
}

//----------------------------------------------------------------------------
// Your Homework
//----------------------------------------------------------------------------

/*!
 *  Timer interrupt that implements our scheduler. Execution of the running
 *  process is suspended and the context saved to the stack.  If everything is
 *  in order, the next process for execution is derived with an exchangeable strategy.
 *  Finally the scheduler restores the next process for execution and releases control
 *  over the processor to that process.
 */
ISR(TIMER2_COMPA_vect)
{

	// 1. Is implicitly done

	// 2. Save runtime-context of recently current process using the well-known macro
  saveContext();
	// 3. Save stack pointer of current process
  os_processes[os_currentProcess].sp.as_int = SP;
	// 4. Set stack pointer onto ISR-Stack
	//    Note that the ISR does not expect any data to be on the stack, 
	//    so we can set SP to the BOTTOM_OF_ISR_STACK
  SP = BOTTOM_OF_ISR_STACK;
	// 5. Set process state to ready
	//    Note for task 3: Only set to ready if it is currently running, otherwise
	//    the dead come back to life
  if (os_processes[os_currentProcess].state == OS_PS_RUNNING){

	  os_processes[os_currentProcess].state = OS_PS_READY;
  }
	// In task 3: Save the processes stack checksum

	// In task 3: Check if the stack pointer has an invalid value
	if (!os_isStackInBounds(os_currentProcess)){
	  os_error("Stack out of Bauntz");
	}
	else {
	  os_processes[os_currentProcess].checksum = os_getStackChecksum(os_currentProcess);
	}
	// 6. Find next process using the set scheduling strategy
  
	if (os_getSchedulingStrategy() == OS_SS_ROUND_ROBIN) {
    	os_currentProcess = os_scheduler_RoundRobin(os_processes, os_currentProcess);
	} else {
    	os_currentProcess = os_scheduler_DynamicPriorityRoundRobin(os_processes, os_currentProcess);
	}
	// In task 3: Check if the checksum changed since the process was interrupted
if (os_processes[os_currentProcess].checksum != os_getStackChecksum(os_currentProcess)) {
		os_error("Stack-Crash!");
	}
	// 7. Set the state of the now chosen process to running
  os_processes[os_currentProcess].state = OS_PS_RUNNING;
	// 8. Set SP to where it was when the resuming process was interrupted
  SP = os_processes[os_currentProcess].sp.as_int;
	// 9. Restore runtime context using the well-known macro
	//    This will cause the process to continue where it was interrupted.
	//    Any code after the macro won't be executed as it has an reti()
	//    instruction at the end
  restoreContext();
	// 10. Return is implicit through restoreContext()

}
/*!
 *  This is the idle program. The idle process owns all the memory
 *  and processor time no other process wants to have.
 */
PROGRAM(0, AUTOSTART)
{
	#warning [Praktikum 2] Implement here
	// lcd_init();  lasse ich mal drin als Erinnerung, wird aber bereits in os_core.c ausgeführt
	while(1){
		lcd_writeChar('.');
		delayMs(DEFAULT_OUTPUT_DELAY);
	}
}

/*!
 *  This function is used to execute a program that has been introduced with
 *  os_registerProgram.
 *  A stack will be provided if the process limit has not yet been reached.
 *
 *  \param programID The program id of the program to start (index of os_programs).
 *  \param priority Either one of OS_PRIO_LOW, OS_PRIO_NORMAL or OS_PRIO_HIGH
 *                  Note that the priority may be ignored by certain scheduling
 *                  strategies.
 *  \return The index of the new process (throws error on failure and returns
 *          INVALID_PROCESS as specified in defines.h).
 */
process_id_t os_exec(program_id_t programID, priority_t priority)
{
	#warning [Praktikum 2] Implement here

	process_id_t newprozess = INVALID_PROCESS;
	// 1. Enter a critical section
	os_enterCriticalSection();
	// 2. Find free slot in os_processes array
	for (int i = 0; i < MAX_NUMBER_OF_PROCESSES; i++){
		if (os_processes[i].state == OS_PS_UNUSED){
			newprozess = i;
			break;
		}
	}
	if (newprozess == INVALID_PROCESS){
		os_leaveCriticalSection();
		return newprozess;
	}
	// 3. Obtain the respective function pointer
	program_t *programptr = os_lookupProgramFunction(programID);
	// If looked up function pointer is invalid, abort os_exec and return INVALID_PROCESS
	if (programptr == NULL){
		os_leaveCriticalSection();
		return newprozess;
	}
	

	
	
	// 4. Save ProgramID and the process-state (and some more)
	
		os_processes[newprozess].state = OS_PS_READY;
		os_processes[newprozess].priority = priority;
		os_processes[newprozess].progID = programID;
	
	os_resetProcessSchedulingInformation(os_getSchedulingStrategy(), newprozess);

	// 5.1 push the address of the function to the stack
	//     Note for task 3: use address of os_dispatcher instead

	stack_pointer_t sp;
	sp.as_int = PROCESS_STACK_BOTTOM(newprozess);
	
	// gibt adresse in uint32_t aus und schaltet iwie warnungen aus
	uint32_t prog_addr = addressOfProgram((program_t*)os_dispatcher); //
	
	*(sp.as_ptr) = (uint8_t) (prog_addr & 0xFF);
	sp.as_int--;
	*(sp.as_ptr) = (uint8_t) ((prog_addr >> 8) & 0xFF);
	sp.as_int--;
	*(sp.as_ptr) = (uint8_t) ((prog_addr >> 16) & 0xFF);
	sp.as_int--;



	// 5.2 leave space on the process stack for register entries
	for (int i = 0; i < 33; i++)
	{
		*(sp.as_ptr) = 0x00;
		sp.as_int--;
	}
	
	os_processes[newprozess].sp = sp;
	os_processes[newprozess].checksum = os_getStackChecksum(newprozess);

	// For task 3: Save the stack checksum

	// 6. Leave Critical Section
	os_leaveCriticalSection();
	return newprozess;
}
/*!
 *  In order for the scheduler to work properly, it must have the chance to
 *  initialize its internal data-structures and register and start the idle
 *  program.
 */
void os_initScheduler(void)
{

	// As the processes are just being initialized, all slots should be unused
	for (int i = 0; i < MAX_NUMBER_OF_PROCESSES; i++)
	{
		os_processes[i].state = OS_PS_UNUSED;
	}

	os_resetSchedulingInformation(os_getSchedulingStrategy());
	
	// Uncomment:
	assert(os_programs[0] != NULL, "There is no idle proc");

	// Start all registered programs, which are marked as autostart
	// (i.e. call os_exec on them)
	for (uint8_t i = 0; i < MAX_NUMBER_OF_PROGRAMS; i++)
	{
		if (os_checkAutostartProgram(i)){
			os_exec(i,DEFAULT_PRIORITY);
		}
	}
}

/*!
 *  If all processes have been registered for execution, the OS calls this
 *  function to start the concurrent execution of the applications.
 */
void os_startScheduler(void)
{
	#warning [Praktikum 2] Implement here

	// Set os_currentProcess to idle process
	os_currentProcess = 0;
	// Set the state of the now chosen process to running
	os_processes[0].state = OS_PS_RUNNING;
	// Set SP on the stack of the idle process, so that we can restore its
	// well-prepared runtime context and jump into its program function
	SP = os_processes[0].sp.as_int;
	// Load initial context and start the idle process
	restoreContext();

	// You should never get here, as the scheduler must not terminate
	os_error("Runner von meim Speicher, Aller!");
}

/*!
 *  Calculates a spare checksum of the stack for a certain process, sampling
 *  only 16 equally distributed bytes of the whole process stack.
 *  First byte to sample is the stack's bottom.
 *
 *  \param pid The ID of the process for which the stack's checksum has to be calculated.
 *  \return The spare checksum of the pid'th stack.
 */
stack_checksum_t os_getStackChecksum(process_id_t pid)
{
	#warning [Praktikum 3] Implement here
	uint16_t stack_bottom = PROCESS_STACK_BOTTOM(pid);
	uint16_t stack_pointer= os_processes[pid].sp.as_int;
	uint16_t distance = (stack_bottom - stack_pointer);
	uint8_t step = distance/16;
	stack_checksum_t checksum = 0;

	for (int i = 0; i < 16; i++)
	{
		uint8_t* current_sp = (uint8_t*)(stack_bottom - (step * i));
		checksum ^= *current_sp;
	}
	return checksum;

}

/*!
 *  Check if the stack pointer is still in its bounds
 *
 *  \param pid The ID of the process for which the stack pointer has to be checked.
 *  \return True if the stack pointer is still in its bounds.
 */
bool os_isStackInBounds(process_id_t pid)
{
	#warning [Praktikum 3] Implement here
	uint16_t current_sp = os_processes[pid].sp.as_int;
	uint16_t max_addr = PROCESS_STACK_BOTTOM(pid);
	uint16_t min_addr = PROCESS_STACK_BOTTOM(pid) - STACK_SIZE_PROC;

	if (current_sp > max_addr || current_sp <= min_addr){
		return false;
	}
	return true;
}

/*!
 * Triggers scheduler to schedule another process.
 */
void os_yield()
{
	#warning [Praktikum 3] Implement here
	if (os_criticalSectionCount > 0){
		return;
	}
	TCNT2 = 0;
	cli();
	TIMER2_COMPA_vect();


	
}

/*!
 * Encapsulates any running process in order make it possible for processes to
 * terminate
 *
 * This wrapper enables the possibility to perform a few necessary steps after
 * the actual process function has finished.
 */
void os_dispatcher()
{
	#warning [Praktikum 3] Implement here

	// 1. Happens in os_exec
	process_id_t currentPid = os_getCurrentProc();
	// 2. Get function pointer
	program_id_t currentProgId = os_processes[currentPid].progID;
	program_t* functionptr = os_lookupProgramFunction(currentProgId);
	// 3. Call function
	functionptr();
	//    This obviously must be outside of a critical section so the
	//    scheduler can do its job while the process is running

	// 4. Happens implicitly

	// 5. Try to kill the terminating process
	os_kill(currentPid);
	// 6. Wait for scheduler to fetch another process
	//    Note that this process will not be fetched anymore as we cleaned up
	//    the corresponding slot in os_processes
	os_yield();
    // 7. You should never get here, terminating the dispatcher can lead to
	//    undefined behavior
	while(1);
}

/*!
 *  Kills a process by cleaning up the corresponding slot in os_processes.
 *
 *  \param pid The process id of the process to be killed
 *  \return True, if the killing process was successful
 */
bool os_kill(process_id_t pid)
{
	#warning [Praktikum 3] Implement here
	os_enterCriticalSection();
	// If the pid is invalid, return false
	if (pid == 0 || pid >= MAX_NUMBER_OF_PROCESSES || os_processes[pid].state == OS_PS_UNUSED){
		os_leaveCriticalSection();
		return false;
	}
	// Clean up the process slot of the terminating process
	os_processes[pid].state = OS_PS_UNUSED;
	// (TASK_4) Call garbage collection

	for (size_t i = 0; i < os_getHeapListLength(); i++) {
        os_freeProcessMemory(os_lookupHeap(i), pid);
    }

	// Tidy up the scheduler
	// (Process needs to be removed from ready queue of DPRR)
	os_resetProcessSchedulingInformation(os_getSchedulingStrategy(), pid);
	// If the process kills itself, critical sections of the process must be closed
	if (os_getCurrentProc() == pid){
		os_criticalSectionCount = 1;
		os_leaveCriticalSection();
		os_yield();
		while(1); // weil kp was passiert wenn yield doch zurückkommt
	}
	else {
		os_leaveCriticalSection();
		return true;
	}

	
}