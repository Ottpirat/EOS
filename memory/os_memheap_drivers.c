/*! \file os_memheap_drivers.c
 *
 *  Heap Driver structs and some initialization functions.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#include "os_memheap_drivers.h"
#include "../lib/defines.h"
#include "os_memory_strategies.h"

//! Heap addresses
#define MAP_START	(AVR_SRAM_START + HEAP_OFFSET)
#define MAP_SIZE	((AVR_MEMORY_SRAM / 2 - HEAP_OFFSET) / 3)
#define USE_START	(MAP_START + MAP_SIZE)
#define USE_SIZE	(2 * MAP_SIZE)

//! Driver instantiation for the internal heap
heap_t intHeap__ =
{
	.driver = intSRAM,
	.mapStart = MAP_START,
	.mapSize = MAP_SIZE,
	.useStart = USE_START,
	.useSize = USE_SIZE,
	.strategy = DEFAULT_ALLOCATION_STRATEGY,
	.lastChunk = USE_START,
};

/* \brief Private function that initializes a single heap
 * This function clears the Heap map of a specific heap.
 *
 * \param heap The heap to be initialized
 */
void os_initHeap(heap_t *heap)
{
	#warning [Praktikum 4] Implement here
	mem_addr_t current_addr = intHeap__.mapStart;
	size_t size = intHeap__.mapSize;

	for (uint16_t i = 0; i < size; i++){
		intHeap__.driver->write(current_addr + i, 0x00);
	}
}

/* \brief Function that initializes all Heaps of the OS
 * This function clears the Heap map of all Heaps used by the OS.
 */
void os_initHeaps()
{
	#warning [Praktikum 4] Implement here
	size_t length = os_getHeapListLength();
	for (size_t i = 0; i < length; i++){
		os_initHeap(os_lookupHeap(i));
	}
}

//----------------------------------------------------------------------------
// For testtasks
//----------------------------------------------------------------------------

/* Function needed by testtasks, returns a Heap depending on a given index.
 *
 * \param index Index of the desired Heap
 * \return Returns the heap that corresponds to the given index (e.g intHeap if the index was 0)
 */
heap_t* os_lookupHeap(uint8_t index)
{
	switch(index)
	{
		case 0:
			return intHeap;
		default:
			return NULL;
	}
}

/* Function needed by testtasks, returns the number of Heaps.
 *
 * \return The number of Heaps existing, e.g. 1 if there is only the intHeap
 */
size_t os_getHeapListLength(void)
{
	return 1;
}