/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Ray.cpp
*/

#define MODULE_RAY

#include <math.h>

#include "Utility.h"
#include "Display.h"
#include "Ray.h"
#include "World.h"
#include "Texture.h"
#include "Player.h"

// Constructor
RayEngine::RayEngine()
{

}

// Reset game to inital 
void RayEngine::init()
{
	debug(MODULE "Init\n");
}

// Draws a texture vertical line slice
inline void RayEngine::render_column(Display &display, uint8_t x, uint16_t size, uint8_t tx_offset, Texture *texture)
{
	float texture_y = 0;
	float texture_dy = texture->size_y/ 2 / (float)size;   // Calculate texture delta

	uint8_t display_x = display.get_display_x();
	uint8_t display_y = display.get_display_y();
	uint16_t *frame = display.get_buffer();

	// Clip 
	if (tx_offset >= texture->size_x-1) tx_offset = texture->size_x-1;
	
	//tx_offset %= texture->size_x;
	
	// If the slice is clipped by the size, 
	// advance into the texture to compensate
	if (size > display_y / 2) 
	{
		texture_y += (size -  display_y / 2) * texture_dy;
		size = display_y / 2;   // clip the size to the display        
	}

	// Setup starting pointer of texture slice
	uint16_t *p = frame + (display_y / 2 - size) * display_x + x;
	
	// Dump the line to the frame buffer	
	#ifdef FIXEDPT_COLUMN
		
	#define FIXED_SHIFT		16
	
	// Fixed point column render
	uint32_t texture_y_int  = texture_y  * (1<<FIXED_SHIFT);
	uint32_t texture_dy_int = texture_dy * (1<<FIXED_SHIFT);

	uint8_t tx_size_x = texture->size_x;
	uint8_t tx_size_y = texture->size_y;

	uint16_t * texture_ptr = texture->data + tx_offset;

	// This is the core loop, needs to be fast
	for (uint8_t i = 0; i < size * 2; i++)
	{
		// Copy texture color to frame buffer
		*p = texture_ptr[(texture_y_int >> FIXED_SHIFT) * tx_size_x];

		p += display_x;  // Advance index into frame buffer
		
		texture_y_int += texture_dy_int; // Advance index into texture
	}
	
	#else
	// Float rendering
	for (uint16_t i = 0; i < size * 2; i++)
	{
		unsigned int cur_y = (int)(texture_y);

		// Clamp y_index in case of round off error
		//if (cur_y > texture->size_y - 1) cur_y = 0;

		// Index into the texture
		*p = texture->data[tx_offset + cur_y * texture->size_x];

		p += display_x;  // Advance index into frame buffer
		
		texture_y += texture_dy; // Advance index into texture
	}
	#endif
	
}

// Cast ray into map
inline uint8_t cast_ray(World &world, float &x, float &y, float dx, float dy)
{
	uint8_t cell;
	
	// Check for intersections
	while((cell = world.get_world(x, y)) == 0)
	{
		x += dx;
		y += dy;
	}
	
	return cell;
}

// Render to display
void RayEngine::render(Display &display, World &world, Player &player)
{
	float player_x, player_y, player_angle;
	
	player.get_pos(player_x, player_y, player_angle);
	
	// Calculate the starting ray angle
	float ray_angle = player_angle + DEGTORAD(FIELD_OF_VIEW) / 2.0;

	uint8_t display_x = display.get_display_x();
	uint8_t display_y = display.get_display_y();
	uint16_t *frame = display.get_buffer();
	
	// Snap to block boundry
	float start_vert_x = (((int)player_x / BLOCK_SIZE) +1) * BLOCK_SIZE; 
	float start_horz_y = (((int)player_y / BLOCK_SIZE) +1) * BLOCK_SIZE;  
	
	// Render a slice for every x
	for (uint8_t x = 0; x < display_x; x++)
	{
		uint8_t vert_cell = 0, horz_cell = 0;	// World value at current ray

		float vert_x = 0,  vert_y = 0;		// Ray position for vertical check
		float vert_dx = 0, vert_dy = 0;		// Ray delta for verticalcheck

		float horz_y = 0,  horz_x = 0;		// Ray position for horizontal check
		float horz_dx = 0, horz_dy = 0;		// Ray delta for check

		// Setup inital positions 
		vert_x = start_vert_x; horz_y = start_horz_y;
		
		// Facing right special case
		if (ray_angle == DEGTORAD(0))  {vert_dx = BLOCK_SIZE;} else
		// Facing up special case
		if (ray_angle == DEGTORAD(90)) {horz_dy = BLOCK_SIZE;} else
		// Facing left special case
		if (ray_angle == DEGTORAD(180)){vert_dx = -BLOCK_SIZE; vert_x -= BLOCK_SIZE + 1;} else
		// Facing down special case
		if (ray_angle == DEGTORAD(270)){horz_dy = -BLOCK_SIZE; horz_y -= BLOCK_SIZE + 1;} else
		// Normal case
		{
			float tan_angle = tan_l(ray_angle);				// Tangent for projection angle
			float inv_tan_angle = 1.0 / tan_l(ray_angle);	// InvTangent for projection angle

			// Facing left
			if (cos(ray_angle) < 0)
			{
				vert_x -= BLOCK_SIZE + 1;
				vert_dx = -BLOCK_SIZE;
				vert_dy = -BLOCK_SIZE * tan_angle;
			}
			else
			// Facing right
			{
				vert_dx = BLOCK_SIZE;
				vert_dy = BLOCK_SIZE * tan_angle;	
			}
			
			// Facing Down
			if (sin(ray_angle) < 0)
			{
				horz_y -= BLOCK_SIZE + 1;
				horz_dx = -BLOCK_SIZE * inv_tan_angle;
				horz_dy = -BLOCK_SIZE;
			}
			else
			// Facing Up
			{
				horz_dx = BLOCK_SIZE * inv_tan_angle;
				horz_dy = BLOCK_SIZE;
			}

			// Calculate starting positions
			vert_y = player_y + (vert_x - player_x) * tan_angle;
			horz_x = player_x + (horz_y - player_y) * inv_tan_angle;
		}

		// Cast rays 
		vert_cell = cast_ray(world, vert_x, vert_y, vert_dx, vert_dy); // Check for vertical
		horz_cell = cast_ray(world, horz_x, horz_y, horz_dx, horz_dy); // Check for horizontal
		
		// Compute squared distances, we will only be using one
		float vert_dist  =  (player_x - vert_x) * (player_x - vert_x) + 
							(player_y - vert_y) * (player_y - vert_y);

		float horz_dist  =  (player_x - horz_x) * (player_x - horz_x) + 
							(player_y - horz_y) * (player_y - horz_y);

		float dist;					// Actual distance to wall
		int offset;					// texture offset
		uint8_t texture_idx;		// texture index
		
		// Find closest wall
		if (horz_dist < vert_dist)
		{
			dist = sqrt(horz_dist); 
			offset = (int)horz_x & BLOCK_MASK;
			texture_idx = horz_cell;
		}
		else
		{
			dist = sqrt(vert_dist); 
			offset = (int)vert_y & BLOCK_MASK;
			texture_idx = vert_cell;
		}
		
		dist *= cos_l(player_angle - ray_angle);    // Correct for fisheye effect

		// Make sure column is valid
		if (dist > 0 && texture_idx != INVALID_CELL && texture_idx > 0)
		{
			// Calculate slice height
			uint8_t vert_scale = BLOCK_SIZE / 2;
			uint16_t size = vert_scale * display_y / dist;

			// Get texture data for cell
			Texture *texture = world.get_texture(texture_idx - 1);
			
			// Adjust for texture size
			uint8_t tx_offset = (offset * texture->size_x) / BLOCK_SIZE;
			
			render_column(display, x, size, tx_offset, texture); 
		}
		
		ray_angle -= DEGTORAD(FIELD_OF_VIEW) / (float)display_x;
	}
}
/* End of render */

