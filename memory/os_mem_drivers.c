/*! \file os_mem_driver.c
 *
 *  Memory Driver structs and their functions.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#include "os_mem_drivers.h"
#include "../lib/defines.h"

//! Forward declarations
void initSRAM_internal(void);
mem_value_t readSRAM_internal(mem_addr_t addr);
void writeSRAM_internal(mem_addr_t addr, mem_value_t value);

//! Driver instantiation for the internal SRAM memory
mem_driver_t intSRAM__ =
{
	.init = initSRAM_internal,
	.read = readSRAM_internal,
	.write = writeSRAM_internal,
	.start = AVR_SRAM_START,
	.size = AVR_MEMORY_SRAM
};

/*! \brief Pseudo-function to initialize the internal SRAM
 * Actually, there is nothing to be done when initializing the internal SRAM
 */
void initSRAM_internal(void)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief Private function to read a value from the internal SRAM
 * It will not check if its call is valid. This has to be done on a higher level.
 *
 * \param addr The address to read the value from
 * \return The read value
 */
mem_value_t readSRAM_internal(mem_addr_t addr)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief Private function to write a value to the internal SRAM
 * It will not check if its call is valid. This has to be done on a higher level.
 *
 * \param addr The address the value shall be written to
 * \param value The value to be written
 */
void writeSRAM_internal(mem_addr_t addr, mem_value_t value)
{
	#warning [Praktikum 4] Implement here
}

/*! \brief Function that needs to be called once in order to initialize all used memories such as the internal SRAM etc...
 */
void initMemoryDevices(void)
{
	#warning [Praktikum 4] Implement here
}


