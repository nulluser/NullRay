/*
	DMA based SPI
	(C) 2020 nulluser@gmail.com

	File: DMASPI.h
*/

#ifndef DMASPI_H
#define DMASPI_H

#include <stdint.h>

// Private
#ifdef MODULE_DMASPI

#define MODULE "[DMASPI]    "

// These are just for reference
//#define MOSI_PIN  PB5  // (D11 Pin)
//#define SCK_PIN   PB3  // (D13 Pin)

// DMA2 channel 2 for SPI3_TX on STM32L432KC
#define DMA_INSTANCE               DMA2_Channel2
#define DMA_INSTANCE_IRQ           DMA2_Channel2_IRQn
#define DMA_INSTANCE_IRQHANDLER    DMA2_Channel2_IRQHandler

void slave_disable(void);
void slave_enable(void);
	
#endif

/* SPI Driver */
class DMASPI
{
  public: 

	DMASPI();
	
	void init(int port, int _cs_pin, unsigned long speed);	// Init SPI and DMA

	void spi_write_data(uint8_t v);							// Write single byte
	void spi_write_array(uint8_t *v, uint16_t len);			// Write array of bytes
	void wait_complete();									// Wait for write operation to finish

  private:

};

#endif

