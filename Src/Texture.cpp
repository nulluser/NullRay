/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com
	
	File: Texture.cpp
*/

#define MODULE_TEXTURE

#include <stdint.h>
#include <math.h>

#include "Utility.h"
#include "Texture.h"
#include "Display.h"
#include "Perlin.h"

// Create a random texture
void Texture::create_random(uint8_t _size_x, uint8_t _size_y, uint8_t color_r, uint8_t color_g, uint8_t color_b)
{
	size_x = _size_x;
	size_y = _size_y;
	
	uint16_t data_size = size_x * size_y;
	
	data = new uint16_t[data_size];

	// Load color data
	for (int iy = 0; iy < size_y; iy++)
	{
		for (int ix = 0; ix < size_x; ix++)
		{
			uint8_t c = rand() % 32;
			
			////float perlin(float x, float y, float gain, int octaves, int hgrid);
			float n =  perlin(ix/(float)(size_x), iy/(float)(size_y), 1.0, 3, 1);
			
			c = n * 16 + 16;
			
			data[ix + iy * size_x] = make_rgb(c * color_r, c * color_g, c * color_b);
		}
	}		
}
	
	
// Load static texture from flash
void Texture::load(const uint16_t *_data)
{
	// First two words are the size of texture, rest is 16 bit texture data
	size_x = _data[0];
	size_y = _data[1];
	data = (uint16_t *)_data + 2;	// Skip size words

	debug(MODULE "Loading Texture: %d %d\n", size_x, size_y);

	if (size_x == 0 || size_y == 0)
	{
		debug(MODULE "Invalid size\n");
		while(1);
		return;
		
	}
}
