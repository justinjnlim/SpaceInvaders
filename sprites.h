// sprites.h
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015

#ifndef __SPRITES_INCLUDED__
#define __SPRITES_INCLUDED__

typedef struct {
	// Generic sprite struct
	unsigned char x; 									// x-coordinate
	unsigned char y; 									// y-coordinate
	unsigned char hp; 								// hit points
	unsigned char pt; 								// score points for killing sprite
	unsigned char width;							// sprite's width in px
	unsigned char height;							// sprite's height in px
	const unsigned char* explode[2];	// sprite exploding image
	const unsigned char* block; 			// image to cover & erase sprite
	const unsigned char* image[5]; 		// sprite's image(s)
} sprite;

// Intialize player's ship sprite at (x,y)
// Returns: sprite of player's ship
sprite Player_Init(unsigned char x, unsigned char y);


// Initialize bunker at (x,y) with specified hp
// Returns: sprite of bunker
sprite Bunker_Init(unsigned char x, unsigned char y);


// Initialize lives at (x,y)
// Returns: sprite of lives
sprite Lives_Init(unsigned char x, unsigned char y);
	

// Intialize 1 enemy sprite of specified variant at (x,y)
// Returns: sprite of enemy
sprite Enemy_Init(unsigned char x, unsigned char y, unsigned char variant);


// Intialize a enemy bonus at (x,y)
// Returns: sprite of enemy bonus
sprite SmallEnemyBonus_Init(unsigned char x, unsigned char y);
	

// Initialize laser sprite at center of sprite 
// direction = 0 -> downward laser, direction = 1 -> upwards laser
// Returns: sprite of laser
sprite ShootLaser(sprite* spr, unsigned char updir);

// Checks if 2 sprites have collided
// returns: 1 if sprites crash, 0 otherwise
unsigned char CrashCheck(sprite* spr1, sprite* spr2);

#endif
