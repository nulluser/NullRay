/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com
	
	Player
	
	File: Player.cpp
*/

#define MODULE_PLAYER

#include <math.h>

#include "Utility.h"
#include "Player.h"
#include "World.h"

// Constructor
Player::Player()
{
	pos_x = 0;
	pos_y = 0;
	angle = 0;
	
	move_speed = 0;
	turn_speed = 0;
}

// Init
void Player::init(float _pos_x, float _pos_y, float _angle)
{
	debug(MODULE "Init\n");
	
	pos_x = _pos_x;
	pos_y = _pos_y;
	angle = _angle;
	
	move_speed = 0;
	turn_speed = 0;
}

// Apply movement commands
void Player::update(World &world)
{
	
	turn_speed = 0.02;
	
	// Turning
	angle += turn_speed;

	WRAP(angle);  // Wrap around 0..2PI

	// Test position
	float next_x = pos_x;
	float next_y = pos_y;

	// Need lookup table
	// Forward / Back
	if (fabs(move_speed) > 0)
	{
		next_x += move_speed * cos_l(angle);
		next_y += move_speed * sin_l(angle);
	}

	// Strafe
	if (fabs(strafe_speed) > 0)
	{
		next_x += strafe_speed * cos_l(angle + M_PI/2.0);
		next_y += strafe_speed * sin_l(angle + M_PI/2.0);
	}

	// Make sure area is clear
	if (world.get_world(next_x, next_y) == 0)
	{
		pos_x = next_x;
		pos_y = next_y;
	} 
	else
	{
		move_speed = 0;
		strafe_speed = 0;
	}
	
	// Damping
	move_speed *= MOVE_DAMP_RATE;;
	strafe_speed *= STRAFE_DAMP_RATE;
	turn_speed *= TURN_DAMP_RATE;

	// Stall
	if (fabs(move_speed) < MIN_SPEED) move_speed = 0;
	if (fabs(strafe_speed) < MIN_STRAFE) strafe_speed = 0;
	if (fabs(turn_speed) < MIN_TURN) turn_speed = 0;
}

// Set movement commands
void Player::move(float d_angle, float d_dist)
{
	move_speed = d_dist * MOVE_RATE;
	turn_speed = d_angle * TURN_RATE;
}

// Set movement commands
void Player::strafe(float s)
{
	strafe_speed = s * STRAFE_RATE;
}

// Return current pos and angle
void Player::get_pos(float &_x, float &_y, float &_angle)
{
	_x = pos_x;
	_y = pos_y;
	_angle = angle;
}

// Print current position and angle
void Player::debug_pos()
{
	debug("Pos: (%5.2f, %5.2f) %2.1f  ", pos_y, pos_y, RADTODEG(angle));
}

