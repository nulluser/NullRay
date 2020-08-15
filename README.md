# NullRay - STM32 RayCaster  
  
## Raycasting Engine for the STM32  

Board:   STM32-NucleoL432KC  
Display: SSD1351 Based Waveshare 1.5" RGB OLED  
  
DMA Based SPI interface to display. ~50 FPS.
  
Compiles on Arduino with the STM32Duino core  
  
Movement is over the serial interface  
 WS - Move  
 AD - Turn  
 QE - Strafe  
  
Wiring  
  
* MOSI_PIN  PB5  // (D11 Pin)  
* SCK_PIN   PB3  // (D13 Pin)  
* CS_PIN    PA4  // (A3 Pin)  
* DC_PIN    PA0  // (A0 Pin)  
* RST_PIN   PA3  // (A2 Pin)  
  
[Video](https://youtu.be/PysdKwrtMgs)  
  
  