/*! \file os_memory_strategies.h
 *  \brief Memory allocation library for the OS.
 *
 *  Contains the memory allocation strategies for the *malloc routines.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */

#ifndef _OS_MEMORY_STRATEGIES_H
#define _OS_MEMORY_STRATEGIES_H

#include "os_memheap_drivers.h"

//-----------------------------------------------------------------------------
// Function headers
//-----------------------------------------------------------------------------

//! First-fit strategy
mem_addr_t os_Memory_FirstFit(heap_t* heap, size_t size);

//! Next-fit strategy
mem_addr_t os_Memory_NextFit(heap_t* heap, size_t size);

//! Best-fit strategy
mem_addr_t os_Memory_BestFit(heap_t* heap, size_t size);

//! Worst-fit strategy
mem_addr_t os_Memory_WorstFit(heap_t* heap, size_t size);

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define DEFAULT_ALLOCATION_STRATEGY		OS_MEM_FIRST

#endif