/*! \file
 *  \brief Temperature sensor library
 *  
 *  Helpers for setting up and reading from the ADC.
 *
 *  \author   Fachbereich 5 - FH Aachen
 *  \date     2025
 *  \version  1.0
 */
#ifndef _TEMP_SENSOR_H
#define _TEMP_SENSOR_H

#include <stdint.h>

// TO_BE_DONE(TASK_5)
//! Define a data type temperature_t to manage temperatures
#warning The datatype uint16_t is only a placeholder and not meant to be used
typedef uint16_t temperature_t;

//! Initializes the ADC
void tempSensor_init(void);

//! Sets ADC channel and reads a single value
uint16_t adc_read(void);

//! Reads the ADC value and converts it to a temperature in degree Celsius
temperature_t tempSensor_read(void);

//! Writes the temperature to the LCD based on the ADC value
void lcd_writeTemperature(temperature_t temp);

#endif
