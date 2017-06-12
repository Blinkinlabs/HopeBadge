/*
 * IRremote
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
 * Edited by Mitra to add new controller SANYO
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
* LG added by Darryl Smith (based on the JVC protocol)
 */

#ifndef IRremote_h
#define IRremote_h

#include <stdint.h>
#include <stdbool.h>

// The following are compile-time library options.
// If you change them, recompile the library.
// If DEBUG is defined, a lot of debugging output will be printed during decoding.
// TEST must be defined for the IRtest unittests to work.  It will make some
// methods virtual, which will be slightly slower, which is why it is optional.
// #define DEBUG
// #define TEST

// Results returned from the decoder
typedef struct {
//  union { // This is used for decoding Panasonic and Sharp data
//    unsigned int panasonicAddress;
//    unsigned int sharpAddress;
//  };
  unsigned long value; // Decoded value
  uint8_t bits; // Number of bits in decoded value
  volatile uint8_t *rawbuf; // Raw intervals in .5 us ticks
  uint16_t rawlen; // Number of records in rawbuf.
} decode_results_t;

// Values for decode_type
#define NEC 1
#define SONY 2
#define RC5 3
#define RC6 4
#define DISH 5
#define SHARP 6
#define PANASONIC 7
#define JVC 8
#define SANYO 9
#define MITSUBISHI 10
#define SAMSUNG 11
#define LG 12
#define UNKNOWN -1

// Decoded value for NEC when a repeat code is received
#define REPEAT 0xffffffff

int decodeIR();
void enableIRIn();
void disableIRIn();
void resumeIR();
bool isReceivingIR();

// Some useful constants

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 70 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 20


// IR Receiver 
extern volatile decode_results_t results;

#endif
