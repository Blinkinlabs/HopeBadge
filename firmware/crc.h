#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

extern uint8_t crc;

// Reset the CRC engine
inline void resetCRC() {
  crc = 0;
}

// Update the CRC engine
inline uint8_t getCRC() {
  return crc;
}

// Get the current value of the CRC engine
inline void updateCRC(uint8_t data) {
  uint8_t i;

  crc = crc ^ data;
  for (i = 0; i < 8; i++)
  {
    if(crc & 0x01) {
      crc = (crc >> 1) ^ 0x8C;
    }
    else {
      crc >>= 1;
    }
  }
}


#endif
