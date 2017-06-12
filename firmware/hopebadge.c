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
#include <avr/wdt.h>

#include "hopebadge.h"

#include "irreceiver.h"
#include "sleep.h"

static inline void set_cpu_frequency_2mhz() {
    // Change the clock to 1MHz
    __asm__ volatile (
                      "st Z,%1" "\n\t"
                      "st Z,%2"
                      : :
                      "z" (&CLKPR),
                      "r" ((uint8_t) (1<<CLKPCE)),
                      "r" ((uint8_t) 1)  // new CLKPR value 0=8MHz, 1=4MHz, 2=2MHz, 3=1MHz)
                      );
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
    ir_start();

    // Wait for IR reciver
    delay_ms(IR_SEARCH_TIME);

    // If we are actively receiving a signal, wait a little longer
    if(ir_active()) {
      delay_ms(IR_RECEIVE_TIME);
    }

    // Disable IR receiver
    ir_stop();

    // If we saw some IR traffic
    if(ir_received()) {
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
