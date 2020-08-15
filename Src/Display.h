/*
	SSD1351 Display Driver
	(C) 2020 nulluser@gmail.com

	File: Display.h
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#include "DMASPI.h"

// Basic colors
#define BLACK 		0x0000
#define RED   		0xF800
#define BLUE   		0x00F8

// Private
#ifdef MODULE_DISPLAY
#define MODULE "[Display]   "

#define SPI_PORT	3				// Connected to SPI3
#define SPI_SPEED 	20000000L		// 20mhz works

//#define MOSI_PIN  PB5  // (D11 Pin)
//#define SCK_PIN   PB3  // (D13 Pin)
//#define CS_PIN    PA4  // (A3 Pin)
//#define DC_PIN    PA0  // (A0 Pin)
//#define RST_PIN   PA3  // (A2 Pin)

#define CS_PORT			GPIO_PORTA
#define CS_PIN 			4
#define DC_PORT			GPIO_PORTA
#define DC_PIN 			0
#define RST_PORT		GPIO_PORTA
#define RST_PIN 		3

#define display_x_size	128
#define display_y_size	128

// DMA2 channel 2 for SPI3_TX on STM32L432KC
#define DMA_INSTANCE               DMA2_Channel2
#define DMA_INSTANCE_IRQ           DMA2_Channel2_IRQn
#define DMA_INSTANCE_IRQHANDLER    DMA2_Channel2_IRQHandler

// Constants from Orig Driver
#define SSD1351_CMD_SETCOLUMN 0x15      ///< See datasheet
#define SSD1351_CMD_SETROW 0x75         ///< See datasheet
#define SSD1351_CMD_WRITERAM 0x5C       ///< See datasheet
#define SSD1351_CMD_READRAM 0x5D        ///< Not currently used
#define SSD1351_CMD_SETREWorld 0xA0       ///< See datasheet
#define SSD1351_CMD_STARTLINE 0xA1      ///< See datasheet
#define SSD1351_CMD_DISPLAYOFFSET 0xA2  ///< See datasheet
#define SSD1351_CMD_DISPLAYALLOFF 0xA4  ///< Not currently used
#define SSD1351_CMD_DISPLAYALLON 0xA5   ///< Not currently used
#define SSD1351_CMD_NORMALDISPLAY 0xA6  ///< See datasheet
#define SSD1351_CMD_INVERTDISPLAY 0xA7  ///< See datasheet
#define SSD1351_CMD_FUNCTIONSELECT 0xAB ///< See datasheet
#define SSD1351_CMD_DISPLAYOFF 0xAE     ///< See datasheet
#define SSD1351_CMD_DISPLAYON 0xAF      ///< See datasheet
#define SSD1351_CMD_PRECHARGE 0xB1      ///< See datasheet
#define SSD1351_CMD_DISPLAYENHANCE 0xB2 ///< Not currently used
#define SSD1351_CMD_CLOCKDIV 0xB3       ///< See datasheet
#define SSD1351_CMD_SETVSL 0xB4         ///< See datasheet
#define SSD1351_CMD_SETGPIO 0xB5        ///< See datasheet
#define SSD1351_CMD_PRECHARGE2 0xB6     ///< See datasheet
#define SSD1351_CMD_SETGRAY 0xB8        ///< Not currently used
#define SSD1351_CMD_USELUT 0xB9         ///< Not currently used
#define SSD1351_CMD_PRECHARGELEVEL 0xBB ///< Not currently used
#define SSD1351_CMD_VCOMH 0xBE          ///< See datasheet
#define SSD1351_CMD_CONTRASTABC 0xC1    ///< See datasheet
#define SSD1351_CMD_CONTRASTMASTER 0xC7 ///< See datasheet
#define SSD1351_CMD_MUXRATIO 0xCA       ///< See datasheet
#define SSD1351_CMD_COMMANDLOCK 0xFD    ///< See datasheet
#define SSD1351_CMD_HORIZSCROLL 0x96    ///< Not currently used
#define SSD1351_CMD_STOPSCROLL 0x9E     ///< Not currently used
#define SSD1351_CMD_STARTSCROLL 0x9F    ///< Not currently used

#endif

/* Display Driver */
class Display
{
  public: 

	Display();

	void init(void);						// Init
	void draw_frame();						// Copy framebuffer to display
	void fill(uint16_t color);				// Fill frame buffer

	void wait_complete();					// Wait for copy to finish

	uint8_t get_display_x();				// Get display x size
	uint8_t get_display_y();				// Get display x size
	uint16_t * get_buffer();				// Get frame buffer

  private:

	void init_module(void);

	void set_window(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h);

	void write_command(uint8_t command);
	void send_command(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes);
	void send_command_list(const uint8_t *list);

	uint16_t *frame_buffer;
	
	int cs_pin;
	int dc_pin;
	int rst_pin;
	
	
	DMASPI spi;
};

uint16_t make_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif


