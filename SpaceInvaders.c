// SpaceInvaders.c
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

#include <stdlib.h>

#include "controls.h"
#include "sprites.h"
#include "sfxDriver.h"
#include "leds.h"

#include "ScreenImages.c"

/*** DEFINITIONS ***/
#define BONUSH 9 		// height of bonus ship's row
#define PLAYERH 8 		// height of player ship's row
#define BUNKERH 5 		// height of bunker's row

#define MINSCREENY 0 	// Minimum screen y-coord (top)
#define MAXSCREENY 47 // Maximum screen y-coord (bottom)
#define EFFSCREENH (MAXSCREENY - MINSCREENY) // effective screen height
#define MINSCREENX 0 	// Minimum screen x-coord (left)
#define MAXSCREENX 83 // Maximum screen x-coord (right)
#define EFFSCREENW (MAXSCREENX - MINSCREENX) // effective screen width
#define ENEMYGAPX 0 	// horizontal gap (in px) between adjacent enemy sprites
#define ENEMYGAPY 1 	// vertical gap (in px) between adjacent enemy sprites
#define ENEMYROWS 2 	// number of rows in enemy horde
#define ENEMYCOLS 4 	// number of columns in enemy horde
#define ENEMYNUM (ENEMYROWS * ENEMYCOLS) // number of enemies in horde

#define BUNKERNUM 3 	// max number of bunkers
#define PLAYERLIVES	5	// players lives

/*** Global variables ***/
unsigned long score;
unsigned int level;
unsigned int timerBonus;

unsigned long enemyShoot=20; // Each enemy's chance of shotting ratio= enemyShoot/ENEMYSHOOTDIV
float enemyFreq=3;
#define ENEMYSHOOTDIV 5000   // Change this value to modify difficulty

sprite playerSpr, livesSprs[PLAYERLIVES], BonusSpr, bunkerSprs[BUNKERNUM], enemySprs[ENEMYNUM]; // sprites
sprite playerBullet, enemyBullets[ENEMYCOLS]; // bullet sprites

unsigned char screenRefreshFlag;	// used for synchronization
unsigned char enemyMove;					// 0 => enemies move right, 1 => move left
unsigned char enemyDance; 				// to alternate enemy images per step
#define TIMERMAX 12 		// max of timer (will be least common multipler of enemies' number of images)

unsigned char levelCleared; 			// bool to indicate if level cleared
sprite* toExplode; 								// marks dead sprites to explode

/*** Helper Function Definitions ***/
void DisableInterrupts(void); 		// Disable interrupts
void EnableInterrupts(void);  		// Enable interrupts

void TitleScreen(void); 					// title screen
void GameOver(void); 							// game over screen
void SysTick_Init(void); 					// screen refresh (FPS)
void Timer2_Init(void); 					// laser movement
void PrintSpr(sprite* spr, unsigned char var); 		// Print specified variant of sprite's image
void ExplodeSpr(sprite* spr, unsigned char var); 	// Print specified variant of sprite's explosion
void PrintBunker(sprite* bunker); 	// print appropriate variant of bunker
void GameInit(void); 								// init player, bunker and lives icons
void EnemyHordeInit(void); 					// init a enemy horde
void SprCrash(sprite* bullet, sprite* spr, unsigned char explodeFlag); 	// Checks collision & update
void Delay100ms(unsigned long count); // delay for count*100 ms
void Timer1_Init(float freq);					// init Timer1A
void PrintLives (sprite* lives);			// prints lives icons

/*** Level Increment shoot ratio ***/
void EnemyShootRatio(void){
// Enemy shoot ratio incease on every level
	if(level < 9)
		enemyShoot += 20;
	else
		enemyShoot += 100 / level;	// asymptotic growth to diminish the difficulty
}

/*** Main Functions ***/
int main(void){
	unsigned char i;
	
	TExaS_Init(NoLCD_NoScope);  // set system clock to 80 MHz
	Nokia5110_Init();	// initialize everything
	controls_Init();	
	DAC_Init(); 
	Timer2_Init();
	Timer1_Init(enemyFreq);
	EnableInterrupts();
  srand(Joystick());	// intializes random number generator seed using joystick ADC noise value	
	while(1){
		TitleScreen();
		Timer2A_Start();
		GameInit();
		screenRefreshFlag = 1;
		levelCleared = 1;
		Sound_Shoot();
		timerBonus=0;
		
		while(playerSpr.hp > 0){	//enter game
			while(screenRefreshFlag == 0){}		// for synchronization
			
			if(levelCleared){
				level += 1;
				EnemyHordeInit();
				EnemyShootRatio();
				SysTick_Init();
				levelCleared = 0;
				enemyFreq=3+level/10.0;
				Timer1_Init(enemyFreq);
				timerBonus=0;
			}
			
			// UPDATE SCREEN
			Nokia5110_ClearBuffer();
			
			if(playerSpr.hp > 0) 					// player ship
				PrintSpr(&playerSpr,0);
			
			if(playerBullet.hp > 0) 				// player bullets
				PrintSpr(&playerBullet,0);
			
			for(i = 0; i < BUNKERNUM; i++){ 		// bunkers
				PrintBunker(&bunkerSprs[i]);
			}
			
			for(i = 0; i < playerSpr.hp; i++){ 		// players Lives
				PrintLives(&livesSprs[i]);
			}
			
			for(i = 0; i < ENEMYNUM; i++){ 			// enemies
				if(enemySprs[i].hp > 0)
					PrintSpr(&enemySprs[i], enemyDance % 2);
			}
			
			for(i = 0; i < ENEMYCOLS; i++){ 		// enemy bullets
				if(enemyBullets[i].hp > 0)
					PrintSpr(&enemyBullets[i], 0);
			}
			
			if(BonusSpr.hp > 0) 								// bonus ship
				PrintSpr(&BonusSpr, enemyDance % 3);
			
			if(toExplode != 0){ 								// explosions
				ExplodeSpr(toExplode,0);
				ExplodeSpr(toExplode,1);
				toExplode = 0;
			}
			
			Nokia5110_DisplayBuffer();
			screenRefreshFlag = 0;
		}

		GameOver();
		
	}
}
//**************************************************************************
void SysTick_Handler(void){
// First periodic interrupt handler continuously updates the game on player movement,
// player firing, projectile movement, collision events, the bonus ship, and level completion.
// It runs at 30 Hz, so it triggers every 1/30th of a second.
  unsigned char i, j;
	
	// Player movement
	playerSpr.x = MINSCREENX + Joystick() * (EFFSCREENW - playerSpr.width - 1) / 4095;
	// Player fire
	Led_1(0);
	if((Fire && playerBullet.hp == 0) && playerSpr.hp > 0){
		playerBullet = ShootLaser(&playerSpr, 1);
		Sound_Shoot();
		Led_1(1);
	}
	
	// Player bullet
	if(playerBullet.hp != 0){
		if(playerBullet.y <= MINSCREENY + playerBullet.height)
			playerBullet.hp = 0;
		else 
			playerBullet.y -= 1; 
	}
	
	// Enemy bullet
	for(i = 0; i < ENEMYNUM; i++){ 
		if(enemyBullets[i].hp > 0){
			if(enemyBullets[i].y >= MAXSCREENY)
				enemyBullets[i].hp = 0;
			else 
				enemyBullets[i].y += 1;
		}
	}
	
	// Collision check
	for(i = 0; i < BUNKERNUM; i++)
		SprCrash(&playerBullet, &bunkerSprs[i], 0); 		// Player hit bunker
	for(i = 0; i < ENEMYNUM; i++){
		SprCrash(&playerBullet, &enemySprs[i], 1); 			// Player hit enemy
		SprCrash(&playerBullet, &enemyBullets[i], 0); 	// Player bullet hit enemy bullet
		SprCrash(&enemySprs[i], &playerSpr, 1);					// Enemy crashes player
		SprCrash(&enemyBullets[i], &playerSpr, 1); 			// Enemy hit player
		for(j = 0; j < BUNKERNUM; j++)
			SprCrash(&enemyBullets[i], &bunkerSprs[j], 0);// Enemy hit bunker
	}
	SprCrash(&playerBullet, &BonusSpr, 1); 						// Player bullet hit UFO
	
	// Level cleared?
	levelCleared = 1;
	for(i = 0; i < ENEMYNUM; i++){
		if(enemySprs[i].hp > 0)
			levelCleared = 0;
	}
	
	// Bonus ship create every 4 seconds
	timerBonus++;
	if (timerBonus>120){
		// Create UFO on top-left
		if(BonusSpr.hp == 0){
			BonusSpr = SmallEnemyBonus_Init(0,BONUSH);
			Sound_UFO();
		}
		timerBonus=0;
	}
	
	screenRefreshFlag = 1;
}

void Timer1A_Handler(void){
// Second periodic interrupt handler manages all the movement of
// the enemy ships, their firing habits, etc.
	unsigned char i, j;
	TIMER1_ICR_R = 0x00000001;
		
	// Enemy move
	for(i = 0; i < ENEMYNUM; i++){
		if(enemySprs[i].hp > 0){ 
			enemySprs[i].x += enemyMove == 0 ? 1 : -1;
			PrintSpr(&enemySprs[i], enemyDance%2);
			Sound_Tempo(enemyDance%4);
		}
	}
	// Enemy fire
	Led_2(0);
	for(i = 0; i < ENEMYCOLS; i++){
		if((enemyBullets[i].hp == 0) && 							// 1 bullet per row
			(((rand()) % ENEMYSHOOTDIV) < enemyShoot)){ // random probability of firing
			for(j = 0; j < ENEMYROWS; j++){ 						// find front sprite & fire laser
				if(enemySprs[j * ENEMYROWS + i].hp != 0){
					enemyBullets[i] = ShootLaser(&enemySprs[j * ENEMYROWS + i], 0);
					Sound_Shoot();
					Led_2(1);
					break;
				}
			}
		}
	}
		
	// Enemy direction
	for(i = 0; i < ENEMYNUM; i++){		
		if(enemySprs[i].hp > 0){
			if(enemySprs[i].x >= SCREENW - enemySprs[i].width){ 	// enemy collides on right
				enemyMove = 1;
				for(j = 0; j < ENEMYNUM; j++) enemySprs[j].y += 1; 	// enemy advance a step
				enemyFreq +=0.2;																		// enemy frequency up
				TIMER1_TAILR_R = 80000000 / enemyFreq -1;						// enemy approach a step
			}
			else if(enemySprs[i].x <= 0){ 												// enemy collides on left
				enemyMove = 0;
				for(j = 0; j < ENEMYNUM; j++) enemySprs[j].y += 1; 	// enemy advance a step
				enemyFreq +=0.2;																		// enemy frequency up
				TIMER1_TAILR_R = 80000000 / enemyFreq -1;						// enemy approach a step
			}
		}
	}
	
	// Bonus ship
	if(BonusSpr.x > SCREENW - BonusSpr.width){ 	// Out of screen
		BonusSpr.hp = 0;
	}
	if(BonusSpr.hp > 0){ 												// Move
		BonusSpr.x += 2;
		PrintSpr(&BonusSpr, enemyDance%3);
	}
	
	enemyDance = (enemyDance + 1) % TIMERMAX;
}
//**************************************************************************

/*** Helper Function Implementation ***/
void TitleScreen(void){
// Displays the Title screen, creator name,
// and will not proceed until the player presses "Fire" (PE0).
	TIMER1_CTL_R &= ~0x00000001; // disable Timer1A
	NVIC_ST_CTRL_R = 0;
	DAC_Out(0);
	
	Nokia5110_Clear();
	Nokia5110_SetCursor(3,0);
	Nokia5110_OutString("SPACE");
	Nokia5110_SetCursor(2,1);
	Nokia5110_OutString("INVADERS");
	Nokia5110_SetCursor(0,5);
	Nokia5110_OutString("by: Justin L");
	Delay100ms(5);
	Sound_GameOver();	
	while(!Fire){
		Led_1(1);
		Led_2(0);
		Nokia5110_SetCursor(1,3);
		Nokia5110_OutString("Hold Fire");
		Delay100ms(2);
		Led_1(0);
		Led_2(1);
		Nokia5110_SetCursor(1,3);
		Nokia5110_OutString("          ");
		Delay100ms(2);
	};
	Led_1(0);
	Led_2(0);
	Delay100ms(5);
	//*****************************************
}

void GameOver(void){
//  Displays the Game Over screen.
//	Will show the player their Score and Level,
// 	press "specialFire" (PE2) to continue.
	TIMER1_CTL_R &= ~0x00000001; // disable Timer1A
	NVIC_ST_CTRL_R = 0;
	DAC_Out(0);
	Nokia5110_Clear();
	Nokia5110_SetCursor(2,0);
	Nokia5110_OutString("You have");
	Nokia5110_SetCursor(2,1);
	Nokia5110_OutString("been");
	Nokia5110_SetCursor(1,2);
	Nokia5110_OutString("DESTROYED!");
	Nokia5110_SetCursor(0,4);
	Nokia5110_OutString("score:");
	Nokia5110_OutUDec(score);
	Nokia5110_SetCursor(0,5);
	Nokia5110_OutString("level:");
	Nokia5110_OutUDec(level);
	Delay100ms(5);
	Sound_GameOver();	
	while(!specialFire){
		Led_1(1);
		Led_2(1);
		Delay100ms(1);
		Led_1(0);
		Led_2(0);
		Delay100ms(1);
	};
	Led_1(0);
	Led_2(0);
	Delay100ms(5);
}

void SysTick_Init(void){
// Initalize SysTick at 30 Hz (assuming 80Mhz PLL)
	NVIC_ST_CTRL_R = 0; 
	NVIC_ST_RELOAD_R =  2666667 - 1;
  NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R |= 0x01 << 29; // priority 1
	NVIC_ST_CTRL_R = 0x07;
}

void PrintSpr(sprite* spr, unsigned char var){
// Prints specified variant of sprite's image
	Nokia5110_PrintBMP(spr->x, spr->y, spr->image[var],0);
}

void ExplodeSpr(sprite* spr, unsigned char var){
// Prints specified variant of sprite's explosion
	Nokia5110_PrintBMP(spr->x, spr->y, spr->explode[var],0);
}

void PrintBunker(sprite* bunker){
// Prints appropriate variant of bunker
	if(bunker->hp > 20)
		PrintSpr(bunker, 0);
	else if(bunker->hp > 10)
		PrintSpr(bunker, 1);
	else if(bunker->hp > 0)
		PrintSpr(bunker, 2);
}

void PrintLives (sprite* lives){
	// Prints lives marks
	if(lives->hp > 0)
		PrintSpr(lives,0);
}

void GameInit(void){
// Intializes player's ship, bunkers, lives and print them
// PlayerSpr will be at bottom-centre
// BunkerSprs[] will be evenly spaced across screen 
	unsigned char x_coord, y_coord; // x-y coordinates 		
	unsigned char i; 	// "for" loops
	float gap; 				// gap b/w bunkers
	unsigned char playerW = Player_Init(0,0).width; 	// player width
	unsigned char playerH = Player_Init(0,0).height; 	// player height
	unsigned char bunkerW = Bunker_Init(0,0).width; 	// bunker width
	unsigned char livesW = Lives_Init(0,0).width; 		// lives width
	unsigned char livesH = Lives_Init(0,0).height; 		// lives height
	
	// Reset global variables
	level = 0;
	score = 0;
	enemyMove = 0;
	enemyDance = 0;
	
	// Destroy any other sprites
	playerBullet.hp = 0;
	BonusSpr.hp = 0 ;
	for(i = 0; i < ENEMYNUM; i++){
		enemySprs[i].hp = 0;
		enemyBullets[i].hp = 0;
		livesSprs[i].hp=0;
	}
	
	// Player initialization
	x_coord=MINSCREENX + (EFFSCREENW - playerW)/2;
	y_coord=MAXSCREENY;
	playerSpr = Player_Init(x_coord,y_coord);
	
	// Bunkers initialization
	y_coord = MAXSCREENY - playerH;
	gap = (float)(EFFSCREENW - BUNKERNUM * bunkerW - playerW) / (float)(BUNKERNUM - 1);
	for(i = 0; i < BUNKERNUM; i++){ // For each bunker
		x_coord = (unsigned char)(MINSCREENX + i * (bunkerW + gap)+playerW/2.0);
		bunkerSprs[i] = Bunker_Init(x_coord, y_coord);
	}
	
	// Lives initialization
	y_coord = MINSCREENY + livesH-1;
	gap=0;
	for (i=0; i<playerSpr.hp; i++){
		x_coord= MAXSCREENX-i*(livesW+gap)-livesW;
		livesSprs[i]=Lives_Init(x_coord, y_coord);
	}
}

void EnemyHordeInit(void){
// Fill enemySprs[] with enemy horde of ENEMYCOLS & ENEMYROWS
// Enemy type randomly varies between rows
// Nokia5110 displays up to 2 rows & 5 columns
// Indexing increases left to right from front row, then following rows
	unsigned char y_coord, x_coord; // individual enemy coordinates
	unsigned char i, j; 						// "for" loops
	unsigned char	var; 							// enemy variant
	unsigned char enemyW = Enemy_Init(0,0,0).width;
	unsigned char enemyH = Enemy_Init(0,0,0).height;
	enemyMove = 0; 									// enemies spawn on left, move right
	
	// Clear enemySprs[] & screen
	for(i = 0; i < ENEMYNUM; i++){ 	// for all enemies
		enemySprs[i].hp = 0; 
	}
	
	// Filling enemySprs[] & print
	for(i = 0; i < ENEMYROWS; i++){ // for each row
		y_coord = MINSCREENY + BONUSH + (enemyH + 1) * (ENEMYROWS - i);	
		var = rand() % 3;	
		for(j = 0; j < ENEMYCOLS; j++){// for each enemy in row
			x_coord = MINSCREENX + j * (enemyW + ENEMYGAPY);											
			enemySprs[i * ENEMYCOLS + j] = Enemy_Init(x_coord, y_coord, var);
		}
	}
}

void SprCrash(sprite* spr1, sprite* spr2, unsigned char explodeFlag){
// Checks sprite colliders
// ExplodeFlag = 1 -> Destroyed sprites get exploded
	if((spr1->hp > 0 && spr2->hp > 0) && CrashCheck(spr1, spr2)){
		Sound_Hit();
		spr1->hp -= 1;
		spr2->hp -= 1;
		score += spr2->pt * level;
		if(explodeFlag == 1 && spr2->hp == 0)
			toExplode = spr2;
	}
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1 s at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

void Timer1_Init(float freq){ 
// Initalize Timer1 to trigger every specified seconds(assuming 880Mhz PLL)
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x02;
  delay = SYSCTL_RCGCTIMER_R;
  TIMER1_CTL_R = 0x00000000;
  TIMER1_CFG_R = 0x00000000; 	// 32-bit mode
  TIMER1_TAMR_R = 0x00000002; // periodic mode, default down-count
  TIMER1_TAILR_R = 80000000 / freq -1;
  TIMER1_TAPR_R = 0;
  TIMER1_ICR_R = 0x00000001;
  TIMER1_IMR_R = 0x00000001;
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|(0x02 <<13); // priority 2
	NVIC_EN0_R |= 1<<21;
	TIMER1_CTL_R = 0x00000001; 
}
