#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_DCF_SHOW_SIGNAL

#include "../../os_core.h"
#include "../../lib/util.h"
#include "../../lib/lcd.h"
#include "../../lib/dcf.h"
#include "../../lib/buttons.h"

//! Forward declarations
void waitForSync();

//! Stores the last 20 samples
uint8_t samples[20];
uint8_t sampleHead = 0;
uint8_t sampleTail = 0;

//! PID of sample process
process_id_t pidSample;

void printHeader()
{
	INFO("Testtask DCF Show Signal");
	INFO("Press SELECT to wait for a SYNC signal or\njust watch the plot.\n");
	
	terminal_writeProgString(PSTR("-----------+----+----+-------------------------------+\n"));
	terminal_writeProgString(PSTR("Plot       |Code|High|Samples                        |\n"));
	terminal_writeProgString(PSTR("-----------+----+----+-------------------------------+\n"));	
}

/*!
 * Display the raw DCF77 signal
 */
PROGRAM(7, AUTOSTART)
{
	uint8_t highCountRow;		// Count number of high samples per 1000 ms
	uint8_t highCountSample;	// Count number of high samples per 100 ms
	uint8_t row = 0;			// Current LCD row
	char level;					// Signal level shown as char
	const char* str;			// Naive decode of signal level
	
	dcf_init();
	
	printHeader();
	
	// Sampling process
	pidSample = os_exec(8, OS_PRIO_HIGH);
	
	while (1)
	{
		// Wait for next sample
		while (sampleHead == sampleTail)
		{
			// Check if the user wants to synchronize
			if (buttons_pressed(BTN_SELECT))
			{
				waitForSync();
				printHeader();
			}
		}
		
		// Print 10 samples per row
		if      (sampleTail == 10) { lcd_line2(); row = 1; }
		else if (sampleTail ==  0) { lcd_clear(); row = 0; }
		
		// Signal is considered HIGH for >= 5 HIGH samples
		
		// Plot signal
		if      (samples[sampleTail] == 0) { level = '_'; }
		else if (samples[sampleTail] <  7) { level = '-'; }
		else                               { level = '^'; }

		lcd_writeChar(level);
		terminal_writeChar(level);
		
		// Decode only at the end of the row
		if (sampleTail == 9 || sampleTail == 19)
		{
			// Sum up current row
			highCountRow = 0;
			for (uint8_t i = 0; i < 10; i++) { highCountRow += samples[row * 10 + i]; }
			
			// Jump to end of corresponding line
			lcd_goto(row, 12);
		
			// Decode bit
			if      (highCountRow ==  0) { str = PSTR("SYNC"); }	// =   0 ms
			else if (highCountRow <  15) { str = PSTR("   0"); }	// < 150 ms
			else if (highCountRow <  25) { str = PSTR("   1"); }	// < 250 ms
			else                         { str = PSTR("   ?"); }
			
			lcd_writeProgString(str);
			terminal_writeProgString(PSTR(" |"));
			terminal_writeProgString(str);
			terminal_writeProgString(PSTR("| "));
			if (highCountRow < 100) { terminal_writeChar(' '); }
			if (highCountRow < 10)  { terminal_writeChar(' '); }
			terminal_writeDec(highCountRow);
			terminal_writeProgString(PSTR("| "));
			
			// Sum up current row
			for (uint8_t i = 0; i < 10; i++)
			{
				highCountSample = samples[row * 10 + i];
				if (highCountSample < 10) { terminal_writeChar(' '); }
				terminal_writeDec(highCountSample);
				terminal_writeChar(' ');
			}
			terminal_writeProgString(PSTR("|\n"));
		
			// Jump back to plot next sample
			lcd_goto(row, sampleTail % 10);
		}
		
		// Increase ring buffer tail
		sampleTail = (sampleTail + 1) % 20;
	}
}

/*!
 * Continuously samples the DCF signal with an interval of 10 ms
 */
PROGRAM(8, DONTSTART)
{
	// Timestamp for sample interval
	time_t t = getSystemTime_ms();
	
	while (1)
	{
		samples[sampleHead] = 0;
		
		// Take 10 samples over 100 ms
		for (uint8_t i = 0; i < 10; i++)
		{
			// Sample in 10 ms intervals
			while (getSystemTime_ms() < t);
			t += 10;
			
			samples[sampleHead] += dcf_getSignal();
			
			os_yield();
		}
		
		// Sample finished, increase ring buffer head
		sampleHead = (sampleHead + 1) % 20;
	}
}

/*!
 * Stops sampling and waits for a SYNC signal to restart sampling in sync
 */
void waitForSync()
{
	// Kill sample process
	os_kill(pidSample);
	
	// Reset sample ring buffer
	sampleHead = 0;
	sampleTail = 0;
	
	terminal_writeChar('\n');
	INFO("Killed sampling process");
	INFO("Waiting for SYNC...");
	
	// Count number of LOW samples
	uint8_t lowCount = 0;
	uint8_t sampleCounter = 0;
	
	// Timestamp for sample interval
	time_t t = getSystemTime_ms();
	
	// Wait the signal to be low for SYNC_MIN_MS
	while (lowCount < (DCF_SYNC_MIN_MS / 10))
	{
		// Sample in 10 ms intervals
		while (getSystemTime_ms() < t);
		t += 10;
		sampleCounter++;
		
		if (!dcf_getSignal())
		{
			lowCount++;
			if (sampleCounter % 10 == 0) { terminal_writeChar('_'); }
		}
		else
		{
			if (lowCount > 0) { terminal_writeProgString(PSTR("^\n")); }
			lowCount = 0;
		}
	}
	
	terminal_writeProgString(PSTR("S\n"));
	INFO("SYNC detected, waiting for rising edge...");
	
	// Wait for the first rising edge after SYNC
	while (!dcf_getSignal());
	
	// Start sampling process
	os_exec(8, OS_PRIO_HIGH);
	os_yield();
	
	INFO("Sampling process started\n");
}

#endif