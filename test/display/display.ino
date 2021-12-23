
#include <Wire.h>
// #include <display.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal_I2C.h>
// #include <gy33.h>
// GY33 colorSensor = GY33();

// Display display;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    // Be sure to set your serial console to 115200 baud
    Serial.begin(115200);

    lcd.init(); // initialize the lcd

    // Print a message to the LCD.
    lcd.backlight();
    lcd.print("Hello, world!");

    if (tcs.begin())
    {
        Serial.println(F("Found sensor"));
    }
    else
    {
        Serial.println(F("No TCS34725 found ... check your connections"));
        while (1)
            ; // halt!
    }

    // if (colorSensor.begin())
    // {
    //     Serial.println(F("Found GY33"));
    // }
    // else
    // {
    //     Serial.println(F("No GY33 found ... check your connections"));
    //     while (1)
    //         ; // halt!
    // }
    // if (!display.begin())
    // {
    //     Serial.println(F("SSD1306 allocation failed"));
    //     for (;;)
    //         ; // Don't proceed, loop forever
    // }

    // Serial.println(F("SSD1306 ok"));
    // display.banner();

    delay(2000);
}

void loop()
{

    float red, green, blue;

    tcs.setInterrupt(false); // turn on LED

    delay(3); // takes 50ms to read
    // delay(614);  // takes 50ms to read

    tcs.getRGB(&red, &green, &blue);

    tcs.setInterrupt(true); // turn off LED

    Serial.print(F("R:\t"));
    Serial.print(int(red));
    Serial.print(F("\tG:\t"));
    Serial.print(int(green));
    Serial.print(F("\tB:\t"));
    Serial.print(int(blue));
    Serial.print(F("\n"));

    // uint8_t r, g, b,c ;

    // colorSensor.getRGB(&r, &g, &b, &c);
    // Serial.print(F("GY33 R:\t"));
    // Serial.print(int(r));
    // Serial.print(F("\tG:\t"));
    // Serial.print(int(g));
    // Serial.print(F("\tB:\t"));
    // Serial.print(int(b));
    // Serial.print(F("\n"));

    // display.ssd1306.clearDisplay();
    // display.ssd1306.setTextSize(2);
    // display.ssd1306.setTextColor(SSD1306_WHITE); // Draw white text
    // display.ssd1306.setCursor(0, 0);             // Start at top-left corner
    // display.ssd1306.print("R:\t");
    // display.ssd1306.print(int(red));
    // display.ssd1306.print("\tG:\t");
    // display.ssd1306.print(int(green));
    // display.ssd1306.print("\tB:\t");
    // display.ssd1306.println(int(blue));
    // display.ssd1306.display();
    //  Serial.print(F("\t"));
    //  Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
    Serial.print(F("\n"));
    delay(1000);
}