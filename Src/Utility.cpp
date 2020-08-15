/*
	STM32 Raycast Engine
	(C) 2020 nulluser@gmail.com

	File: Utility.cpp
*/

#include <malloc.h>

#include "../Config.h"
#include "Utility.h"

/*
	Arch 
*/

// Detect arduino Enviroment
#ifdef ARDUINO

#include <arduino.h>

// Generic print
inline void print_string(char * s)
{
	Serial.print(s);
}

// Memory info 
extern "C" char *sbrk(int i);
extern char _end;				// Use linker definition 
extern char _sdata;
extern char _estack;
extern char _Min_Stack_Size;

static char *ramstart = &_sdata;
static char *ramend = &_estack;
static char *minSP = (char*)(ramend - &_Min_Stack_Size);

// Show memory info
void display_mallinfo(void)
{
	char *heapend = (char*)sbrk(0);
	char * stack_ptr = (char*)__get_MSP();
	struct mallinfo mi = mallinfo();
	uint32_t free_mem = ((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks;

	debug("RAM Start at:                           %x\n", (unsigned long)ramstart);
	debug("Data/Bss end at:                        %x\n", (unsigned long)&_end);
	debug("Heap end at:                            %x\n", (unsigned long)heapend);
	debug("Stack Ptr end at:                       %x\n", (unsigned long)stack_ptr);
	debug("RAM End at:                             %x\n", (unsigned long)ramend);

	debug("Total non-mWorldped bytes (arena):      %d\n", mi.arena);
	debug("# of free chunks (ordblks):             %d\n", mi.ordblks);
	debug("# of free fastbin blocks (smblks):      %d\n", mi.smblks);
	debug("# of Worldped regions (hblks):          %d\n", mi.hblks);
	debug("Bytes in Worldped regions (hblkhd):     %d\n", mi.hblkhd);
	debug("Max. total allocated space (usmblks):   %d\n", mi.usmblks);
	debug("Free bytes held in fastbins (fsmblks):  %d\n", mi.fsmblks);
	debug("Total allocated space (uordblks):       %d\n", mi.uordblks);
	debug("Total free space (fordblks):            %d\n", mi.fordblks);
	debug("Topmost releasable block (keepcost):    %d\n", mi.keepcost);

	debug("Heap RAM Used:                          %d\n", mi.uordblks);
	debug("Program RAM Used:                       %d\n", &_end - ramstart);
	debug("Stack RAM Used:                         %d\n", ramend - stack_ptr);
	debug("Estimated Free RAM:                     %d\n", free_mem);
}

// Millisecond delay
void delayms(uint16_t d)
{
	delay(d);
}

// Return arch gpio reference
int get_gpio(int port, int pin)
{
	if (port == GPIO_PORTA)
	{
		if (pin == 0)  return PA0;  
		if (pin == 1)  return PA1;  
		if (pin == 2)  return PA2;  
		if (pin == 3)  return PA3;  
		if (pin == 4)  return PA4;  
		if (pin == 5)  return PA5;  
		if (pin == 6)  return PA6;  
		if (pin == 7)  return PA7;  
		if (pin == 8)  return PA8;  
		if (pin == 9)  return PA9;  
		if (pin == 10) return PA10; 
		if (pin == 11) return PA11; 
		if (pin == 12) return PA12; 
	}
	if (port == GPIO_PORTB)
	{
		if (pin == 0)  return PB0;  
		if (pin == 1)  return PB1;  
		if (pin == 3)  return PB3; 
		if (pin == 4)  return PB4; 
		if (pin == 5)  return PB5; 
		if (pin == 6)  return PB6; 
		if (pin == 7)  return PB7; 
	} 
	if (port == GPIO_PORTC)
	{
		if (pin == 14) return PC14; 
		if (pin == 15) return PC15;
	} 
	
	debug("Unknown port %d or pin %d", port, pin);
	
	return 0;
}


// Set pin direction
void gpio_mode(int pin, int mode)
{
	if (mode == GPIO_MODE_INPUT) pinMode(pin, INPUT); else
	if (mode == GPIO_MODE_OUTPUT) pinMode(pin, OUTPUT); 
	
}

// Set pin value
void gpio_write(int pin, bool val)
{
	digitalWrite(pin, val);
}

#endif


/*
	Common 
*/

// Generic
void debug(char *format, ...)
{
#ifdef DEBUG
	const unsigned int buff_max = 128;
	static char buffer[buff_max]; 
	va_list args; 

	va_start(args, format);
	vsnprintf(buffer, buff_max-1, format, args);

	Serial.print(buffer);

	va_end(args); 
#endif
}

/* 
	Math 
*/

// Return a random number in the specified range 
double f_rand(double s, double e)
{
	return((e - s)*(rand() / (double)RAND_MAX) + s);
}

//Test lookup table
#ifdef USE_LOOKUP
void lookup_test()
{
	unsigned int steps = 1024;

	// Force sin table init
	float t = sin_l(0);

	unsigned long start1 = micros();

	float sum1 = 0;
	for (int i = 0; i < steps; i++)
		sum1 += sin(i / (float)steps * M_PI / 2);
	unsigned long end1 = micros();
	
	debug("STD: %d   %f\n", (end1 - start1)/1000000, sum1);

	unsigned long start2 = micros();

	float sum2 = 0;
	for (int i = 0; i < steps; i++)
		sum2 += sin_l(i / (float)steps * M_PI / 2);
	unsigned long end2 = micros();
	
	debug(" LU: %d   %f\n", (end2 - start2)/1000000, sum2);
	
	while(1);
}
#endif

// Lookup table based sine
float sin_l(float angle)
{
	#ifdef USE_LOOKUP
	
	static float table[LU_TABLE_SIZE];
	static bool init = true;
	
	if (init)
	{
		debug("Init Sin Lookup\n");
		init = false;
		
		for (uint16_t i = 0; i < LU_TABLE_SIZE; i++)
			table[i] = sin(i * M_PI * 2 / LU_TABLE_SIZE);
	}
		
	if (angle < 0) angle += M_PI * 2;
	if (angle >= M_PI * 2) angle -= M_PI * 2;
	
	int index = angle / (M_PI * 2) * LU_TABLE_SIZE;
	
	return table[index & LU_TABLE_MASK];
	#else
	return sin(angle);
	#endif
}

// Lookup table based cosine
float cos_l(float angle)
{
	#ifdef USE_LOOKUP
	static float table[LU_TABLE_SIZE];
	static bool init = true;
	
	if (init)
	{
		debug("Init Cos Lookup\n");
		init = false;
		
		for (uint16_t i = 0; i < LU_TABLE_SIZE; i++)
			table[i] = cos(i * M_PI * 2 / LU_TABLE_SIZE);
	}
		
	if (angle < 0) angle += M_PI * 2;
	if (angle >= M_PI * 2) angle -= M_PI * 2;
	
	int index = angle / (M_PI * 2) * LU_TABLE_SIZE;

	return table[index & LU_TABLE_MASK];
	#else
	return cos(angle);
	#endif
}

// Lookup table based tangent
float tan_l(float angle)
{

	#ifdef USE_LOOKUP
	static float table[LU_TABLE_SIZE];
	static bool init = true;
	
	if (init)
	{
		debug("Init Tan Lookup\n");
		init = false;
		
		for (uint16_t i = 0; i < LU_TABLE_SIZE; i++)
			table[i] = tan(i * M_PI * 2 / LU_TABLE_SIZE);
	}
		
	if (angle < 0) angle += M_PI * 2;
	if (angle >= M_PI * 2) angle -= M_PI * 2;
	
	int index = angle / (M_PI * 2) * LU_TABLE_SIZE;
	
	return table[index & LU_TABLE_MASK];
	#else
	return tan(angle);
	#endif
}

