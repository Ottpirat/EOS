#include "../progs.h"
#if defined(TESTTASK_ENABLED) && (TESTTASK == TT_DCF_GENERATOR || TESTTASK == TT_DCF_DECODE_TIME)

#include "../../os_process.h"
#include "../../lib/dcf.h"
#include "../../lib/terminal.h"
#include "../../lib/lcd.h"

// Enable debug output to the terminal
// #define PRINT_TO_TERMINAL

// DCF77 output pin: AVR PA0 == Arduino Pin D22
#define DCF77_OUT_PIN	PA0 
#define DCF77_OUT_DDR	DDRA
#define DCF77_OUT_PORT	PORTA

typedef struct DCF77
{
    uint8_t M;					// Start bit (always 0)
    uint8_t weather[14];		// Weather information
    uint8_t R;					// Antenna status
    uint8_t A1;					// DST switch announcement
    uint8_t Z[2];				// Time zone
    uint8_t A2;					// Leap second announcement
    uint8_t S;					// Time start bit (always 1)

    uint8_t minute[7];			// Minute bits
    uint8_t minuteParity;		// Minute parity bit
    uint8_t hour[6];			// Hour bits
    uint8_t hourParity;			// Hour parity bit

    uint8_t day[6];				// Day bits
    uint8_t dayOfWeek[3];		// Day of week bits
    uint8_t month[5];			// Month bits
    uint8_t year[8];			// Year bits
    uint8_t dateParity;			// Year parity bit

    uint8_t synchronization;	// Synchronization bit (no transmission)
} dcf77_t;

//! increment the simulated datetime by one minute
void incrementDateTime(dcfDateTime_t *dateTime)
{
    dateTime->minutes++;
	
	// Next hour
    if(dateTime->minutes >= MINUTES_PER_HOUR)
	{
        dateTime->minutes = 0;
        dateTime->hours++;
		
		// Next day
        if(dateTime->hours >= HOURS_PER_DAY)
		{
            dateTime->hours = 0;
            dateTime->dayOfWeek++;
            if(dateTime->dayOfWeek > DAYS_PER_WEEK) { dateTime->dayOfWeek = 1; }
            dateTime->day++;
			
			// Next month (we always assume a month to have 31 days)
            if(dateTime->day > DAYS_PER_MONTH)
			{
                dateTime->day = 1;
                dateTime->month++;
				
				// Next year
                if(dateTime->month > MONTHS_PER_YEAR)
				{
                    dateTime->month = 1;
                    dateTime->year++;
                }
            }
        }
    }
}

//! Encode value into the corresponding DCF77 bit representation
void encodeBit(uint8_t value, uint8_t *startBit, uint8_t length)
{
    static uint8_t bitValues[] = {1, 2, 4, 8, 10, 20, 40, 80};
	
	// i >= 0 does not work since i is unsigned
    for (uint8_t i = length; i != 255; i--)
	{
        if (value >= bitValues[i])
		{
            startBit[i] = 1;
            value -= bitValues[i];
        } 
		else
		{
            startBit[i] = 0;
        }
    }
}

//! Calculate even parity for the given data and write it to the next byte
void encodeParity(uint8_t *startBit, uint8_t length)
{
    bool parity = false;
    for (uint8_t i = 0; i < length; i++) { parity ^= startBit[i]; }
    startBit[length] = parity;
}

//! Write the DCF77 data to the terminal
void terminal_writeDateTime(dcfDateTime_t *dateTime)
{
    INFO("Time: %02d:%02d", dateTime->hours, dateTime->minutes);
    INFO("Date: %S %02d/%02d/%04d", dateTime->dayOfWeek > DAYS_PER_WEEK ? PSTR("??") : strWeekday[dateTime->dayOfWeek],
	dateTime->day, dateTime->month, dateTime->year);
}

//! Convert DateTime to DCF77 format
void dateTimeToDcf77(dcfDateTime_t *dateTime, dcf77_t *signal)
{
    encodeBit(dateTime->minutes, signal->minute, 7);
    encodeParity(signal->minute, 7);

    encodeBit(dateTime->hours, signal->hour, 6);
    encodeParity(signal->hour, 6);

    encodeBit(dateTime->day, signal->day, 6);
    encodeBit(dateTime->dayOfWeek, signal->dayOfWeek, 2);
    encodeBit(dateTime->month, signal->month, 5);
    encodeBit(dateTime->year - 2000, signal->year, 8);
    encodeParity(signal->day, 22);
}

//! More acuate delay function that waits until the specified point in time is reached, minimizing drift
void waitUntil(uint16_t ms)
{
    static time_t waitUntil = 0;

    if (waitUntil == 0) { waitUntil = getSystemTime_ms(); }

    waitUntil += ms;
    while (getSystemTime_ms() < waitUntil);
}

//! Compare two dcfDateTime_t structs for equality (excluding systemTimeOffset)
bool dateTimeEquals(dcfDateTime_t *a, dcfDateTime_t *b)
{
    return (a->hours == b->hours && 
			a->minutes == b->minutes &&
            a->day == b->day &&
			a->month == b->month &&
            a->year == b->year &&
			a->dayOfWeek == b->dayOfWeek);
}

#if TESTTASK == TT_DCF_GENERATOR

PROGRAM(8, AUTOSTART)
{
    // Buffer for the generated DCF77 signal
	dcf77_t signal =
	{
        M: 0,
        weather: {1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        R: 0,
        A1: 0,
        Z: {0, 1},
        A2: 0,
        S: 1,
        minute: {0, 0, 0, 0, 0, 0, 0},
        minuteParity: 0,
        hour: {0, 0, 0, 0, 0, 0},
        hourParity: 0,
        day: {0, 0, 0, 0, 0, 0},
        dayOfWeek: {0, 0, 0},
        month: {0, 0, 0, 0, 0},
        year: {0, 0, 0, 0, 0, 0, 0, 0},
        dateParity: 0,
        synchronization: 2 // No transmission
    };
	
    // Simulated date and time
    dcfDateTime_t simulatedDateTime =
	{
        .hours = 1,
        .minutes = 22, // will be incremented before the first complete signal is sent
        .day = 5,
        .month = 6,
        .year = 2007,
        .dayOfWeek = 2,
    };
	
    uint8_t seconds = 57; // Start at 57 seconds to trigger sync soon after start

    // Set the DCF77 output pin to output
    sbi(DCF77_OUT_DDR, DCF77_OUT_PIN);
    cbi(DCF77_OUT_PORT, DCF77_OUT_PIN);

    INFO("Starting DCF77 signal generator...");

    while (1)
	{
        // Simulate the DCF77 signal
        switch (((uint8_t*)(&signal))[seconds])
		{
            case 0: // 100 ms High signal == 0 bit
                sbi(DCF77_OUT_PORT, DCF77_OUT_PIN);

                #ifdef PRINT_TO_TERMINAL
                terminal_writeChar('0');
                #endif

                waitUntil(DCF_HIGH_MS);

                cbi(DCF77_OUT_PORT, DCF77_OUT_PIN);

                seconds++;
                waitUntil(DCF_SEP_TICKS - DCF_HIGH_MS);
                break;

            case 1:  // 200 ms High signal == 1 bit
                sbi(DCF77_OUT_PORT, DCF77_OUT_PIN);

                #ifdef PRINT_TO_TERMINAL
                terminal_writeChar('1');
                #endif

                waitUntil(DCF_LOW_MS);
                cbi(DCF77_OUT_PORT, DCF77_OUT_PIN);

                seconds++;
                waitUntil(DCF_SEP_TICKS - DCF_LOW_MS);
                break;
            
            default: // 59th second
                incrementDateTime(&simulatedDateTime);
                seconds = 0;
                dateTimeToDcf77(&simulatedDateTime, &signal);

                #ifdef PRINT_TO_TERMINAL
                terminal_writeChar('S');
                terminal_newLine();
                #endif
                
                #ifdef PRINT_TO_TERMINAL
                terminal_newLine();
                INFO("Starting new second:");
                terminal_writeDateTime(&simulatedDateTime);
                terminal_writeProgString(PSTR("        Sending signal:\n\r        "));
                #endif
        
                waitUntil(1000);
                break;
		}
    }
}

#else

PROGRAM(1, AUTOSTART)
{
    dcfDateTime_t dt =
	{
        .hours = 0,
        .minutes = 0,
        .day = 0,
        .month = 0,
        .year = 2000,
        .dayOfWeek = 1,
    };
	
    dcf77_t signal =
	{
        M: 0,
        weather: {1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        R: 0,
        A1: 0,
        Z: {0, 1},
        A2: 0,
        S: 1,
    };
	
    dcf_status = DS_WAITING;

	dcf_init();
    lcd_writeProgString(PSTR("Decoder Test"));
    INFO("Starting DCF77 signal decoder test...");
    
    // 1: Valid signals
    lcd_goto(1, 0);
    lcd_writeProgString(PSTR("Valid: "));
    terminal_writeProgString(PSTR("Valid signals:"));
	
    for (uint8_t i = 0; i < 60; i++)
	{
        // Increment the simulated date time
        dt.minutes++;
        dt.hours = (dt.hours + 1) % HOURS_PER_DAY;
        dt.day = (dt.day + 1) % DAYS_PER_MONTH;
        dt.month = (dt.month + 1) % MONTHS_PER_YEAR;
        dt.year++;
        dt.dayOfWeek = (dt.dayOfWeek % DAYS_PER_WEEK) + 1; // 1=Monday, ..., 7=Sunday
        
        // Convert the date time to DCF77 format
        dateTimeToDcf77(&dt, &signal);
        
        // Decode the signal
        dcf_status = DS_WAITING;
        dcf_decodeTime((uint8_t*)&signal);
        if (dcf_status != DS_SUCCESS)
		{
            lcd_writeProgString(PSTR("Fail"));
            terminal_writeProgString(PSTR("Fail:\n\r"));
            ERROR("decodeTime failed with status: %d for:", dcf_status);
            terminal_writeDateTime(&dt);
            while (1);
        }
        if (!dateTimeEquals(&dt, dcf_getDateTime()))
        {
            lcd_writeProgString(PSTR("Fail"));
            terminal_writeProgString(PSTR("Fail:\n\r"));
            ERROR("decodeTime got unexpected date time:");
            terminal_writeDateTime(dcf_getDateTime());
            terminal_writeProgString(PSTR("\n\rExpected: "));
            terminal_writeDateTime(&dt);
            while (1);
        }
    }
	
    lcd_writeProgString(PSTR("OK"));
    terminal_writeProgString(PSTR("OK\n\r"));

    delayMs(1000);

    // 2: Invalid signals
    lcd_goto(1, 0);
    lcd_writeProgString(PSTR("Invalid: "));
    terminal_writeProgString(PSTR("Invalid signals:"));
    dateTimeToDcf77(&dt, &signal);

    // Prepare to set invalid values in dcfDateTime to be certain that they where not overwritten
    dcfDateTime_t invalidDateTime =
    {
        .minutes = 99,
        .hours = 99,
        .day = 9,
        .month = 99,
        .year = 3000,
        .dayOfWeek = 8,
        .systemTimeOffset = 99999
    };
	
    for (uint8_t i = 1; i < 59; i++)
	{
        // Reset dcfDateTime to known values (invalid)
        dcfDateTime = invalidDateTime;

        // Introduce an error in the signal
        ((uint8_t*)&signal)[i-1] = !((uint8_t*)&signal)[i-1];	// Undo last times bit flip
        ((uint8_t*)&signal)[i]   = !((uint8_t*)&signal)[i];		// Flip one bit in the signal
        
        // Decode the signal
        dcf_status = DS_WAITING;
        dcf_decodeTime((uint8_t*)&signal);
        decodeStatus_t expectedStatus;
        if (i==20)                   { expectedStatus = DS_FAIL_START; }			// Start bit error
        else if (21 <= i && i <= 28) { expectedStatus = DS_FAIL_MINUTE_PARITY; }	// Minute parity error
        else if (29 <= i && i <= 35) { expectedStatus = DS_FAIL_HOUR_PARITY; }		// Hour parity error
        else if (36 <= i && i <= 58) { expectedStatus = DS_FAIL_DATE_PARITY; }		// Date parity error
        else                         { expectedStatus = DS_SUCCESS; }				// No error
        
		if (dcf_status != expectedStatus)
		{
            terminal_writeProgString(PSTR("\n\r err:"));
            for (uint8_t j = 0; j < sizeof(signal); j++) { terminal_writeChar(((uint8_t*)&signal)[j] ? '1' : '0'); }
            lcd_writeProgString(PSTR("Fail"));
            terminal_writeProgString(PSTR("Fail:\n\r"));
            ERROR("decodeTime got unexpected status: %d, expected: %d for bitflip at %d:", dcf_status, expectedStatus, i);
            terminal_writeDateTime(&dt);
            while (1);
        }

        if (expectedStatus != DS_SUCCESS &&
            (!dateTimeEquals(&invalidDateTime, dcf_getDateTime()) || 
            dcf_getDateTime()->systemTimeOffset != 99999))
        {
            lcd_writeProgString(PSTR("Fail"));
            terminal_writeProgString(PSTR("Fail:\n\r"));
            ERROR("decodeTime modified dcfDateTime unexpectedly at bitflip %d:", i);
            terminal_writeDateTime(dcf_getDateTime());
            terminal_writeProgString(PSTR("\n\rExpected:\n\r"));
            terminal_writeDateTime(&invalidDateTime);
            terminal_writeProgString(PSTR("\n\rSystem time offset: "));
            terminal_writeDec(dcf_getDateTime()->systemTimeOffset);
            terminal_writeProgString(PSTR("\n\rExpected: 99999\n\r"));
            while (1);
        }
    }
    lcd_writeProgString(PSTR("OK"));
    terminal_writeProgString(PSTR("OK\n\r"));

    delayMs(1000);

    lcd_goto(1, 0);
    lcd_writeProgString(PSTR("     All OK!    "));
    terminal_writeProgString(PSTR("All OK!\n\r"));
    while (1);
}

#endif

#endif
