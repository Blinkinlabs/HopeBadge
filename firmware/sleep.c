#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "sleep.h"

// Watchdog ISR for sleep routine
ISR(WDT_vect) {
    // Interrupt flag is automatically cleared

    disable_watchdog();
}
