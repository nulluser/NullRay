/*
	DMA based SPI
	(C) 2020 nulluser@gmail.com

	World
	
	File: World.cpp
*/

#define MODULE_WORLD

#include <stdint.h>
#include <math.h>

#include "Utility.h"
#include "World.h"
#include "Display.h"
#include "Texture.h"
#include "TextureData.h"


// For render test
#ifdef RENDER_TEST
static const uint16_t test_texture[] =  
					{ 
						8, 8, 
						0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 
						0xfff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xfff0, 
						0xfff0, 0x0000, 0x0ff0, 0x0000, 0xf00f, 0x0000, 0xffff, 0xfff0, 
						0xfff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xfff0, 
						0xfff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xfff0, 
						0xfff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xfff0, 
						0xfff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xfff0, 
						0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00, 0xff00
					};
#endif
					
					

// Constructor
World::World()
{
	textures = NULL;
	data = NULL;

	num_textures = 0;

	cells_x = 0;
	cells_y = 0;
	size_x = 0;
	size_y = 0;
}


// Load static demo World
void World::load(uint8_t *_world_data)
{
	debug(MODULE "Loading World\n");

	// List of static textures to load
	#ifdef RENDER_TEST
	const uint16_t* texture_list[] = {test_texture, test_texture, test_texture, test_texture};
	#else
	const uint16_t* texture_list[] = {texture_data_1, texture_data_2, texture_data_3, texture_data_4};
	#endif
	
	num_textures = sizeof(texture_list) / sizeof(texture_list[0]);
	textures = new Texture[num_textures];
	
	// Load the textures
	for (uint8_t i = 0; i < num_textures; i++)
		textures[i].load((uint16_t*)texture_list[i]);
	
	// Copy static World data pointer
	cells_x = _world_data[0];
	cells_y = _world_data[1];
	data = _world_data + 2;
	
	debug(MODULE "Size: %d %d\n", cells_x, cells_y);
	
	// Compute world size
	size_x = cells_x * BLOCK_SIZE;
	size_y = cells_y * BLOCK_SIZE;
	
	debug(MODULE "Done Loading World\n");
}

// Create a random World. Uses more ram
void World::create_random(void)
{
	debug(MODULE "Creating Random World\n");
	
	// Add some random textures
	num_textures = 4;
	textures = new Texture[num_textures];
	
	textures[0].create_random(32, 32, 1, 1, 1); // Grey
	textures[1].create_random(32, 32, 0, 1, 0); // Green
	textures[2].create_random(32, 32, 0, 0, 1); // Blue
	textures[3].create_random(32, 32, 1, 0, 1); // Purple
	
	// Setup World size
	cells_x = 16;
	cells_y = 16;
	
	// Allocate World data
	data = new uint8_t[cells_x * cells_y];
	
	size_x = cells_x * BLOCK_SIZE;
	size_y = cells_y * BLOCK_SIZE;
	
	// Clear
	for (int i = 0; i < cells_y * cells_y; i++)
		data[i] = 0;
		
	// Vertical 
	for (int iy = 0; iy < cells_y; iy++)
	{
		data[iy * cells_x] = 2;
		data[(cells_x-1) + iy * cells_x] = (rand() % num_textures)+1;
	}
		
	// Horizontal	
	for (int ix = 0; ix < cells_x; ix++)
	{
		data[ix] = 2;
		data[ix + (cells_y-1) * cells_x] = (rand() % num_textures)+1;
	}
		
	// Set some cells
	uint8_t rand_cells = 30;
		
	for (uint8_t i = 0; i < rand_cells; i++)
	{
		uint8_t x = rand() % cells_x;
		uint8_t y = rand() % cells_y;
		
		set_cell(x, y, (rand() % num_textures)+1);
	}

	// Clear cells near player	
	uint8_t mid_x = cells_x / 2;
	uint8_t mid_y = cells_y / 2;

	for (uint8_t iy = -1; iy <= 1; iy++)
		for (uint8_t ix = -1; ix <= 1; ix++)
			set_cell(mid_x + ix, mid_y + iy, 0);
		
		
	debug(MODULE "Done creating Random World\n");		
}

// Get data from real World cords 
uint8_t World::get_world(float x, float y)
{
	uint8_t world_x = (uint8_t)(x / BLOCK_SIZE);
	uint8_t world_y = (uint8_t)(y / BLOCK_SIZE);

	if (world_x < 0 || world_y < 0 || world_x >= cells_x || world_y >= cells_y)
		return(INVALID_CELL);

	return(data[world_x + world_y * cells_x]);
}

// Set cell
void World::set_cell(uint8_t x, uint8_t y, uint8_t value)
{
	if (x < 0 || y < 0 || x >= cells_x || y >= cells_y)
		return;

	data[x + y * cells_x] = value;
}

// Get texture 
Texture * World::get_texture(uint8_t index)
{
	if (index >= num_textures)
		return NULL;
		
	return(&textures[index]);
}

// World x size in world cords
uint16_t World::get_size_x() 
{
	return size_x; 
}

// World y size in world cords
uint16_t World::get_size_y() 
{ 
	return size_y; 
}
	



