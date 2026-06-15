/*! \file os_scheduling_strategies.c
 *
 *  Scheduling strategies used by the Interrupt Service RoutineA from Timer 2 (in scheduler.c)
 *  to determine which process may continue its execution next.
 *
 *  The file contains two strategies:
 *  -round-robin
 *  -dynamic-priority-round-robin
 */

#include "os_scheduling_strategies.h"
#include "lib/defines.h"
#include "lib/ready_queue.h"
#include "os_scheduler.h"

#include <stdbool.h>
#include <stdlib.h>
#include "lib/util.h"
#include "lib/terminal.h"


extern process_t os_processes[];
//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

scheduling_information_t os_schedulingInfo; // initialization to 0 fits our needs

//----------------------------------------------------------------------------
// Given functions
//----------------------------------------------------------------------------

/*!
 *  Function used to determine whether there is any process ready (except the idle process)
 *
 *  \param processes[] The array of processes that it supposed to be looked through for processes that are ready
 *  \return True if there is a process ready which is not the idle proc
 */
bool isAnyProcReady(process_t const processes[])
{
	process_id_t i;
	for (i = 1; i < MAX_NUMBER_OF_PROCESSES; i++)
	{
		// The moment we find a single process that is ready/running, we can already
		// return True
		if (processes[i].state == OS_PS_READY) { return true; }
	}

	// Not a single process that is ready/running has been found in the loop, so there is none
	return false;
}

//----------------------------------------------------------------------------
// Your Homework
//----------------------------------------------------------------------------

/*!
 *  This function implements the round-robin strategy. Every process gets the same
 *  amount of processing time and is rescheduled after each scheduler call
 *  if there are other processes running other than the idle process.
 *  The idle process is executed if no other process is ready for execution
 *
 *  \param processes An array holding the processes to choose the next process from.
 *  \param current The id of the current process.
 *  \return The next process to be executed determined on the basis of the even strategy.
 */
process_id_t os_scheduler_RoundRobin(process_t const processes[], process_id_t current)
{

	// If no process except idle process ready, choose idle process

	for (int i = 0; i < MAX_NUMBER_OF_PROCESSES; i++){
		current++;
		process_id_t next_id = current % MAX_NUMBER_OF_PROCESSES;
		if ((next_id != 0) && processes[next_id].state == OS_PS_READY){
			return next_id;
		}
	}

	return 0;
	// Look for the next proc that is ready, there has to be at least one,
	// this has been checked before. Don't choose 0 as it is the idle process

	// Return resulting process id

}

/*!
 * Reset the scheduling information for a specific process slot
 * This is necessary when a new process is started to clear out any
 * leftover data from a process that previously occupied that slot
 *
 * \param strategy The scheduling strategy currently in use
 * \param id  The process slot to erase state for
 */
void os_resetProcessSchedulingInformation(scheduling_strategy_t strategy, process_id_t id)
{
	#warning [Praktikum 3] Implement here

	if (strategy != OS_SS_DYNAMIC_PRIORITY_ROUND_ROBIN) return;

	for (uint8_t i = 0; i < PRIORITY_COUNT; i++)
	{
		rq_remove(&os_schedulingInfo.queues_ready[i], id);
	}
	if (os_processes[id].state == OS_PS_READY) {
    rq_push(&os_schedulingInfo.queues_ready[os_processes[id].priority], id);
}
}

/*!
 *  Reset the scheduling information for a specific strategy
 *  This is only relevant for DynamicPriorityRoundRobin
 *  and is done when the strategy is changed through os_setSchedulingStrategy
 *
 * \param strategy  The strategy to reset information for
 */
void os_resetSchedulingInformation(scheduling_strategy_t strategy)
{
	if (strategy != OS_SS_DYNAMIC_PRIORITY_ROUND_ROBIN){
		return;
	}

	for (uint8_t i = 0; i < PRIORITY_COUNT; i++)
	{
		rq_clear(&os_schedulingInfo.queues_ready[i]);
	}
	for (process_id_t i = 1; i < MAX_NUMBER_OF_PROCESSES; i++){
		if (os_processes[i].state == OS_PS_READY){

			priority_t procprio = os_processes[i].priority;

			rq_push(&os_schedulingInfo.queues_ready[procprio], i);
		}
	}
	
}

/*!
 *  This function implements the dynamic-priority-round-robin strategy.
 *  In this strategy, process priorities will matter that's achieved through multiple ready queues
 *
 *  \param processes An array holding the processes to choose the next process from.
 *  \param current The id of the current process.
 *  \return The next process to be executed determined on the basis of the dynamic priority round-robin strategy.
 */
/*
process_id_t os_scheduler_DynamicPriorityRoundRobin(process_t const processes[], process_id_t current)
{
	debug_dump_queues();

    while (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL])) {
        process_id_t aged_pid = rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL]);
        rq_push(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH], aged_pid);
        DEBUG("[DPRR-Ageing] PID %d von NORMAL nach HIGH \n", aged_pid);
    }

    while (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_LOW])) {
        process_id_t aged_pid = rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_LOW]);
        rq_push(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL], aged_pid);
        DEBUG("[DPRR-Ageing] PID %d von LOW nach NORMAL \n", aged_pid);
    }



    if (current != 0 && processes[current].state != OS_PS_UNUSED) {
        priority_t prio = processes[current].priority;
        rq_push(&os_schedulingInfo.queues_ready[prio], current);
        DEBUG("[DPRR] Pausiere PID %d -> zurück in due Queue %d\n", current, prio);
    }

	debug_dump_queues();

    process_id_t next_pid = 0;

    if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH])) {
        next_pid = rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH]);
        DEBUG("[DPRR] Picke PID %d aus HIGH\n", next_pid);
        return next_pid;
    }
    
    if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL])) {
        next_pid = rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL]);
        DEBUG("[DPRR] Picke PID %d aus NORMAL\n", next_pid);
        return next_pid;
    }
    
    if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_LOW])) {
        next_pid = rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_LOW]);
        DEBUG("[DPRR] Picke PID %d aus LOW\n", next_pid);
        return next_pid;
    }

    // Wenn wir hier landen, sind alle Queues komplett leer.
    DEBUG("[DPRR] Alle Queues leer. Idle.\n");
    return 0;
} 

*/


process_id_t os_scheduler_DynamicPriorityRoundRobin(process_t const processes[], process_id_t current)
{
	#warning [Praktikum 3] Implement here

	// 1. Move processes one higher in priority
if (!isAnyProcReady(processes)) return 0;

if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL])) {
    	rq_push(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH], 
		rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL]));
		
}

if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_LOW])) {
    	rq_push(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL],
		rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_LOW]));
}
	// 2. Push current process to the ready queue
	if (current != 0 && processes[current].state == OS_PS_READY){
		priority_t prio = processes[current].priority;
		rq_push(&os_schedulingInfo.queues_ready[prio],current);
	}
	// 3. Get next process from ready queue
	if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH])){
		return rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_HIGH]);
	}
	if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL])){
		return rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_NORMAL]);
	}
	if (!rq_isEmpty(&os_schedulingInfo.queues_ready[OS_PRIO_LOW])){
		return rq_pop(&os_schedulingInfo.queues_ready[OS_PRIO_LOW]);
	}
	return 0;
}




void debug_dump_queues(void) {
    DEBUG("=== QUEUE DUMP ===\n");
    
    for (uint8_t p = 0; p < PRIORITY_COUNT; p++) {
        DEBUG("Prio %d: [ ", p);
        
        ready_queue_t *q = &os_schedulingInfo.queues_ready[p];
        
        uint8_t current_idx = q->head;
        
        while (current_idx != q->tail) {
            DEBUG("%d ", q->processes[current_idx]);
            current_idx = (current_idx + 1) % READY_QUEUE_SIZE; 
        }
        
        DEBUG("]\n");
    }
    DEBUG("==================\n");
}