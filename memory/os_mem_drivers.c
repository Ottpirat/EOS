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
// #include <util.h>

//! Forward declarations
static void initSRAM_internal(void);
static mem_value_t readSRAM_internal(mem_addr_t addr);
static void writeSRAM_internal(mem_addr_t addr, mem_value_t value);

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
static void initSRAM_internal(void)
{
	//os_printf("SRAM INIT: Erfolgreich");
}

/*! \brief Private function to read a value from the internal SRAM
 * It will not check if its call is valid. This has to be done on a higher level.
 *
 * \param addr The address to read the value from
 * \return The read value
 */
static mem_value_t readSRAM_internal(mem_addr_t addr)
{
	mem_value_t value = *((volatile uint8_t*) addr);
	return value;
}

/*! \brief Private function to write a value to the internal SRAM
 * It will not check if its call is valid. This has to be done on a higher level.
 *
 * \param addr The address the value shall be written to
 * \param value The value to be written
 */
static void writeSRAM_internal(mem_addr_t addr, mem_value_t value)
{
	// *((volatile uint8_t*) addr) == das speicherbyte, auf das addr zeigt 
	*((volatile uint8_t*) addr) = value;
}

/*! \brief Function that needs to be called once in order to initialize all used memories such as the internal SRAM etc...
 */
void initMemoryDevices(void)
{
	#warning [Praktikum 4] Implement here

}


