#include <avr/interrupt.h>

// Watchdog ISR for sleep routine
ISR(WDT_vect) {
    // Interrupt flag is automatically cleared

}
