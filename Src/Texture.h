/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Texture.h
*/

#ifndef TEXTURE_H
#define TEXTURE_H

// Private
#ifdef MODULE_TEXTURE

#define MODULE "[Texture]   "

#endif

// Texture	
struct Texture
{
	void load(const uint16_t *data);
	
	void create_random(uint8_t _size_x, uint8_t _size_y, uint8_t cr, uint8_t cg, uint8_t cb); // Create a random texture
	
	uint16_t *data;				// Row major rgb data
	uint8_t size_x, size_y;		// Texture dimensions
};

#endif
