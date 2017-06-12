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

#include "hopebadge.h"

#include "irremote.h"
#include "crc.h"
#include "sleep.h"

uint8_t speed;
uint8_t repeats;

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



static inline void set_cpu_frequency_2mhz() {
    // Change the clock to 4MHz
    __asm__ volatile (
                      "st Z,%1" "\n\t"
                      "st Z,%2"
                      : :
                      "z" (&CLKPR),
                      "r" ((uint8_t) (1<<CLKPCE)),
                      "r" ((uint8_t) 2)  // new CLKPR value 0=8MHz, 1=4MHz, 2=2MHz, 3=1MHz)
                      );
}


static inline void enable_ir() {
    bit_set(PORTB, PIN_IR_ENABLE);  // Power up IR receiver
    PRR &= ~_BV(PRTIM0); // Turn on timer0 clock
    enableIRIn(); // Start the IR receiver state machine
}


// Listen for an IR command
// Duration: x seconds
static inline bool check_ir() {
    // If we didn't get an NEC command, bail
    if (!decodeIR()) {
        return false;
    }

    repeats =       (results.value >> 24) & 0xFF;
    speed =         (results.value >> 16) & 0xFF;
    uint8_t extra = (results.value >>  8) & 0xFF;

    resetCRC();
    updateCRC(speed);
    updateCRC(repeats);
    updateCRC(extra);

    // If the CRC is valid, store the results.
    if(getCRC() != (results.value & 0xFF)) {
        return false;
    }

    return true;
}

static inline bool check_any_ir() {
    return (results.transitions > 8);
}

static inline void disable_ir() {
    disableIRIn();  // Disable IR receiver state machine
    PRR |= _BV(PRTIM0); // Turn off timer0 clock
    bit_clear(PORTB, PIN_IR_ENABLE); // Power down IR receiver
}

static void flash_lights(uint8_t count, uint8_t delay) {
    for(uint8_t i = 0; i < count; i++) {
        bit_clear(PORTB, PIN_LED1);
        sleep_ms(delay);
        bit_set(PORTB, PIN_LED1);

        bit_clear(PORTB, PIN_LED2);
        sleep_ms(delay);
        bit_set(PORTB, PIN_LED2);

        bit_clear(PORTB, PIN_LED3);
        sleep_ms(delay);
        bit_set(PORTB, PIN_LED3);
   }
}


static inline void setup() {
    // Configure the processor for 2MHz operation (save power)
    set_cpu_frequency_2mhz();

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
    bit_set(DIDR0, AIN1D);       // Disable digital input buffers
    bit_set(DIDR0, AIN0D);       //  on AIN1/0 pins

    // Note: Watchdog disabled by fuse setting
    
    // Disable clicks to peripherals that we aren't using
    // (This saves power in run mode)
    PRR |=
        _BV(PRTIM0)
        | _BV(PRTIM1)
        | _BV(PRUSI)
        | _BV(ADC);
    
    // Configure the sleep mode and wakeup interrupt
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Use power down mode for sleep

    // If the device was reset from the user button, flash the LEDs once before continuing
    if(MCUSR & _BV(EXTRF)) {
        bit_clear(MCUSR, EXTRF);
        flash_lights(FLASH_REPEAT_COUNT, FLASH_DELAY);
    }

}

static inline void loop() {
    // Enable IR receiver
    enable_ir();

    // Wait for IR reciver
    delay_ms(IR_SEARCH_TIME);

    // If we are actively receiving a signal, wait a little longer
    if(irActive()) {
      delay_ms(IR_RECEIVE_TIME);
    }


    // Disable IR receiver
    disable_ir();

    // If a valid NEC code was detected, flash the lights using the received
    // parameters
    if(check_ir()) {
        //flash_lights(repeats, speed);
    }
    // Otherwise, use a generic pattern
    else if(check_any_ir()) {
        flash_lights(FLASH_REPEAT_COUNT, FLASH_DELAY);
    }
    else {
        // Sleep using a watchdog timeout
        sleep_ms(SLEEP_TIME);
    }
}

//#define TEST_ON
//#define TEST_SLEEP
//#define TEST_LED_ON_NOSLEEP
//#define TEST_LED_ON_SLEEP
//#define TEST_FLASH_LIGHTS
//#define TEST_ENABLE_IR
//#define TEST_ENABLE_DISABLE_IR

int main(void) {
    setup();

    #ifdef TEST_ON
    while(true)
        {}
    #endif

    #ifdef TEST_SLEEP
    while(true) {
        sleep_s(5);
        bit_clear(PORTB, PIN_LED1);
        sleep_s(1);
        bit_set(PORTB, PIN_LED1);
    }
    #endif

    #ifdef TEST_LED_ON_NOSLEEP
    bit_clear(PORTB, PIN_LED1);
    while(true)
        {}
    #endif

    #ifdef TEST_LED_ON_SLEEP
    bit_clear(PORTB, PIN_LED1);
    while(true)
        sleep_s(5);
    #endif

    #ifdef TEST_FLASH_LIGHTS
    while(true)
        flash_lights(1,255);    
    #endif

    #ifdef TEST_ENABLE_IR
    enable_ir();
    while(true)
        {}
    #endif

    #ifdef TEST_ENABLE_DISABLE_IR
    enable_ir();
    disable_ir();
    while(true)
        {}
    #endif
    

    for(;;){
        loop();
    }
    
    return 0;
}
