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

// IR Receiver 
extern decode_results_t results;

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



void set_cpu_frequency_4mhz() {
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

void watchdog_setup() {
    cli();
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR |= (_BV(WDP2) | _BV(WDP1));
    sei();
}

// Watchdog ISR for sleep routine
ISR(WDT_vect) {
    // Interrupt flag is automatically cleared

}


// Sleep for the given number of counts with the specified prescaler bits
void sleep(uint8_t prescaler, int16_t counts) {
    int16_t sleep_counter = counts;
    
    sleep_enable(); // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    
    while(sleep_counter > 0) {
        cli();
        
        WDTCR = _BV(WDCE) | _BV(WDE);  // Program the watchdog timer as an interrupt with the given prescaler
        WDTCR |= _BV(WDIE) | prescaler;
        
        sei();
        
//        sleep_bod_disable();    // Disable brown-out detection during sleep
        sleep_cpu();            // Go to sleep: We will jump to the WDT_vect on wakeup
        
        sleep_counter--;        // When we get back, decrement the sleep counter.
    }
    
    // Reset the original watchdog settings
    watchdog_setup();
    
    sleep_disable(); // Clear SE bit
}

// Sleep for a short period of time
// @param milliseconds  Number of milliseconds to sleep, with resolution 16ms
inline void sleep_ms(int16_t milliseconds)
{
    sleep(0, milliseconds/16);
}


// Sleep for some time
//
inline void sleep_s(int16_t seconds)
{
    sleep(_BV(WDP2) | _BV(WDP1), seconds);
}


// Listen for an IR command
// Duration: x seconds
bool monitor_ir() {
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


void flash_lights(uint8_t count) {
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


void setup() {
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

void loop() {
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
