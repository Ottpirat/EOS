/*! \file buttons.c
 *  \brief Handles button presses and releases (pin change interrupt).
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2024
 *  \version  1.0
 */
#include "buttons.h"
#include "util.h"
#include "../os_scheduler.h"

#include <avr/io.h>
#include <stdbool.h>

/*!
 *  Initializes the buttons.
 */
void buttons_init()
{
	// Set PF0 (ADC0) as input
	cbi(DDRF, PF0);
	
	// Select Vref = AVcc
	sbi(ADMUX, REFS0);
	
	// Enable ADC and set prescaler to 128
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

/*!
 *  Read the button that is currently pressed
 *
 *  \return the button that is currently pressed
 */
button_t buttons_read()
{
	uint16_t value;

	// Secure exclusive access to ADC
	os_enterCriticalSection();
	
	// Set to channel ADC0 (PF0): clear MUX5:0 bits
	cbi(ADMUX,  MUX0);
	cbi(ADMUX,  MUX1);
	cbi(ADMUX,  MUX2);
	cbi(ADMUX,  MUX3);
	cbi(ADMUX,  MUX4);
	cbi(ADCSRB, MUX5);
	
	// Start conversion
	sbi(ADCSRA, ADSC);
	
	// Wait until conversion is complete
	while (gbi(ADCSRA, ADSC));
	
	// ADC is a 10-bit register, so you get a value between 0 and 1023
	value = ADC;

	os_leaveCriticalSection();

	// Return the button that was pressed
	if      (value <  66) { return BTN_RIGHT;  }
	else if (value < 219) { return BTN_UP;     }
	else if (value < 393) { return BTN_DOWN;   }
	else if (value < 600) { return BTN_LEFT;   }
	else if (value < 872) { return BTN_SELECT; }
	else                  { return BTN_NONE;   }
}

/*!
 *  Check if button got pressed.
 *
 * 	\param button button to check
 *  \return true if only the given button is currently pressed
 */
bool buttons_pressed(button_t button)
{
	return buttons_read() == button;
}

/*!
 *  Check if button got released.
 *
 * 	\param button button to check
 *  \return true if any other button or none is pressed
 */
bool buttons_released(button_t button)
{
	return buttons_read() != button;
}

/*!
 *  Blocks until button got pressed
 *	\param button button to wait for
 */
void buttons_waitForPressed(button_t button)
{
	while (!buttons_pressed(button));
}

/*!
 *  Blocks until button got released
 *
 * \param button button to wait for
 */
void buttons_waitForReleased(button_t button)
{
	while (!buttons_released(button));
}
