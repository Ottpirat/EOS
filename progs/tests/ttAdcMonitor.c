#include "../progs.h"
#if defined(TESTTASK_ENABLED) && (TESTTASK == TT_ADC_MONITOR)

#include "../../os_process.h"
#include "../../lib/tempSensor.h"
#include "../../lib/terminal.h"
#include "../../lib/lcd.h"

PROGRAM(1, AUTOSTART)
{
    // Initialize the ADC
    tempSensor_init();

    // Initialize the LCD
    lcd_init();
    lcd_clear();
	lcd_writeProgString(PSTR("ADC-Test:"));
    INFO("Starting ADC-Monitor...");
    
    while (1)
    {
        // Read the raw ADC value
        uint16_t adcValue = adc_read();
        
        // Print the ADC value on the LCD
        lcd_goto(1, 0);
        lcd_writeDec(adcValue);
		lcd_writeProgString(PSTR("   "));
		INFO("ADC: %d", adcValue);
        
        delayMs(200);
    }
}

#endif