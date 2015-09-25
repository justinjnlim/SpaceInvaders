// sfxDriver.h
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

#ifndef __SOUNDDRIVER_INCLUDED__
#define __SOUNDDRIVER_INCLUDED__

// Global variables used
extern const unsigned char* currentSound;
extern unsigned long soundIndex;
extern unsigned long soundCount;

// Stop timer2
void Timer2A_Stop(void);

// Start timer2
void Timer2A_Start(void);

// DAC out
void DAC_Out(unsigned int wave);
	
// Initialise DAC (PB3-PB0)
void DAC_Init(void);

// Initialise timer2 (11.025kHz)
void Timer2_Init(void);

// Play UFO sound
void Sound_UFO(void);

// Play Shooting sound
void Sound_Shoot(void);

// Hit sound
void Sound_Hit(void);

// Game over sound
void Sound_GameOver(void);

// Play 4-beat tempo sound (make var change from 0 to 3 to 0)
void Sound_Tempo(unsigned char var);

#endif
