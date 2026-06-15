/*! \file os_mem_drivers.h
 *  \brief Drivers for OS memory management.
 *
 *  Contains management of several RAM devices of the OS. It contains everything that is associated with low level memory access.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#ifndef OS_MEM_DRIVERS_H_
#define OS_MEM_DRIVERS_H_

#include <inttypes.h>

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

//! Type used instead of uint8_t* pointers to avoid direct dereferencing
typedef uint16_t mem_addr_t;

//! Type for a single value (used instead of uint8_t to increase readability)
typedef uint8_t mem_value_t;

//! Type for memory sizes
typedef uint16_t size_t;

//! Type of a memory driver initialization function
typedef void MemoryInitHnd(void);

//! Type of a memory driver read function
typedef mem_value_t MemoryReadHnd(mem_addr_t addr);

//! Type of a memory driver write function
typedef void MemoryWriteHnd(mem_addr_t addr, mem_value_t value);

//!The data structure for a memory driver such as intSRAM
typedef struct MemDriver {
	MemoryInitHnd* const init;
	MemoryReadHnd* const read;
	MemoryWriteHnd* const write;
	mem_addr_t start;
	size_t size;
} mem_driver_t;


//-----------------------------------------------------------------------------
// Function Headers
//-----------------------------------------------------------------------------

//! Initialize all memory devices
void initMemoryDevices(void);

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

//! Realizes a pointer to the MemDriver intSRAM__
#define intSRAM (&intSRAM__)

//! This specific MemDriver is initialized in os_mem_drivers.c
extern mem_driver_t intSRAM__;


#endif /* OS_MEM_DRIVERS_H_ */