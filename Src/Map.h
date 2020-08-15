/*
	DMA based SPI
	(C) 2020 nulluser@gmail.com

	File: Position.h
*/
#ifndef MAP_H
#define MAP_H

#ifdef MODULE_MAP

#define BLOCK_SIZE		64		// Grid elements per block
#define MAX_TEXTURES	4		// Maximum textures

// Map Config
#define MAP_X_SIZE		16
#define MAP_Y_SIZE		16

// Texture config
#define TEXTURE_X		16
#define TEXTURE_Y		16

#endif

// Texture	
struct Texture
{
	uint16_t *data;		// Row major rgb data
	uint8_t size_x, size_y;		// Texture dimensions
};

// Map
class Map
{
  public: 

	Map();
	
	void create();
	void load(void);
	
	uint8_t get_cell(float x, float y);
	Texture * get_texture( uint8_t index);

	uint16_t get_block_size();
	uint16_t get_size_x();
	uint16_t get_size_y();
	
  private:

	// Texture Data
	Texture *textures;

	// Map Data
	uint8_t *data;

	uint8_t num_textures;

	int16_t cells_x, cells_y;
	int16_t size_x,  size_y;
	
	uint16_t block_size;
};

#endif
