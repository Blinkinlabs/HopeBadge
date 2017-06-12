#include <avr/interrupt.h>

#include "irreceiver.h"

uint8_t irCount;

ISR(PCINT0_vect)
{
    irCount++;
}
