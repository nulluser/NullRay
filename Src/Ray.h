/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Ray.h
*/

#ifndef RAY_H
#define RAY_H

#include <stdint.h>

#ifdef MODULE_RAY

#define MODULE "[Ray]       "

#define FIELD_OF_VIEW	60			// Field of view in degrees

#define FIXEDPT_COLUMN				// Used fixed point for column
#define FIXED_SHIFT		16			// 16.16 format


#endif

class Display;
class Player;
class World;
class Texture;

// Raycaster
class RayEngine
{
  public:

	RayEngine();

	void init();
	void render(Display &display, World &World, Player &player);

  private:
	
	void render_column(Display &display, uint8_t x, uint16_t size, uint8_t offset, Texture *texture);
	
};

#endif
