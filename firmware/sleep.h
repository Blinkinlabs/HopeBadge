
static inline void disable_watchdog() {
    cli();
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR = 0;
    sei();
}


// Sleep for the given number of counts with the specified prescaler bits
static inline void sleep(uint8_t prescaler, int16_t counts) {
    int16_t sleep_counter = counts;
    
    sleep_enable(); // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    
    while(sleep_counter > 0) {
        cli();
        WDTCR = _BV(WDCE) | _BV(WDE);  // Program the watchdog timer as an interrupt with the given prescaler
        WDTCR |= _BV(WDIE) | prescaler;
        sei();
       
        sleep_cpu();            // Go to sleep: We will jump to the WDT_vect on wakeup
        
        sleep_counter--;        // When we get back, decrement the sleep counter.
    }
    
    sleep_disable(); // Clear SE bit
}

// Sleep for a short period of time
// @param milliseconds  Number of milliseconds to sleep, with resolution 16ms
static inline void sleep_ms(int16_t milliseconds)
{
    sleep(0, milliseconds/16);
}


// Sleep for some time
//
static inline void sleep_s(int16_t seconds)
{
    sleep(_BV(WDP2) | _BV(WDP1), seconds);
}



// Delay for the given number of counts with the specified prescaler bits
static inline void delay(uint8_t prescaler, int16_t counts) {
    int16_t delay_counter = counts;
    
    while(delay_counter > 0) {
        cli();
        WDTCR = _BV(WDCE) | _BV(WDE);  // Program the watchdog timer as an interrupt with the given prescaler
        WDTCR |= _BV(WDIE) | prescaler;
        sei();
     
        // Wait until the watchdog interrupt disables the watchdog
        while(WDTCR & _BV(WDIE));
        
        delay_counter--;        // When we get back, decrement the sleep counter.
    }
}

// Delay for a short period of time
// @param milliseconds  Number of milliseconds to sleep, with resolution 16ms
static inline void delay_ms(int16_t milliseconds)
{
    delay(0, milliseconds/16);
}


// Delay for some time
//
static inline void delay_s(int16_t seconds)
{
    delay(_BV(WDP2) | _BV(WDP1), seconds);
}
