#ifndef DEVICE_H
#define DEVICE_H

#include <BricktronicsMegashield.h>
#include <BricktronicsMotor.h>
#include <BricktronicsButton.h>
#include <LiquidCrystal.h>
// #include <yahboom.h>
#include <gy33.h>

#include "led.h"
#include "color.h"
#include "dist.h"
#include "solver.h"
#include "validator.h"
#include "solvercompo.h"
#include "rotator.h"

Led led = Led();
// Yahboom colorSensor = Yahboom();
GY33 colorSensor = GY33();

CubeColors cubeColors = CubeColors();
Validator validator = Validator();
CubeSolver cubeSolver = CubeSolver();
CubeSolverCompo solverCompo = CubeSolverCompo();
SensorDist sensorDist = SensorDist();
Rotator rotator = Rotator();

const int rs = 49, en = 48, d4 = 47, d5 = 46, d6 = 45, d7 = 44;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Select the desired motor port (MOTOR_1 through MOTOR_6) in the constructor below.
// BricktronicsMotor motor_base(BricktronicsMegashield::MOTOR_1);
// BricktronicsMotor motor_arm(BricktronicsMegashield::MOTOR_2);



#endif