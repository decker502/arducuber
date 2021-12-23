#ifndef LED_H
#define LED_H

#include "global.h"

class Led
{
public:
  Led(int r = 24,
      int g = 26,
      int b = 28)
  {
    redPin = r;
    greenPin = g;
    bluePin = b;
  }

  void begin()
  {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
  }

  void on(uint8_t r, uint8_t g, uint8_t b)
  {
#ifdef DEBUG
    Serial.println();
    Serial.print(F("Led on; r:"));
    Serial.print(r);
    Serial.print(F(" g:"));
    Serial.print(g);
    Serial.print(F(" b:"));
    Serial.println(b);
#endif

    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
    // delay(100);
  }

  void off()
  {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }

private:
  int redPin;
  int greenPin;
  int bluePin;
};

#endif