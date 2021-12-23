#ifndef BUTTON_H
#define BUTTON_H

#include <utility/BricktronicsSettings.h>

#define BTN_LEFT 0
#define BTN_RIGHT 1
#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_CONFIRM 4

const int keysRange[5][2] = {
    {0, 20},
    {151, 250},
    {21, 100},
    {101, 150},
    {251, 500},
};

class Button
{
public:
  // Constructor - Simple constructor accepts an input pin
  Button(uint8_t inputPin) : _inputPin(inputPin) {}

  // Constructor - Advanced constructor accepts a SensorSettings
  // struct to also override the low-level Arduino functions.
  Button(const BricktronicsSensorSettings &settings) : _inputPin(settings.ANA) {}

  // Set up the input pin
  void begin(void)
  {
    pinMode(_inputPin, INPUT_PULLUP);
  }

  // Returns true if the button is currently pressed
  bool isPressed(byte key)
  {

    int v = analogRead(_inputPin);

    if (v > keysRange[key][0] && v < keysRange[key][1])
      return true;
    return false;
  }

  // Returns true if the button is currently released
  bool isReleased(byte key)
  {
    return !isPressed(key);
  }

private:
  // We really don't like to hide things inside private,
  // but if we did, these would be the private items.
  uint8_t _inputPin;
};

#endif // #ifndef BUTTON_H
