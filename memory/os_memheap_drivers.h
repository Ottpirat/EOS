/*! \file os_memheap_drivers.h
 *  \brief Drivers for OS heap management.
 *
 *  Contains management of several logical heaps, each associated with one MemDriver.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#ifndef OS_MEMHEAP_DRIVERS_H_
#define OS_MEMHEAP_DRIVERS_H_

#include "os_mem_drivers.h"
#include <stddef.h>

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------
//! All available heap allocation strategies.
typedef enum
{
	OS_MEM_FIRST,
	OS_MEM_NEXT,
	OS_MEM_BEST,
	OS_MEM_WORST
} alloc_strategy_t;

//! The structure of a heap driver which consists of a low level memory driver and heap specific information such as start, size etc...
typedef struct
{
	mem_driver_t* driver;
	mem_addr_t mapStart;
	mem_addr_t useStart;
	size_t mapSize;
	size_t useSize;
	alloc_strategy_t strategy;
	mem_addr_t lastChunk;
} heap_t;

//-----------------------------------------------------------------------------
// Function Headers
//-----------------------------------------------------------------------------

//! Initializes all Heaps
void os_initHeaps(void);

// Testtask interaction -------------------------------------------------------

//! Returns a Heap depending on a given index
heap_t* os_lookupHeap(uint8_t index);

//! Returns the number of Heaps
size_t os_getHeapListLength(void);

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

//! Pointer to the Heap intHeap__
#define intHeap (&intHeap__)

//! The heap_t structure for the internal Heap
extern heap_t intHeap__;


#endif /* OS_MEMHEAP_DRIVERS_H_ */