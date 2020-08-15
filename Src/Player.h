/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Player.h
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

// Private
#ifdef MODULE_PLAYER

#define MODULE "[Player]    "

#define MIN_SPEED			0.01
#define MIN_STRAFE			0.01
#define MIN_TURN			0.001

#define MOVE_RATE			5.0
#define STRAFE_RATE			5.0
#define TURN_RATE			0.03

#define MOVE_DAMP_RATE		0.9
#define STRAFE_DAMP_RATE	0.9
#define TURN_DAMP_RATE		0.9

	
#endif

class World;

/* SPI Driver */
class Player
{
  public: 

	Player();
	
	void init(float _pos_x, float _pos_y, float _angle);
	void update(World &world);
	void get_pos(float &x, float &y, float &angle);
	void move(float d_angle, float d_dist);
	void strafe(float dist);
	
	void debug_pos();

  private:

	float pos_x;
	float pos_y;
	float angle;
	
	float move_speed;
	float strafe_speed;
	float turn_speed;
	

};

#endif

