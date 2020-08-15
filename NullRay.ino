/*
  STM32 Raycast Engine
  (C) 2020 nulluser@gmail.com

  Display: SSD1351 Based Waveshare 1.5" RGB OLED on SPI3
  Core: STM32Duino
  Micro:   STM32-NucleoL432KC
*/

#define MODULE "[System]    "

#include "Src/Utility.h"
#include "Src/Display.h"
#include "Src/Ray.h"
#include "Src/World.h"
#include "Src/Player.h"
#include "Src/WorldData.h"
#include "Config.h"

static Display display;         // Graphics Driver
static World world;               // Map
static RayEngine ray;           // Raycast engine
static Player player;           // Player

// Init and clear display
void setup(void)
{
  // Start serial
  Serial.begin(SERIAL_BAUD);
  delay(START_DELAY);

  // Startup Header
  debug("\n\n\n\n");
  debug(MODULE "*******************************\n");
  debug(MODULE PROGRAM " - " VERSION "\n");
  debug(MODULE COPYRIGHT " " LICENSE "\n");
  debug(MODULE OPTIONS "\n");
  debug(MODULE "CPU Speed: %d\n", F_CPU);

  // Start display
  display.init();

  // Load a map
  world.load(demo_world);
  //world.create_random();
  
  // Init display engine
  ray.init();

  // Locate player in center of map
  player.init(world.get_size_x() / 2, world.get_size_y() / 2,  DEGTORAD(45.0));

  // No more heap usage after this point
}

// Update and record framerate
void loop()
{
  static int count = 0;

  // Process any new input
  update_input();

  player.update(world);

  unsigned long start = micros();

  // Make sure we are done copying before rendering
  display.wait_complete();

  // Clear framebuffer
  display.fill(BLACK);

  // Render into framebuffer
  ray.render(display, world, player);

  unsigned long end_update = micros();

  // Copy frame buffer to display
  display.draw_frame();

  unsigned long end_draw = micros();

  //while(1);

  // Display stats once in a while
  if (++count > STAT_COUNT)
  {
    count = 0;

    // Compute and display stats
    unsigned long update_duration = end_update - start;
    unsigned long draw_duration = end_draw - end_update;
    unsigned long total_duration = end_draw - start;

    // Compute updates / second
    float update_rate = 1000000 / (float)(update_duration);
    float draw_rate = 1000000 / (float)(draw_duration);
    float total_rate = 1000000 / (float)(total_duration);

    //Serial.print("Total T: ");    Serial.print((float)total_duration/1000, 3);
    //Serial.print(" Total R: ");   Serial.print(total_rate, 2);
    debug("Update: %6.3f ms %5.2f fps ", (float)update_duration / 1000, update_rate);
    //Serial.print(" Draw T: ");    Serial.print((float)draw_duration/1000,3);
    //Serial.print(" Draw R: ");    Serial.println(draw_rate, 2);
    player.debug_pos();

    debug("\n");
  }
}

// Check for new input
void update_input()
{
  // Take input over serial
  if (Serial.available())
  {
    char ch = Serial.read();

    // Parse WASD
    if (ch == 'q') player.strafe(1);
    if (ch == 'e') player.strafe(-1);
    if (ch == 'w') player.move(0, 1);
    if (ch == 's') player.move(0, -1);
    if (ch == 'a') player.move( 1, 0);
    if (ch == 'd') player.move(-1, 0);
  }
}
