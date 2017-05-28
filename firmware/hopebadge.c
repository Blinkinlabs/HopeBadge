/* Name: main.c
 * Hope Badge
 *
 * Copyright 2017 Blinkinlabs, LLC
 * Author: Matt Mets
 */

#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "hopebadge.h"

#include "irremote.h"
#include "crc.h"
#include "sleep.h"

#ifdef SERIAL_DEBUG
// Bitbang a 9600 baud serial output
void pulse_out(uint8_t data) {
    const uint8_t BIT_DELAY = 100; // ~9600 baud

    // start bit
    bit_clear(PORTB, PIN_UNUSED);
    _delay_us(BIT_DELAY);

    // data bits, lsb first
    for(uint8_t bit = 0; bit < 8; bit++) {
        if((data >> bit) & 1) {
            bit_set(PORTB, PIN_UNUSED);
        }
        else {
            bit_clear(PORTB, PIN_UNUSED);
        }
        _delay_us(BIT_DELAY);
    }

    // Stop bits
    bit_set(PORTB, PIN_UNUSED);
    _delay_us(BIT_DELAY);
    _delay_us(BIT_DELAY);
}
#endif



static inline void set_cpu_frequency_4mhz() {
    // Change the clock to 4MHz
    __asm__ volatile (
                      "st Z,%1" "\n\t"
                      "st Z,%2"
                      : :
                      "z" (&CLKPR),
                      "r" ((uint8_t) (1<<CLKPCE)),
                      "r" ((uint8_t) 1)  // new CLKPR value 0=8MHz, 1=4MHz, 2=2MHz, 3=1MHz)
                      );
}


// Listen for an IR command
// Duration: x seconds
static inline bool monitor_ir() {
    // If we didn't get an NEC command, bail
    if (!decodeIR()) {
        return false;
    }

    uint8_t speed =         (results.value >> 24) & 0xFF;
    uint8_t repeats =       (results.value >> 16) & 0xFF;
    uint8_t sensitivity =   (results.value >>  8) & 0xFF;

    resetCRC();
    updateCRC(speed);
    updateCRC(repeats);
    updateCRC(sensitivity);

    // If the CRC is valid, store the results.
    if(getCRC() != (results.value & 0xFF)) {
        return false;
    }

    return true;
}


static inline void flash_lights(uint8_t count) {
    for(uint8_t i = 0; i < count; i++) {
        bit_clear(PORTB, PIN_LED1);
        sleep_ms(32);
        bit_set(PORTB, PIN_LED1);

        bit_clear(PORTB, PIN_LED2);
        sleep_ms(32);
        bit_set(PORTB, PIN_LED2);

        bit_clear(PORTB, PIN_LED3);
        sleep_ms(32);
        bit_set(PORTB, PIN_LED3);

        wdt_reset();
   }

}


static inline void setup() {
    // Turn on the watchdog
    watchdog_setup();
    
    // Configure the processor for 4MHz operation (for voltage spec compliance)
    set_cpu_frequency_4mhz();

    // Set the LEDs to outputs and disable
    DDRB = (1<<PIN_LED1)
           | (1<<PIN_LED2)
           | (1<<PIN_LED3)
           | (1<<PIN_IR_ENABLE);

    // Disable the LEDs and the IR receiver
    PORTB = (1<<PIN_LED1)
            | (1<<PIN_LED2)
            | (1<<PIN_LED3);

    bit_set(ACSR, ACD);          // Disable the analog comparitor
    bit_clear(ADCSRA, ADEN);     // Disable the ADC

    // Note: Watchdog disabled by fuse setting
    
    // Disable clicks to peripherals that we aren't using
    // (This saves power in run mode)
    PRR |= _BV(PRTIM0) | _BV(PRTIM1) | _BV(PRUSI) | _BV(ADC);
    
    // Configure the sleep mode and wakeup interrupt
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Use power down mode for sleep
}

static inline void loop() {
    // Enable IR receiver
    bit_set(PORTB, PIN_IR_ENABLE);  // Power up IR receiver
    PRR &= ~_BV(PRTIM0); // Turn on timer0 clock
    enableIRIn(); // Start the IR receiver state machine

    // Wait for IR reciver
    // TODO: can we light sleep here using the watchdog?
    _delay_ms(IR_MONITOR_TIME); // Wait a while to have a chance at receiving the signal

    // Check if a valid response
    bool got_ir = monitor_ir();

    // Disable IR receiver
    disableIRIn();  // Disable IR receiver state machine
    PRR |= _BV(PRTIM0); // Turn off timer0 clock
    bit_clear(PORTB, PIN_IR_ENABLE); // Power down IR receiver

    // IR signal received?
    if(got_ir) {
        // Play LED pattern
        flash_lights(10);

        // disable LEDs
        PORTB |= (1<<PIN_LED1) | (1<<PIN_LED2) | (1<<PIN_LED3);
    }
    else {
        // Sleep using a watchdog timeout
        sleep_ms(SLEEP_TIME);
    }
}


int main(void) {
    setup();
    
    for(;;){
        loop();
    }
    
    return 0;
}
