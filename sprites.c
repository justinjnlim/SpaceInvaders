// sprites.c
// Justin Lim
// Runs on LM4F120/TM4C123
// Space Invaders - EDX Embedded Systems Final Project
// 05/05/2015


#include "sprites.h"
#include "spriteBMP.c"

#define BUNKERMAXHP 30 	// Bunker's max hp
#define PLAYERLIVES 5 	// Players lives

sprite Player_Init(unsigned char x, unsigned char y){
// Intialize player's ship sprite at (x,y)
// Returns: sprite of player's ship
	sprite player;
	player.x = x;
	player.y = y;
	player.hp = PLAYERLIVES;
	player.pt = 0;
	player.image[0] = PlayerShip;
	player.explode[0] = PlayerShipExplode0;
	player.explode[1] = PlayerShipExplode1;
	player.width = PlayerShip[18];
	player.height = PlayerShip[22];
	return player;
}

sprite Bunker_Init(unsigned char x, unsigned char y){
// Initialize bunker at (x,y)
// Returns: sprite of bunker
	sprite bunker;
	bunker.x = x;
	bunker.y = y;
	bunker.hp = BUNKERMAXHP;
	bunker.pt = 0;
	bunker.image[0] = BunkerUndamage;
	bunker.image[1] = BunkerModerateGenericDamage;
	bunker.image[2] = BunkerHeavyGenericDamage;
	bunker.width = BunkerUndamage[18];
	bunker.height = BunkerUndamage[22];
	return bunker;
}

sprite Lives_Init(unsigned char x, unsigned char y){
// Initialize lives icons at (x,y)
// Returns: sprite of lives icons
	sprite lives;
	lives.x = x;
	lives.y = y;
	lives.hp = 1;
	lives.pt = 0;
	lives.image[0] = Lives;
	lives.width = Lives[18];
	lives.height = Lives[22];
	return lives;
}

sprite Enemy_Init(unsigned char x, unsigned char y, unsigned char variant){
// Intialize 1 enemy sprite of specified variant at (x,y)
// Returns: sprite of enemy
	sprite enemy;
	enemy.x = x;
	enemy.y = y;
	enemy.hp = 1;
	if(variant == 0){ // pick appropriate variant
		enemy.image[0] = SmallEnemy3PointA;
		enemy.image[1] = SmallEnemy3PointB;
		enemy.pt = 3;
		enemy.width = SmallEnemy3PointA[18];
		enemy.height = SmallEnemy3PointA[22];
	} else if(variant == 1){
		enemy.image[0] = SmallEnemy2PointA;
		enemy.image[1] = SmallEnemy2PointB;
		enemy.pt = 2;
		enemy.width = SmallEnemy2PointA[18];
		enemy.height = SmallEnemy2PointA[22];
	} else if(variant == 2){
		enemy.image[0] = SmallEnemy1PointA;
		enemy.image[1] = SmallEnemy1PointB;
		enemy.pt = 1;
		enemy.width = SmallEnemy1PointA[18];
		enemy.height = SmallEnemy1PointA[22];
	}
	enemy.explode[0] = EnemyShipExplode0;
	enemy.explode[1] = EnemyShipExplode1;
	return enemy;
}

sprite SmallEnemyBonus_Init(unsigned char x, unsigned char y){
// Intialize a bonus ship at (x,y)
// Returns: sprite of bonus ship
	sprite bonusShip;
	bonusShip.x = x;
	bonusShip.y = y;
	bonusShip.hp = 1;
	bonusShip.pt = 10;
	bonusShip.image[0] = SmallEnemyBonusA;
	bonusShip.image[1] = SmallEnemyBonusB;
	bonusShip.image[2] = SmallEnemyBonusC;
	bonusShip.explode[0] = SmallEnemyBonusExplode;
	bonusShip.explode[1] = SmallEnemyBonusExplode;
	bonusShip.width = SmallEnemyBonusA[18];
	bonusShip.height = SmallEnemyBonusA[22];
	return bonusShip;
}

sprite ShootLaser(sprite* spr, unsigned char updir){
// Initialize laser sprite at center of sprite 
// direction = 0 -> downward laser, direction = 1 -> upwards laser
// Returns: sprite of laser
	sprite laser;
	laser.width = Laser[18];
	laser.height = Laser[22];
	laser.x = spr->x + spr->width / 2 - 1; //shoot from the centre of the sprite
	laser.y = updir == 0 ? spr->y + laser.height : spr->y - spr->height; // 
	if(updir == 0)
		laser.y = spr->y + laser.height - 1;
	else
		laser.y = spr->y - spr->height;
	laser.hp = 1;
	laser.pt = 0;
	laser.image[0] = Laser;
	return laser;
}

unsigned char CrashCheck(sprite* spr1, sprite* spr2){
// Checks if 2 sprites have collided
// returns: 1 if sprites crash, 0 otherwise
	char l, r, t, b;
	l=(spr1->x > spr2->x) && (spr1->x - spr2->x < spr2->width);		// spr1 collides w/ spr2 on left
	r=(spr1->x < spr2->x) && (spr2->x - spr1->x < spr1->width);		// spr1 collies w/ spr2 on right
	t=(spr1->y > spr2->y) && (spr1->y - spr2->y < spr1->height);	// spr1 collides w/ spr2 on top
	b=(spr1->y < spr2->y) && (spr2->y - spr1->y < spr2->height);	// spr1 collides w/ spr2 on bottom
	
	if((l || r ) && (t || b)) 
		return 1;
	else
		return 0;
}
