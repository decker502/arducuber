#ifndef BUTTON_H
#define BUTTON_H

#include <BricktronicsButton.h>
#include <BricktronicsMegashield.h>

#include "global.h"


//-----------------------------------------------------------------------------
// Routines to detect button presses with auto-repeat when held
//-----------------------------------------------------------------------------
#define BTNLEFT 0
#define BTNCENTER 1
#define BTNRIGHT 2


#define BTN_AUTO_REPEAT_FIRST 1000
#define BTN_AUTO_REPEAT 30

BricktronicsButton btns[] = {
  BricktronicsButton(BricktronicsMegashield::SENSOR_2),
  BricktronicsButton(BricktronicsMegashield::SENSOR_3),
  BricktronicsButton(BricktronicsMegashield::SENSOR_4)
};

long btn_time = 0;
bool btn_left = false;
bool btn_right = false;
bool btn_center = false;

bool btn_auto_repeat(byte btn, bool &btn_pressed)
{
  if (btns[btn].isPressed()) {
    if (btn_pressed) {
      long diff = millis()-btn_time;
      if (diff >= 0) {
        btn_time += BTN_AUTO_REPEAT;
        return true;
      }
    } else {
      btn_pressed = true;
      btn_time = millis()+BTN_AUTO_REPEAT_FIRST;
      return true;
    }
  } else
    btn_pressed = false;
  return false;
}

bool leftPressed()
{
  return btn_auto_repeat(BTNLEFT, btn_left);
}

bool rightPressed()
{
  return btn_auto_repeat(BTNRIGHT, btn_right);
}

bool centerPressed()
{
  return btn_auto_repeat(BTNCENTER, btn_center);
}

#endif