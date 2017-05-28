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
    bool received_signal = false;

    enableIRIn(); // Start the receiver
    
    for(uint32_t irLoop = 0; irLoop < IR_MONITOR_TIME; irLoop++) {
    
        _delay_ms(60);  // Derate this to handle processing time of decodeIR() @4MHz

        wdt_reset(); // Hit the watchdog

        // If we didn't get an NEC command, bail
        if (!decodeIR()) {
            continue;
        }

        uint8_t speed =         (results.value >> 24) & 0xFF;
        uint8_t repeats =       (results.value >> 16) & 0xFF;
        uint8_t sensitivity =   (results.value >>  8) & 0xFF;

        resetCRC();
        updateCRC(speed);
        updateCRC(repeats);
        updateCRC(sensitivity);

        // If the CRC is valid, store the results.
        if(getCRC() == (results.value & 0xFF)) {
            received_signal = true;
            break;
        }
        
        resumeIR(); // Receive the next value
    }

    disableIRIn();

    return received_signal;
}


static inline void flash_lights(uint8_t count) {
    for(uint8_t i = 0; i < count; i++) {
        bit_clear(PORTB, PIN_LED1);
        _delay_ms(30);
        bit_set(PORTB, PIN_LED1);

        bit_clear(PORTB, PIN_LED2);
        _delay_ms(30);
        bit_set(PORTB, PIN_LED2);

        bit_clear(PORTB, PIN_LED3);
        _delay_ms(30);
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
    PRR |= _BV(PRTIM1) | _BV(PRTIM0) | _BV(PRUSI) | _BV(ADC);
    
    // Configure the sleep mode and wakeup interrupt
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Use power down mode for sleep
}

static inline void loop() {
    // Sleep using a timer countdown
    sleep_ms(SLEEP_TIME);

    // Enable IR receiver
    bit_set(PORTB, PIN_IR_ENABLE);
    
    // Wait for the IR receiver to stabilize
    // TODO: am needed?
    _delay_ms(50);

    // Wait for IR reciver
    bool got_ir = monitor_ir();

    // Disable IR receiver
    bit_clear(PORTB, PIN_IR_ENABLE);

    // IR signal received?
    if(!got_ir) {
        // no: continue
        flash_lights(1);
        return;
    }

    // yes:

    // Play LED pattern
    flash_lights(10);

    // disable LEDs
    PORTB |= (1<<PIN_LED1) | (1<<PIN_LED2) | (1<<PIN_LED3);
}


int main(void) {
    setup();
    
    for(;;){
        loop();
    }
    
    return 0;
}
