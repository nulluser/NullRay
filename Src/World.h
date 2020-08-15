/*
	DMA based SPI
	(C) 2020 nulluser@gmail.com

	File: World.h
*/

#ifndef WORLD_H
#define WORLD_H

#define INVALID_CELL	0xff

#define BLOCK_SIZE		64				// Size of block in World units
#define BLOCK_MASK		(BLOCK_SIZE-1)	// Bitmask for block size		

// Private
#ifdef MODULE_WORLD

///#define RENDER_TEST			// Static test texture

#define MODULE "[World]     "

#endif


class Texture;

// World
class World
{
  public: 

	World(void);
		
	void load(uint8_t *_world_data);					// Load World from flash
	void create_random(void);							// Create random World
	
	uint8_t get_world(float x, float y);				// Get cell valur from world cords
	void set_cell(uint8_t x, uint8_t y, uint8_t value);	// Set cell value from grid cords
	Texture * get_texture(uint8_t index);				// Return texture for texture index

	uint16_t get_size_x();								// Return size in world cords
	uint16_t get_size_y();
	
  private:

	// Texture Data
	Texture *textures;

	// World Data
	uint8_t *data;

	uint8_t num_textures;

	uint8_t cells_x, cells_y;
	uint16_t size_x,  size_y;
};




#endif
