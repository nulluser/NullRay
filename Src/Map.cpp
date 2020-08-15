/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	Map
	
	File: Map.cpp
*/

#define MODULE_MAP

#include <stdint.h>
#include <math.h>

#include "Utility.h"
#include "Map.h"
#include "Display.h"

static uint8_t map_data [] = {  1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 
                                1, 0, 0, 0, 0, 0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 1, 
                                1, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 2, 
                                1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 2, 0, 0, 0, 0, 1, 
                                1, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 2, 2, 0, 0, 2, 
                                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 
                                1, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 2, 
                                1, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 
                                1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 
                                1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 2, 0, 0, 2, 
                                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 1, 
                                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 0, 0, 0, 2, 
                                1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                                1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                                1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3 } ;


// Constructor
Map::Map()
{
	
	
}

// Load static demo map
void Map::load(void)
{
	textures = new Texture[MAX_TEXTURES];
	
	num_textures = 4;

	// Allocate and load textures
	for (uint8_t i = 0; i < num_textures; i++)
	{
		textures[i].size_x = TEXTURE_X;
		textures[i].size_y = TEXTURE_Y;
		
		uint16_t data_size = textures[i].size_x * textures[i].size_y;
		
		textures[i].data = new uint16_t[data_size];

		// Load color data
		for (int j = 0; j < data_size; j++)
		{
			uint8_t c = rand() % 32;
			uint16_t color;
			
			if (i == 0) color = make_rgb(c, c, c);
			if (i == 1) color = make_rgb(0, c, 0);
			if (i == 2) color = make_rgb(0, 0, c);
			if (i == 3) color = make_rgb(0, c, c);
			
			textures[i].data[j]  = color;
		}	
		
	}
	
	data = map_data;
	
	
	
	// error with overrunning map?
	
	
	for (int iy = 0; iy < MAP_Y_SIZE; iy++)
		for (int ix = 0; ix < MAP_X_SIZE; ix++)
		{
			data[ix + iy * MAP_X_SIZE] = 0;
		}
		
	
	for (int iy = 0; iy < MAP_Y_SIZE; iy++)
		//for (int iy = 0; iy < MAP_Y_SIZE; iy++)
		{
			data[iy * MAP_X_SIZE] = 2;
			data[(MAP_X_SIZE-1) + iy * MAP_X_SIZE] = 2;
		}
		

	for (int ix = 0; ix < MAP_X_SIZE; ix++)

		{
			data[ix] = 2;
			data[ix + (MAP_Y_SIZE-1) * MAP_X_SIZE] = 2;
		}

		
	
	cells_x = MAP_X_SIZE;
	cells_y = MAP_Y_SIZE;
	
	size_x = cells_x * BLOCK_SIZE;
	size_y = cells_y * BLOCK_SIZE;
}

// Get data from real map cords 
uint8_t Map::get_cell( float x, float y)
{
	int map_x = (int)(x / BLOCK_SIZE);
	int map_y = (int)(y / BLOCK_SIZE);

	if (map_x < 0 || map_y < 0 || map_x >= MAP_X_SIZE || map_y >= MAP_Y_SIZE)
		return(0); // TODO  -1?

	return(data[map_x + map_y * MAP_X_SIZE]);
}


// Get data from real map cords 
Texture * Map::get_texture( uint8_t index)
{
	if (index >= num_textures)
		return NULL;
		
	return(&textures[index]);
}



uint16_t Map::get_size_x() 
{
	return size_x; 
};

uint16_t Map::get_size_y() 
{ 
	return size_y; 
};
	
uint16_t Map::get_block_size()
{
	return BLOCK_SIZE;	
	
}


