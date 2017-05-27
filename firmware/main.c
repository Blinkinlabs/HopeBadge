/* Name: main.c
 * Heartbeat ball
 *
 * Copyright 2015 Blinkinlabs, LLC
 * Author: Matt Mets
 *
 * Based loosely on the ATTiny example by Markus Konrad
 */

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "irremote.h"
#include "hopebadge.h"
#include "crc.h"

#if 0

// IR Receiver 
extern decode_results_t results;



void sleep()
{
    interruptCount = 0;

    bitSet(GIMSK, INT0);    // Enable INT0

    sleep_enable(); // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei(); // Enable interrupts
    sleep_cpu(); // SLEEP
 
    sleep_disable(); // Clear SE bit

    // Wait a small amount of time to accrue debounce information   
    _delay_ms(1);

    cli(); // Disable interrupts
    bitClear(GIMSK, INT0);      // Disable INT0
    sei(); // Enable interrupts
}

ISR(INT0_vect) {
    interruptCount++;
}


// Listen for an IR command
// Duration: 4+ seconds
void monitorIR() {
    setLEDs(0); // Make sure that the LEDs are off to prevent interference
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
            // Update the rates both in working memory and in the EEPROM
            heartbeatSpeed = speed;
            repeatCount = repeats;
            debounceCount = sensitivity;
            validateRates();
            saveRates();

            // Flash the LEDs to indicate IR reception
            for(uint8_t i = 0; i < 5; i++) {
                setLEDs(i%2==0?20:0);
                _delay_ms(2);
            }
            setLEDs(0);

            // And reset our timeout counter
            irLoop = 0; 
        }
        
        resumeIR(); // Receive the next value
        
    }
}

#endif

// program entry point
// Kinda sad that the production devices will only run this once T_T
// Unless they brown out and somehow restart O_O
int main(void) {
    cli();
    MCUSR &= ~_BV(WDRF); // Clear the watchdog reset flag
    wdt_enable(WDTO_2S);
    wdt_reset();
    sei();

    // Change the clock to 4MHz
    __asm__ volatile (
        "st Z,%1" "\n\t"
        "st Z,%2"
        : :
        "z" (&CLKPR),
        "r" ((uint8_t) (1<<CLKPCE)),
        "r" ((uint8_t) 1)  // new CLKPR value 0=8MHz, 1=4MHz, 2=2MHz, 3=1MHz)
    );


    bitSet(ACSR, ACD);          // Disable the analog comparitor
    bitClear(ADCSRA, ADEN);     // Disable the ADC
    // Note: Brown out detection disabled by fuse setting
    // Note: Watchdog disabled by fuse setting

    // Disable clicks to peripherals that we aren't using
    // (This saves power in run mode)
    PRR |= _BV(PRUSI) | _BV(ADC);

    // Configure the sleep mode and wakeup interrupt
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));    // INT0 on low level
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Use power down mode for sleep

    // Set the LEDs to outputs and disable
    bitSet(DDRB,    PIN_LED1);
    bitSet(PORTB,   PIN_LED1);
    bitSet(DDRB,    PIN_LED2);
    bitSet(PORTB,   PIN_LED2);
    bitSet(DDRB,    PIN_LED3);
    bitSet(PORTB,   PIN_LED3);

    // Main loop.
    for(;;){
        // Sleep using a timer countdown

        // Enable IR Receiver

        // Wait for IR reciver

        // Disable IR receiver

        // IR signal received?

        // no: continue

        // yes:

        // Play LED pattern

        // disable LEDs


        bitSet(PORTB, PIN_LED3);
        bitClear(PORTB, PIN_LED1);
        _delay_ms(500);
        bitSet(PORTB, PIN_LED1);
        bitClear(PORTB, PIN_LED2);
        _delay_ms(500);
        bitSet(PORTB, PIN_LED2);
        bitClear(PORTB, PIN_LED3);
        _delay_ms(500);

    }
    
    return 0;   /* never reached */
}
