/*
	DMA based SPI
	(C) 2020 nulluser@gmail.com

	Configured for SPI3 on STM32L4
	
	File: DMASPI.cpp
*/

#define MODULE_DMASPI

#include <stm32l4xx_hal.h>

#include "Utility.h"
#include "DMASPI.h"

// Needed for some ISRs, must be module scope
static DMA_HandleTypeDef dma_handle; 
static SPI_HandleTypeDef spi_handle;
static volatile uint8_t dma_pending = false;
static int cs_pin; // Needs to be moved for multi port support

// Constructor
DMASPI::DMASPI()
{
	dma_pending = false;
}

// Init
void DMASPI::init(int port, int _cs_pin, unsigned long speed)
{
	debug(MODULE "Init\n");
	debug(MODULE "Port: %d CS Pin: %d Speed: %d\n", port, _cs_pin, speed);
		
	cs_pin = _cs_pin;		// Link chip select

	gpio_mode(cs_pin, GPIO_MODE_OUTPUT);
	slave_disable();

	// Setup IO pins for SPI
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;  // PB3=SCK, PB5=MOSI
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// Configure SPI
	spi_handle.Instance = SPI3;
	spi_handle.Init.Mode = SPI_MODE_MASTER;
	spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
	spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
	spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	spi_handle.Init.NSS = SPI_NSS_SOFT;
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; //  20MHZ
	spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
	spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi_handle.Init.CRCPolynomial = 7;
	spi_handle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	spi_handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE; // SPI_NSS_PULSE_ENABLE;
	
	if (HAL_SPI_Init(&spi_handle) != HAL_OK)
	{
		debug(MODULE "Unable to init SPI3\n"); 
		return;
	}

	// Configure DMA
	__HAL_RCC_DMA2_CLK_ENABLE();

	dma_handle.Instance                 = DMA2_Channel2;			// SPI3 TX
	dma_handle.Init.Request             = DMA_REQUEST_3;			// SPI3 TX
	dma_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	dma_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
	dma_handle.Init.MemInc              = DMA_MINC_ENABLE;
	dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	dma_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	dma_handle.Init.Mode                = DMA_NORMAL;
	dma_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
 
	if(HAL_DMA_Init(&dma_handle) != HAL_OK)
	{
		debug(MODULE "Unable to init DMA\n"); 
		return;
	}

	__HAL_LINKDMA(&spi_handle, hdmatx, dma_handle);// Link SPI to DMA

	HAL_NVIC_SetPriority(DMA_INSTANCE_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(DMA_INSTANCE_IRQ);

	dma_pending = false;

	debug(MODULE "Complete\n");
}

// Interrupt handler for SPI TX Complete. Must be extern "C" 
extern "C" 
{
	void DMA_INSTANCE_IRQHANDLER(void)
	{
		HAL_DMA_IRQHandler(&dma_handle);
	}
}

// HAL DMA ISR callback from HAL_SPI_Transmit_DMA
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	// Release CS
	slave_disable();
	
	// Write complete
	dma_pending = 0;
}

// Write single byte
void DMASPI::spi_write_data(uint8_t v)
{
	//debug("Data %x\n", v);
	
	while(dma_pending);

	dma_pending = 1;

	slave_enable();

	if(HAL_SPI_Transmit_DMA(&spi_handle, (uint8_t *)&v, 1) != HAL_OK)
		debug("Write data error\n");

	while(dma_pending);
}

// Write array
void DMASPI::spi_write_array(uint8_t *v, uint16_t len)
{
	// Do not send zero length
	if (len == 0) return;

	//debug("Array (%d)\n", len);
	//for (int i = 0; i < len; i++)
	//	debug("%x ", v[i]);
	//debug("\n");

	// Make sure previous completed
	while(dma_pending);

	dma_pending = 1;

	slave_enable();

	if (HAL_SPI_Transmit_DMA(&spi_handle, (uint8_t *)v, len) != HAL_OK) 
		debug("Write array error");
}

// Waituntil SPI transaction is complete
void DMASPI::wait_complete()
{
	while(dma_pending);
}

// Disable the slave device
inline void slave_disable(void)
{
	gpio_write(cs_pin, 1); 
}

// Enable the slace device
inline void slave_enable(void)
{
	gpio_write(cs_pin, 0); 
}
