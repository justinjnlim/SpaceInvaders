// leds.c
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

#include "Leds.h"
#include "tm4c123gh6pm.h"

// Led 1
void Led_1(unsigned char state){
	if (state==1)
		Led1=0x10;
	else
		Led1=0x00;
}

// Led 2
void Led_2(unsigned char state){
	if (state==1)
		Led2=0x20;
	else
		Led2=0x00;
}
