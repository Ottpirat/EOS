/*! \file os_memory_strategies.c
 *
 * Memory strategies used by the malloc function of os_memory to allocate blocks on the memory.
 *
 * The file contains four strategies:
 * - First-Fit
 * - Next-Fit
 * - Best-Fit
 * - Worst-Fit
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#include "os_memory_strategies.h"
#include "os_memory.h"

/*!
 * \brief Helper to realize FirstFit and NextFit
 *
 * \param heap The heap in which we want to find a free chunk
 * \param size The size of the desired free chunk
 * \param start Start address for the search
 * \return The first address of the found free chunk, or 0 if no chunk was found.
 */
mem_addr_t os_Memory_RangeFit(heap_t* heap, size_t size, mem_addr_t start)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * \brief This function realizes the first-fit strategy as described in the corresponding exercise document
 *
 * \param heap The heap in which we want to find a free chunk
 * \param size The size of the desired free chunk
 * \return The first address of the found free chunk, or 0 if no chunk was found.
 */
mem_addr_t os_Memory_FirstFit(heap_t* heap, size_t size)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * \brief This function realizes the next-fit strategy as described in the corresponding exercise document
 *
 * \param heap The heap in which we want to find a free chunk
 * \param size The size of the desired free chunk
 * \return The first address of the found free chunk. Returns 0, if no chunk was found.
 */
mem_addr_t os_Memory_NextFit(heap_t* heap, size_t size)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * \brief This function realizes the best-fit strategy as described in the corresponding exercise document
 *
 * \param heap The heap in which we want to find a free chunk
 * \param size The size of the desired free chunk
 * \return The first address of the found free chunk. Returns 0, if no chunk was found.
 */
mem_addr_t os_Memory_BestFit(heap_t* heap, size_t size)
{
	#warning [Praktikum 4] Implement here
}

/*!
 * \brief This function realizes the worst-fit strategy as described in the corresponding exercise document
 *
 * \param heap The heap in which we want to find a free chunk
 * \param size The size of the desired free chunk
 * \return The first address of the found free chunk. Returns 0, if no chunk was found.
 */
mem_addr_t os_Memory_WorstFit(heap_t* heap, size_t size)
{
	#warning [Praktikum 4] Implement here
}