#ifndef DEVICE_H
#define DEVICE_H

#include <BricktronicsMegashield.h>
#include <BricktronicsMotor.h>
#include <BricktronicsButton.h>
#include <LiquidCrystal.h>
#include <HCSR04.h>
#include <Color.h>

Color color = Color();
const int rs = 49, en = 48, d4 = 47, d5 = 46, d6 = 45, d7 = 44;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
HCSR04 hc(25, 29);//initialisation class HCSR04 (trig pin , echo pin)

BricktronicsButton btns[] = {
  BricktronicsButton(BricktronicsMegashield::SENSOR_2),
  BricktronicsButton(BricktronicsMegashield::SENSOR_3),
  BricktronicsButton(BricktronicsMegashield::SENSOR_4)
};

// Select the desired motor port (MOTOR_1 through MOTOR_6) in the constructor below.
// BricktronicsMotor motor_base(BricktronicsMegashield::MOTOR_1);
// BricktronicsMotor motor_arm(BricktronicsMegashield::MOTOR_2);

BricktronicsMotor motors[] = {
    BricktronicsMotor(BricktronicsMegashield::MOTOR_1),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_2),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_3)};

#endif