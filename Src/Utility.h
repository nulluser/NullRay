/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Utility.h
*/

#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>
#include <stdint.h>

/*
 Arch
*/
 
// Ports
#define GPIO_PORTA			1
#define GPIO_PORTB			2
#define GPIO_PORTC			3
#define GPIO_PORTD			4

// Port Modes
#define GPIO_MODE_INPUT		1
#define GPIO_MODE_OUTPUT	2

void debug(char *format, ...);				// Debug printf
void delayms(uint16_t delay);
int get_gpio(int port, int pin);
void gpio_mode(int pin, int mode);			// Set pin direction
void gpio_write(int pin, bool val); 		// Set pin value
void display_mallinfo(void);				// Memory info

/* 
	Math 
*/

//#define USE_LOOKUP  // The lookup tables were slower on SMT32
#define LU_TABLE_SIZE  1024
#define LU_TABLE_MASK 0x3ff

double f_rand(double s, double e);			// Random number in range

float sin_l(float angle);					// Lookup table based trig
float cos_l(float angle);
float tan_l(float angle);

#define DEGTORAD(a) (((a) * M_PI / 180.0))    // Macro to convert degrees to radians
#define RADTODEG(a) (((a) * 180.0 / M_PI))    // Macro to convert degrees to radians

#define WRAP(a) {if (a<0) a += 2*M_PI; if (a>=2*M_PI) a -= 2* M_PI; }

#endif

