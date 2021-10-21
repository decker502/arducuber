#ifndef BANNER_H
#define BANNER_H

#include "global.h"
#include "device.h"

void banner()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MindCuber v2.2");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
}

#endif
