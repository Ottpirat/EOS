/*! \file
 *  \brief Temperature sensor library
 *  
 *  Helpers for setting up and reading from the ADC.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#include "tempSensor.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util.h"
#include "../os_scheduler.h"
#include "lcd.h"

// Port definitions
#define TMP_DDR				DDRK
#define TMP_PORT			PORTK
#define TMP_GND_PIN			PK5		// Pin A13
#define TMP_VCC_PIN			PK6		// Pin A14
#define TMP_ADC_CHANNEL		15		// Pin A15

/*!
 * Initializes ADC
 */
void tempSensor_init(void)
{
	#warning [Praktikum 5] Implement here

	// Configure and set the temp sensor pins: GND and Vcc (A13 and A14)

	// ADMUX: Reference voltage to AVcc

    // ADCSRA: Enable ADC and set prescaler to 128

}

/*!
 * Sets ADC channel and reads a single value
 *
 * \return The 10-bit ADC value
 */
uint16_t adc_read(void)
{
	#warning [Praktikum 5] Implement here

	// Secure exclusive access to ADC

	// Set ADC channel:
	// ADCSRB: Select the "bank" of the ADC:
	// ADC0 - ADC7  -> MUX5 = 0
	// ADC8 - ADC15 -> MUX5 = 1

	// ADMUX: Set pin of the bank (only modify MUX2:0)

}

/*!
 * Converts given ADC value to a temperature in degree Celsius
 *
 * \param adcValue The raw ADC value read from the temperature sensor
 * \return Temperature in degree Celsius
 */
temperature_t tempSensor_convert(uint16_t adcValue)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Reads the ADC value and converts it to a temperature in degree Celsius
 *
 * \return Temperature in degree Celsius
 */
temperature_t tempSensor_read(void)
{
	#warning [Praktikum 5] Implement here
}

/*!
 * Prints the given temperature on the LCD.
 * Format: " XX.X", "  X.X" or "-XX.X" with leading spaces
 * 
 * \param t Temperature to print
 */
void lcd_writeTemperature(temperature_t t)
{
	#warning [Praktikum 5] Implement here

	// Handle negative temperatures

	// Print temperature in the defined format

}