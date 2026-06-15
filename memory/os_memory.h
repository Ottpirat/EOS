/*! \file os_memory.h
 *  \brief Library for OS memory management.
 *
 *  Contains heap management functionality for the OS.\n\n
 *
 *  \internal
 *  Structure of memory map:
 *	0 = free
 *	1-7 = private process
 *	F = stuffing symbol
 *
 *   example:
 *      3FFFF 5 bytes private memory reserved by proc 3
 *      3     1 byte private memory reserved by proc 3
 *      33FF  1 byte private memory and 3 bytes private memory, both reserved by 3
 *
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "os_mem_drivers.h"
#include "os_memheap_drivers.h"
#include "../os_scheduler.h"

//----------------------------------------------------------------------------
// Function Headers
//----------------------------------------------------------------------------

//! Function used to allocate private memory
mem_addr_t os_malloc(heap_t *heap, size_t size);

//! Function used by processes to free their own allocated memory
void os_free(heap_t *heap, mem_addr_t addr);

//! Function used to get the value of a single map entry, this is made public so the allocation strategies can use it
mem_value_t os_getMapEntry(heap_t const* heap, mem_addr_t addr);

//! Function that realizes the garbage collection
void os_freeProcessMemory(heap_t *heap, process_id_t pid);

// Testtask interaction -------------------------------------------------------

//! Get the size of the heap-map.
size_t os_getMapSize(heap_t const* heap);

//! Get the size of the usable heap.
size_t os_getUseSize(heap_t const* heap);

//! Get the start of the heap-map.
mem_addr_t os_getMapStart(heap_t const* heap);

//! Get the start of the usable heap.
mem_addr_t os_getUseStart(heap_t const* heap);

//! Get the size of a chunk on a given address
uint16_t os_getChunkSize(heap_t const* heap, mem_addr_t addr);

//! Get the address of the first byte of chunk
mem_addr_t os_getFirstByteOfChunk(heap_t const* heap, mem_addr_t addr);

// Strategy -------------------------------------------------------------------

//! Changes the memory management strategy
void os_setAllocationStrategy(heap_t *heap, alloc_strategy_t allocStrat);

//! Returns the current memory management strategy
alloc_strategy_t os_getAllocationStrategy(heap_t const* heap);


#endif