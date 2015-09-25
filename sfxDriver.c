// sfxDriver.c
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

#include "sfxDriver.h"

#include "tm4c123gh6pm.h"
#include "SoundFiles.c"

const unsigned char* wave = 0;
unsigned long soundIndex = 0;
unsigned long soundCount = 0;

void DAC_Init(void){
	// Initialise DAC (PB3-PB0) and leds (PB5-PB4)
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x02; 					// activate port B
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTB_DIR_R |= 0X3F;     		// make PB0-PB5 out
	GPIO_PORTB_AFSEL_R &= ~0X3F;  		// disable alt funct on PB0-PB5
	GPIO_PORTB_DEN_R |= 0X3F;     		// enable digital I/O on PB0-PB5
	GPIO_PORTB_AMSEL_R &= ~0x30;      // no analog on PB5-4
  GPIO_PORTB_PCTL_R = 0x00; 				// regular function
	GPIO_PORTB_DR8R_R |= 0x3F;				// drive up to 8mA out
}

// Initialise timer2 (11.025kHz)
void Timer2_Init(void){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 7256-1;    	// 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;  	// acknowledge
  if(soundCount){
    DAC_Out(wave[soundIndex]>>4); 
    soundIndex += 1;
    soundCount -= 1;
  }else{
    Timer2A_Stop();							// disable Timer2
  }
}

void Timer2A_Stop(void){ 
  TIMER2_CTL_R &= ~0x00000001; 	// disable Timer2
}

void Timer2A_Start(void){ 
  TIMER2_CTL_R |= 0x00000001;   // enable
}

void DAC_Out(unsigned int wave){
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & 0xF0) | (0x0F & wave); // 4 bits DAC-out.
}

void PlaySound(const unsigned char *soundPointer, unsigned long soundLength){
// Enables timer2 & set timer2 to output sound
  wave = soundPointer;
  soundIndex = 0;
  soundCount = soundLength;
	Timer2A_Start(); // enable Timer2
}

void Sound_UFO(void){
// UFO sound
  PlaySound(UFO_Sound,1802);
}

void Sound_Shoot(void){
// Shooting sound
  PlaySound(shoot,4080);
}

void Sound_Hit(void){
// Enemy explosion
  PlaySound(hit,3377);
}

void Sound_GameOver(void){
// Player explosion
  PlaySound(gameOver,2000);
}

void Sound_Tempo(unsigned char var){
// Plays 4-beat tempo sound
	if(var == 0)
		PlaySound(tempo0,982);
	else if(var == 1)
		PlaySound(tempo1,1042);
	else if(var == 2)
		PlaySound(tempo2,1054);
	else
		PlaySound(tempo3,1098);
}
