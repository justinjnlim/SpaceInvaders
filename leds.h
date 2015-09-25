// Leds.h
// Runs on LM4F120/TM4C123
// Fernando Angel Liozzi
// A Space Invaders game for UTAustinX: UT.6.02x Embedded Systems - Shape the World
// May 05, 2015

#ifndef	__LEDS_INCLUDED__
#define __LEDS_INCLUDED__
 
// Led 1
#define Led1 (*((volatile unsigned long *)0x40005040))
// Led 2
#define Led2 (*((volatile unsigned long *)0x40005080))

// Led 1
void Led_1(unsigned char state);

// Led 2
void Led_2(unsigned char state);

#endif
