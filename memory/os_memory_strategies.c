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
	mem_addr_t current = start;
    mem_addr_t end = heap->useStart + heap->useSize;

    while (current < end) {
        // Ist das aktuelle Byte frei?
        if (os_getMapEntry(heap, current) == 0x00) {
            size_t free_space = 0;
            mem_addr_t check = current;
            
            // Zählen, wie groß das Loch ist
            while (check < end && os_getMapEntry(heap, check) == 0x00 && free_space < size) {
                free_space++;
                check++;
            }
            
            // Reicht der Platz für unsere Anfrage?
            if (free_space == size) {
                return current; 
            }
            
            // Platz reicht nicht, also überspringen wir dieses nutzlose Loch komplett
            current = check; 
        } else {
            // Block ist belegt -> Springe direkt über den gesamten belegten Chunk!
            // Das spart massiv Rechenzeit, statt jedes Byte einzeln zu prüfen.
            current += os_getChunkSize(heap, current); 
        }
    }
    
    return 0; // Nichts gefunden in diesem Bereich
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
	return os_Memory_RangeFit(heap, size, heap->useStart);
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
	mem_addr_t found = os_Memory_RangeFit(heap, size, heap->lastChunk);
    
    // Wenn wir hinten angekommen sind und nichts gefunden haben: Wrap-Around!
    if (found == 0) {
        found = os_Memory_RangeFit(heap, size, heap->useStart);
    }
    
    // Nur Next-Fit aktualisiert den lastChunk Pointer!
    if (found != 0) {
        heap->lastChunk = found;
    }
    
    return found;
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
	mem_addr_t current = heap->useStart;
    mem_addr_t end = heap->useStart + heap->useSize;
    
    mem_addr_t best_addr = 0;
    size_t best_size = 0xFFFF; // Startet mit unendlich großem Wert

    while (current < end) {
        if (os_getMapEntry(heap, current) == 0x00) {
            size_t free_space = 0;
            mem_addr_t check = current;
            
            // Komplettes Loch ausmessen (kein Abbruch bei 'size' wie bei RangeFit)
            while (check < end && os_getMapEntry(heap, check) == 0x00) {
                free_space++;
                check++;
            }
            
            // Ist das Loch groß genug UND kleiner als unser bisher bester Treffer?
            if (free_space >= size && free_space < best_size) {
                best_size = free_space;
                best_addr = current;
            }
            current = check; 
        } else {
            current += os_getChunkSize(heap, current); 
        }
    }
    
    return best_addr;
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
	mem_addr_t current = heap->useStart;
    mem_addr_t end = heap->useStart + heap->useSize;
    
    mem_addr_t worst_addr = 0;
    size_t worst_size = 0; 

    while (current < end) {
        if (os_getMapEntry(heap, current) == 0x00) {
            size_t free_space = 0;
            mem_addr_t check = current;
            
            // Komplettes Loch ausmessen
            while (check < end && os_getMapEntry(heap, check) == 0x00) {
                free_space++;
                check++;
            }
            
            // Ist das Loch groß genug UND größer als unser bisher härtester Treffer?
            if (free_space >= size && free_space > worst_size) {
                worst_size = free_space;
                worst_addr = current;
            }
            current = check; 
        } else {
            current += os_getChunkSize(heap, current); 
        }
    }
    
    return worst_addr;
}