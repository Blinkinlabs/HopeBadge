#
# Makefile template for ATtiny85
# Derived from AVR Crosspack template
#

DEVICE     = attiny45           # See avr-help for all possible devices
#DEVICE     = attiny25           # See avr-help for all possible devices
CLOCK      = 1000000            # Clock speed
#PROGRAMMER = -c usbtiny -P usb	# For using Adafruit USBtiny
#PROGRAMMER = -c avrisp2		# For using AVR ISP MK2
PROGRAMMER = -C+../production/test/avrdude_gpio.conf -c testrig 	# For using AVR ISP MK2

# Internal oscillator @8MHz, divide clock by 8, fast startup
LFUSE      = 0x42

# Serial Programming enabled, watchdog disabled, reset enabled, brown-out @1.8V
HFUSE      = 0xDE

# Self-programming disabled
EFUSE      = 0xFF
FUSES      = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

TARGET = hopebadge

CC = avr-gcc
CXX = avr-g++


# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -O3 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# compiler options for C++ only
CXXFLAGS = 

# compiler options for C only
CFLAGS = -std=c11

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

C_FILES = \
	hopebadge.c \
	irreceiver.c \
	sleep.c

CPP_FILES =

OBJECTS := $(CPP_FILES:.cpp=.o) $(C_FILES:.c=.o)

# symbolic targets:
all:	$(TARGET).hex

verify-program:
	$(AVRDUDE) -b 1 -U flash:v:$(TARGET).hex:i -U lfuse:v:$(LFUSE):m -U hfuse:v:$(HFUSE):m -U efuse:v:$(EFUSE):m

flash:	all
	$(AVRDUDE) -b 1 -U flash:w:$(TARGET).hex:i

fuse:
	$(AVRDUDE) -b 200 -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

install: fuse flash

clean:
	rm -f $(TARGET).hex $(TARGET).elf $(OBJECTS)

# file targets:
$(TARGET).elf: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $@ $(OBJECTS)

$(TARGET).hex: $(TARGET).elf
	rm -f $(TARGET).hex
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	avr-size --format=avr --mcu=$(DEVICE) $(TARGET).elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	$(TARGET).elf
	avr-objdump -d $(TARGET).elf
