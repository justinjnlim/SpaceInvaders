// leds.h
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

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
