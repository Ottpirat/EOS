/*! \file os_memory.c
 *
 *  Contains functions to allocate blocks of any memory-drivers.
 *  We manage the blocks by creating a lookup table with one nibble per byte payload.
 *  So we have 1/3 map and 2/3 use-heap of each driver.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#include "os_memory.h"
#include "os_memory_strategies.h"
#include "../lib/util.h"
#include "../os_core.h"

//----------------------------------------------------------------------------
// Helper Functions
//----------------------------------------------------------------------------

/*! \brief Writes a value from 0x0 to 0xF to the lower nibble of the given address
 * 
 * \param heap The heap that is written to
 * \param addr The address on which the lower nibble is supposed to be changed
 * \param value The value that the lower nibble of the given addr is supposed to get
 */
inline void setLowNibble(heap_t const* heap, mem_addr_t addr, mem_value_t value)
{
	#warning [Praktikum 4] Implement here
}

/*!
 *  \brief Writes a value from 0x0 to 0xF to the higher nibble of the given address
 * 
 * \param heap The heap that is written to
 * \param addr The address on which the higher nibble is supposed to be changed
 * \param value The value that the higher nibble of the given addr is supposed to get
 */
inline void setHighNibble(heap_t const* heap, mem_addr_t addr, mem_value_t value)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief Reads the value of the lower nibble of the given address
 * 
 * \param heap The heap that is read from
 * \param addr The address which the lower nibble is supposed to be read from
 * \return The value that can be found on the lower nibble of the given address
 */
inline mem_value_t getLowNibble(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief Reads the value of the higher nibble of the given address
 * 
 * \param heap The heap that is read from
 * \param addr The address which the higher nibble is supposed to be read from
 * \return The value that can be found on the higher nibble of the given address
 */
inline mem_value_t getHighNibble(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief This function is used to set a heap map entry on a specific heap
 *
 * \param heap The heap on who's map the entry is supposed to be set
 * \param addr The address in use space for which the corresponding map entry shall be set
 * \param value The value that is supposed to be set onto the map (valid range: 0x0 - 0xF)
 */
void os_setMapEntry(heap_t const* heap, mem_addr_t addr, mem_value_t value)
{
	#warning [Praktikum 4] Implement here
}

/*! This function is used to get a heap map entry on a specific heap
 *
 * \param heap The heap from who's map the entry is supposed to be fetched
 * \param addr The address in use space for which the corresponding map entry shall be fetched
 * \return The value that can be found on the heap map entry that corresponds to the given use space address
 */
mem_value_t os_getMapEntry(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! This function is used to determine where a chunk starts if a given address might not point
 *  to the start of the chunk but to some place inside of it.
 *
 * \param heap The heap the chunk is on hand in
 * \param addr The address that points to some byte of the chunk
 * \return The address that points to the first byte of the chunk
 */
mem_addr_t os_getFirstByteOfChunk(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! This function determines the value of the first nibble of a chunk. 
 * 
 * \param heap The heap the chunk is on hand in
 * \param addr The address that points to some byte of the chunk
 * \return The map entry that corresponds to the first byte of the chunk
 */
process_id_t getOwnerOfChunk(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*!
 *  Takes a use-pointer and computes the length of the chunk.
 *  This only works for occupied chunks. The size of free chunks is always 0.
 *
 *  \param heap The driver to be used.
 *  \param addr An address of the use-heap.
 *  \returns 	The chunk's length.
 */ 
size_t os_getChunkSize(heap_t const* heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! Frees a chunk of allocated memory on the medium given by the driver.
 *  This function checks if the call has been made by someone with the right to do it
 *  (i.e. the process that owns the memory or the OS). This function is made in order to avoid 
 *  code duplication and is called by several functions that, in some way, free allocated memory
 *  such as os_freeProcessMemory/os_free...
 *
 *  \param heap The driver to be used.
 *  \param addr An address inside of the chunk (not necessarily the start).
 *  \param owner The expected owner of the chunk to be freed
 */
void os_freeOwnerRestricted(heap_t* heap, mem_addr_t addr, process_id_t owner)
{
	#warning [Praktikum 4] Implement here
}


//----------------------------------------------------------------------------
//Memory Management
//----------------------------------------------------------------------------

/*!
 *  Allocates a chunk of memory on the medium given by the driver and reserves
 *  it for the current process.
 *
 *  \param heap The driver to be used.
 *  \param size The amount of memory to be allocated in Bytes.
 *              Must be able to handle a single byte and values greater than 255.
 *  \returns 	A pointer to the first Byte of the allocated chunk.\n
 *           	0 if allocation fails (0 is never a valid address).
 */
mem_addr_t os_malloc(heap_t *heap, size_t size)
{
	#warning [Praktikum 4] Implement here
}

/*!
 *  Frees a chunk of allocated memory of the currently running process on the given heap
 *
 *  \param heap The driver to be used.
 *  \param addr An address inside of the chunk (not necessarily the start).
 */
void os_free(heap_t *heap, mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * This function realizes the garbage collection.
 * When called, every allocated memory chunk of the given process is freed
 *
 * \param heap The heap on which we look for allocated memory
 * \param pid The ProcessID of the process that owns all the memory to be freed
 */
void os_freeProcessMemory(heap_t *heap, process_id_t pid)
{
	#warning [Praktikum 4] Implement here
}

/*! 
 * Simple setter function to change the allocation strategy of a given heap
 * \param heap The heap of which the allocation strategy shall be changed
 * \param allocStrat The strategy is changed to allocStrat
 */
void os_setAllocationStrategy(heap_t *heap, alloc_strategy_t allocStrat)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * Simple getter function to fetch the allocation strategy of a given heap
 * 
 * \param heap The heap of which the allocation strategy is returned
 * \return The allocation strategy of the given heap
 */
alloc_strategy_t os_getAllocationStrategy(heap_t const* heap)
{
	#warning [Praktikum 4] Implement here
}

//----------------------------------------------------------------------------
// For testtasks
//----------------------------------------------------------------------------

/*!
 * The Heap-map-size of the heap (used by testtasks)
 *
 * \param heap 	The heap to be used.
 * \returns 	The size of the map of the heap
 */
size_t os_getMapSize(heap_t const* heap)
{
	return heap->mapSize;
}

/*!
 * The Heap-use-size of the heap (used by testtasks)
 *
 * \param heap 	The heap to be used.
 * \returns 	The size of the use-area of the heap
 */
size_t os_getUseSize(heap_t const* heap)
{
	return heap->useSize;
}

/*!
 * The Map-start of the heap (used by testtasks)
 *
 * \param heap 	The heap to be used.
 * \returns 	The first byte of the map of the heap
 */
mem_addr_t os_getMapStart(heap_t const* heap)
{
	return heap->mapStart;
}

/*!
 * The Heap-use-start of the heap (used by testtasks)
 *
 * \param heap 	The heap to be used.
 * \returns 	The first byte of the use-area of the heap
 */
mem_addr_t os_getUseStart(heap_t const* heap)
{
	return heap->useStart;
}