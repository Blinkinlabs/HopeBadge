#ifndef HOPEBADGE_H_
#define HOPEBADGE_H_

// Define pins
#define PIN_LED1        PB0	// LED 1 output (active low)
#define PIN_LED2        PB1	// LED 2 output (active low)
#define PIN_IR_DATA     PB4	// IR Data input
#define PIN_LED3        PB3	// LED 3 output (active low)
#define PIN_IR_ENABLE   PB2	// IR enable (active high)
#define PIN_RESET       PB5	// Reset switch; requires high voltage programmer


// System parameters
#define SLEEP_TIME          (200)   // Time to sleep between cycles, in ms
#define IR_SEARCH_TIME      (16*1)  // Amount of time to stay awake searching for a signal
#define IR_RECEIVE_TIME     (16*4)  // Amount of time to stay awake listening for a complete signal, after a search detects activity

// Flash parameters
#define FLASH_REPEAT_COUNT  30 // Number of times the LED flashing cycle s repeated
#define FLASH_DELAY         32 // Amount of time to delay between each step of the flash cycle (ms). Rounded to multiples of 16ms.


#define bit_set(reg, bit) reg |= (1<<bit)
#define bit_clear(reg, bit) reg &= ~(1<<bit)

//#define SERIAL_DEBUG

#endif
