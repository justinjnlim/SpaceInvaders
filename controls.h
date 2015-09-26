// controls.h
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

#ifndef __CONTROLS_INCLUDED__
#define __CONTROLS_INCLUDED__

// Primary Fire button (PE0)
#define Fire (*((volatile unsigned long *)0x40024004))
// Special Attack button (PE2)
#define specialFire (*((volatile unsigned long*)0x40024008))

// Initialize port E
void controls_Init(void);

// Read Potentiometer
// Returns: 12-bit ADC input (0 - 4095)
unsigned long Joystick(void);

#endif
