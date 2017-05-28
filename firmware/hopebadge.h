#ifndef HOPEBADGE_H_
#define HOPEBADGE_H_

// Define pins
#define PIN_LED1	PB0	// LED 1 output (active low)
#define PIN_LED2	PB1	// LED 2 output (active low)
#define PIN_IR_DATA	PB4	// IR Data input
#define PIN_LED3	PB3	// LED 3 output (active low)
#define PIN_IR_ENABLE	PB2	// IR enable (active high)
#define PIN_RESET	PB5	// Reset switch; requires high voltage programmer


// System parameters
#define SLEEP_TIME		500 // Time to sleep between cycles, in ms
#define IR_MONITOR_TIME         150 // Amount of time to stay awake for IR reception


#define bit_set(reg, bit) reg |= (1<<bit)
#define bit_clear(reg, bit) reg &= ~(1<<bit)

//#define SERIAL_DEBUG

#endif
