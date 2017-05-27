#ifndef HOPEBADGE_H_
#define HOPEBADGE_H_

// Define pins
#define PIN_LED1	PB0	// LED 1 output (active low)
#define PIN_LED2	PB1	// LED 2 output (active low)
#define PIN_IR_ENABLE	PB2	// IR enable; pulled low externally
#define PIN_LED3	PB3	// LED 3 output (active low)
#define PIN_IR_DATA	PB4	// IR Data input
#define PIN_RESET	PB5	// Reset switch; requires high voltage programmer


// System parameters
#define SLEEP_TIME		200 // Time to sleep between cycles, in 10ths of a second
#define IR_MONITOR_TIME         20  // Amount of time to stay awake for IR reception, in 10ths of a second

//#define SERIAL_DEBUG

#define bitSet(reg, bit) reg |= (1<<bit)
#define bitClear(reg, bit) reg &= ~(1<<bit)

#define setLEDs(value) OCR1A = value

#include <avr/io.h>
#include <util/delay.h>

void pulseOut(uint8_t data);

#endif
