#ifndef IRRECEIVER_H
#define IRRECEIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "hopebadge.h"

extern uint8_t irCount;

static inline void ir_start() {
    irCount = 0;

    bit_set(PORTB, PIN_IR_ENABLE);  // Power up IR receiver

    GIMSK |= (1 << PCIE);   // pin change interrupt enable
    PCMSK = (1 << PCINT4); // pin change interrupt enabled for PCINT4
}


static inline void ir_stop() {
    GIMSK &= ~(1 << PCIE);   // pin change interrupt disable

    bit_clear(PORTB, PIN_IR_ENABLE); // Power down IR receiver
}

static inline bool ir_active() {
    return (irCount > 4);
}

static inline bool ir_received() {
    return (irCount > 8);
}

#endif // IRRECEIVER_H
