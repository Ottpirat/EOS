# Makefile for your Atmega2560 project

MCU=atmega2560
PROGRAMMER=wiring
PARTNO=m2560
#PORT = $(shell ls /dev/ttyACM* 2>/dev/null | head -n 1)
PORT = $(shell ls /dev/ttyACM* 2>/dev/null | tail -n 1)
# Define the source files
SRC = $(wildcard *.c) $(wildcard */*.c) $(wildcard progs/*/*.c)
HEADERS = $(wildcard *.h) $(wildcard */*.h) $(wildcard progs/*/*.h)
HEADER_DIRS = -I /usr/lib/avr/include/ -I./ -I./lib -I./progs -I./progs/tests -I./progs/user_programs

# Define the object files
OBJ = $(SRC:.c=.o)

# Define the compiler and linker
CC = avr-gcc
CFLAGS = -c -mmcu=$(MCU) -Wall -g -Og -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -std=gnu99
# OLD_CFLAGS = -fno-jump-tables -fshort-enums
#CFLAGS = -c -mmcu=$(MCU) -Wall -g -O1 -fno-jump-tables
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

# Define the final executable
TARGET = main.hex

SILENT = @
ECHO = echo
MAKE = make

all: headers-changed $(TARGET) flash

#.PHONY: main
#main:
#	echo "Compiling the following files:"
#	ls *.c */*.c progs/*/*.c
#	avr-gcc -mmcu=$(MCU) -Wall -g -Og -o main.o -I./ -I./lib -I./progs -I./progs/tests -I./progs/user_programs *.c */*.c progs/*/*.c
#	avr-objcopy -j .text -j .data -O ihex main.o main.hex

# Rule for compiling source files into object files
%.o: %.c
	$(SILENT) $(ECHO) "Compiling $<..."
	$(SILENT) $(CC) $(CFLAGS) $(HEADER_DIRS) -c $< -o $@

# Rule for linking object files into the final executable
$(TARGET): $(OBJ)
	$(SILENT) $(ECHO) "Linking object files..."
	$(SILENT) $(CC) $(LDFLAGS) $(OBJ) -o main.elf
	$(SILENT) $(ECHO) "Creating HEX file..."
	$(SILENT) avr-objcopy -j .text -j .data -O ihex main.elf $(TARGET)

# Define the main target
#main: $(TARGET)

# Flash the target to the board
.PHONY: flash
flash:
	avrdude -V -D -p $(PARTNO) -c $(PROGRAMMER) -U flash:w:$(TARGET):i -F -P $(PORT) -D

# clean if headers have changed
.PHONY: headers-changed
headers-changed: $(HEADERS)
	$(SILENT) $(ECHO)  "Header files changed: $< -> recompling all"
	$(SILENT) $(MAKE) clean

# Clean up build files
.PHONY: clean
clean:
	$(SILENT) $(ECHO) "Cleaning up..."
	$(SILENT) find . -type f \( -name "*.o" -o -name "*.hex" -o -name "*.elf" \) -exec rm -f {} +

