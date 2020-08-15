/*
	SSD1351 Display Driver
	(C) 2020 nulluser@gmail.com
	
	Display driver for SSD1351
	Framebuffer takes up 32768 bytes of ram

	File: Display.cpp
*/

#define MODULE_DISPLAY

#include "Utility.h"
#include "Display.h"

// Constructor
Display::Display()
{
	// Allocate framebuffer
	frame_buffer = new uint16_t[display_x_size * display_y_size];
}

// Init 
void Display::init()
{
	debug(MODULE "Display Init\n");

	cs_pin  = get_gpio(CS_PORT, CS_PIN);
	dc_pin  = get_gpio(DC_PORT, DC_PIN);
	rst_pin = get_gpio(RST_PORT, RST_PIN);
	
	// Data / Command Pin
	gpio_mode(dc_pin, GPIO_MODE_OUTPUT);
	gpio_write(dc_pin, 1); // Data mode
	
	spi.init(SPI_PORT, cs_pin, SPI_SPEED);

	init_module();

	fill(BLUE);	
	draw_frame();
}

// Init the module 
void Display::init_module()
{
	debug(MODULE "Module Init\n");
	
	// Display init commands
	static const uint8_t init_list[] = 
	{
		SSD1351_CMD_COMMANDLOCK,    1,  0x12,				// Set command lock, 1 arg
		SSD1351_CMD_COMMANDLOCK,    1,  0xB1,				// Set command lock, 1 arg
		SSD1351_CMD_DISPLAYOFF,     0,  					// Display off
		SSD1351_CMD_CLOCKDIV,       1,  0xF1,				// 7:4 = Oscillator Freq, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
		SSD1351_CMD_MUXRATIO,       1,  127,
		SSD1351_CMD_DISPLAYOFFSET,  1,  0x0,
		SSD1351_CMD_SETGPIO,        1,  0x00,
		SSD1351_CMD_FUNCTIONSELECT, 1,  0x01,				// Internal (diode drop)
		SSD1351_CMD_PRECHARGE,      1,  0x32,
		SSD1351_CMD_VCOMH,          1,  0x05,
		SSD1351_CMD_NORMALDISPLAY,  0,
		SSD1351_CMD_CONTRASTABC,    3,  0xC8, 0x80, 0xC8,
		SSD1351_CMD_CONTRASTMASTER, 1,  0x0F, 
		SSD1351_CMD_SETVSL,         3,  0xA0, 0xB5, 0x55,
		SSD1351_CMD_PRECHARGE2,     1,  0x01, 
		SSD1351_CMD_DISPLAYON,      0,  					// Main screen turn on
		SSD1351_CMD_SETREWorld,     1,  0b01110100,			// Rotation
		SSD1351_CMD_STARTLINE,      1,  128,				// Start Line
		0                          //						END OF COMMAND LIST
	}; 

	// Reset Chip
	debug(MODULE "Module Reset\n");
	gpio_mode(rst_pin, GPIO_MODE_OUTPUT);
	gpio_write(rst_pin, 1);		delayms(100);
	gpio_write(rst_pin, 0);		delayms(100);
	gpio_write(rst_pin, 1);		delayms(200);

	send_command_list(init_list);
	
	debug(MODULE "Module Init Complete\n");
}


// Copy framebuffer to display
void Display::draw_frame()
{
	// Reset write address to origin
	set_window(0, 0, display_x_size, display_y_size);

	spi.spi_write_array((uint8_t*)frame_buffer, display_x_size * display_y_size*2);
}


// Fill Framebuffer with solid color
void Display::fill(uint16_t color)
{
	uint16_t rev_color = (color << 8) | (color >> 8);    // Swap byte order
	
	for (int i = 0; i < display_x_size * display_y_size; i++)
		frame_buffer[i] = rev_color;
}

// Configure pixel window
void Display::set_window(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h) 
{
	uint16_t x2 = x1 + w - 1, y2 = y1 + h - 1;

	//if (rotation & 1) { // Vertical address increment mode
	//    ssd1351_swap(x1, y1);
	//ssd1351_swap(x2, y2);
	//}

	write_command(SSD1351_CMD_SETCOLUMN); // X range

	spi.spi_write_data(x1);
	spi.spi_write_data(x2);

	write_command(SSD1351_CMD_SETROW); // Y range

	spi.spi_write_data(y1);
	spi.spi_write_data(y2);

	write_command(SSD1351_CMD_WRITERAM); // Begin write
}


/* 
  Low Level 
*/

// Set byte to display as command
void Display::write_command(uint8_t command)
{
	gpio_write(dc_pin, 0); 
	spi.spi_write_data(command);
	gpio_write(dc_pin, 1); 
}

// Send a command and parameters
void Display::send_command(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes) 
{
	write_command(commandByte);

	if (numDataBytes > 0)
		spi.spi_write_array(( uint8_t *)dataBytes,  numDataBytes);
}

// Parse and send a command list
void Display::send_command_list(const uint8_t *list)
{
	// Sent init commands
	uint8_t cmd;
	uint8_t index = 0;

	// Send commands
	while ((cmd = list[index++]) > 0)  // '0' command ends list
	{ 
		uint8_t num_args = list[index++] & 0x7F;

		send_command(cmd, (uint8_t *)(list + index),  num_args);

		index += num_args;
	}
}

// Wait for display transaction to complete
void Display::wait_complete()
{
	spi.wait_complete();
}

// Get display x dimension
uint8_t Display::get_display_x()
{
	return display_x_size;
}
 
// Get display y dimension
uint8_t Display::get_display_y()
{
	return display_y_size;
}

// Get framebuffer for drawing
uint16_t * Display::get_buffer()
{
	return frame_buffer;
}

// Create reversed RGB color
uint16_t make_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	if (r > 31) r = 31;
	if (g > 31) g = 31;
	if (b > 31) b = 31;
	
	return ((uint16_t)(g) << 14) | ((uint16_t)(b) << 8) | ((uint16_t)(r) << 3) | ((uint16_t)(g) >> 2);
}

/* 
  End SPI DMA
*/

