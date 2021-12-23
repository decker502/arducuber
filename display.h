#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

class Display
{
public:
  Display(int screenWidth = 128, int screenHeight = 64) : ssd1306(screenWidth, screenHeight, &Wire, OLED_RESET)
  {
  }

  bool begin()
  {
    bool s = ssd1306.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    if (!s)
      return false;
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    ssd1306.display();
    delay(2000); // Pause for 2 seconds
    return true;
  }

  void banner()
  {
    ssd1306.clearDisplay();
    ssd1306.setTextSize(2);              // Normal 1:1 pixel scale
    ssd1306.setTextColor(SSD1306_WHITE); // Draw white text
    ssd1306.setCursor(0, 0);             // Start at top-left corner
    ssd1306.println(F("MindCuber v2.2!"));
    ssd1306.println("----------------");
    ssd1306.display();
  }

// private:
  Adafruit_SSD1306 ssd1306;
};

#endif