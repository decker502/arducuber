#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "global.h"
#include "device.h"
#include "banner.h"
#include "tilt.h"
#include "scan.h"
#include "btn.h"
#include "led.h"
#include "color.h"
#include "fakedata.h"

#include "validator.h"
#include "solvercompo.h"
#include "rotator.h"

Validator validator = Validator();
Rotator rotator = Rotator();
CubeSolverCompo solverCompo = CubeSolverCompo();

void setup()
{
  // Be sure to set your serial console to 115200 baud
  Serial.begin(115200);

  // if (colorSensor.begin())
  // {
  //   Serial.println("Found color sensor");
  // }
  // else
  // {
  //   Serial.println("No GY-33 found ... check your connections");
  //   while (1)
  //     ;
  // }
  // lcd.begin(16, 2);
  // led.begin();

  btns[BTNCENTER].begin();
  btns[BTNLEFT].begin();
  btns[BTNRIGHT].begin();

  // Initialize the motor connections
  motors[M_TURN].begin();
  motors[M_TILT].begin();
  motors[M_SCAN].begin();

  motors[M_SCAN].pidSetTunings(2.64, 23.6, 0.1207317073);

  initialize();

  OCR0A = 0x7F;
  TIMSK0 |= _BV(OCIE0A);
}

// This function will be called every millisecond.
// It just calls update() for each motor.
ISR(TIMER0_COMPA_vect)
{
  static unsigned char count_ms = 0;
  static int stableFactor[3] = {0, 0, 0};

  if (++count_ms == 50)
  {
    // motors[M_TURN].update();
    if (abs(positions[M_TURN] - motors[M_TURN].getPosition()) < 5)
    {
      stableFactor[M_TURN]++;
      if (stableFactor[M_TURN] > 5)
        stableFactor[M_TURN] = 5;
    }
    else
    {
      stableFactor[M_TURN] = 0;
    }

    if (stableFactor[M_TURN] < 5)
    {
      motors[M_TURN].update();
    }

    if (abs(positions[M_TILT] - motors[M_TILT].getPosition()) < 5)
    {
      stableFactor[M_TILT]++;
      if (stableFactor[M_TILT] > 5)
        stableFactor[M_TILT] = 5;
    }
    else
    {
      stableFactor[M_TILT] = 0;
    }

    if (stableFactor[M_TILT] < 5)
    {
      motors[M_TILT].update();
    }

    if (abs(positions[M_SCAN] - motors[M_SCAN].getPosition()) < 5)
    {
      stableFactor[M_SCAN]++;
      if (stableFactor[M_SCAN] > 5)
        stableFactor[M_SCAN] = 5;
    }
    else
    {
      stableFactor[M_SCAN] = 0;
    }

    if (stableFactor[M_SCAN] < 5)
    {
      motors[M_SCAN].update();
    }

    count_ms = 0;
  }
}

void initialize()
{
  // color.setLevel(8);
  // lcd.scrollDisplayLeft();
  // lcd.clear();
  // led.off();

  motors[M_TURN].coast();
  motors[M_TILT].coast();
  motors[M_SCAN].coast();

  motors[M_TURN].setPosition(0);
  positions[M_TURN] = 0;
  motors[M_TILT].setPosition(0);
  positions[M_TILT] = 0;
  motors[M_SCAN].setPosition(0);
  positions[M_SCAN] = 0;

  motors[M_TURN]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);
  motors[M_TILT]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);
  motors[M_SCAN]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);

  turnTablePosition = 0;
  turnTableOffset = 24;

  bool scanOK = true;
}

void init_cube(byte *cube)
{
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++)
      cube[o++] = f;
}

bool CubeSense()
{
  float cm = sensorDist.get();

  // #ifdef DEBUG
  //   Serial.print(" CubeSense dist:");
  //   Serial.println(cm);

  // #endif

  return cm > 0 && cm < 16;
}

void CubeInsert()
{
  int count = 0;
  while (count < 150)
  {
    // #ifdef DEBUG
    //     Serial.print(" CubeInsert count:");
    //     Serial.println(count);
    // #endif
    count++;
    if (!CubeSense())
      count = 0;
    if (leftPressed())
    {
      moveRel(M_TURN, 75, 2 * ratio[M_TURN], true);
      turnTablePosition += 2 * ratio[M_TURN];
    }
    if (rightPressed())
    {
      moveRel(M_TURN, 75, -2 * ratio[M_TURN], true);
      turnTablePosition -= 2 * ratio[M_TURN];
    }
  }
}

void CubeRemove()
{
  int count = 0;
  while (count < 150)
  {
    count++;
    if (CubeSense())
      count = 0;
  }
}

bool Solve(byte *cube)
{
  unsigned long start_time = millis();
  bool solved = false;
  int move = 0;
  int pieces_valid = 0;
  for (int tries = 0; !solved && tries < 3; tries++)
  {
    start_time = millis();
    solved = false;
    move = 0;

    ScanCube();
    // testColors();

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Processing...");
    Serial.println("Processing...");

    int t = -1;
    // spike 版本增加到12
    //  for (int i = 0; i < 6; i++)
    for (int i = 0; i < 12; i++)
    {
      Serial.println("determine_colors...");
      cubeColors.determine_colors(cube, i);

      Serial.println("valid_pieces...");
      bool is_valid = validator.valid_pieces(cube);

      // bool is_valid = valid_pieces(cube);

      if (is_valid)
      {
        t = i;
        pieces_valid++;
        Serial.println("is_valid");
        cubeColors.print();

        solverCompo.calcInput(cube);

        Serial.print("Input:");
        Serial.println(solverCompo.solverInput);

        solverCompo.solve();

        Serial.print(" result: ");
        for (int i = 0; i < solverCompo.count; i++)
        {
          Serial.print(solverCompo.soulutions[i]);
          Serial.print(":");
          Serial.print(solverCompo.actions[i]);
          Serial.print(" ");
        }
        Serial.println();
        solved = true;

        break;
      }
      else
      {
        Serial.print(" not valid: ");
        Serial.println(i);
        cubeColors.print();
      }
    }

    if (!solved)
    {
      cubeColors.print();
    }
  }
  if (solved)
  {
    ScanAway();
    Serial.println(" Solving..");

    rotator.rotate(solverCompo.soulutions, solverCompo.actions, solverCompo.count);
  }
  else
  {

    Serial.print("Scan error...");
    delay(500);

    cubeColors.print();
    if (pieces_valid > 1)
    {
      Serial.print("Cube ");
      Serial.println(pieces_valid);
      delay(1000);
      // flash_blue();
    }
    else
    {
      Serial.println(pieces_valid);
      // flash_red();
    }
  }

  ScanAway();
  TiltAway();

  return solved;
}

void testSolve(byte *cube)
{
  Color colors[NFACE * 9];

  for (int i = 0; i < NFACE; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      cubeColors.setRGB(i, j, fake_colors[5][i * 9 + j]);
    }
  }
  ScanAway();

  for (int i = 0; i < 12; i++)
  {
    cubeColors.determine_colors(cube, i);
    Serial.print("valid_pieces...");
    bool is_valid = validator.valid_pieces(cube);
    if (is_valid)
    {
      solverCompo.calcInput(cube);
      Serial.print(solverCompo.solverInput);
      Serial.println();
      solverCompo.solve();
      Serial.print("result:");
      Serial.print(" solverCompo.count:");
      Serial.print(solverCompo.count);
      for (int i = 0; i < solverCompo.count; i++)
      {
        // Serial.print("FBRLUD"[solverCompo.soulutions[i]] << solverCompo.actions[i];
        Serial.print(solverCompo.soulutions[i]);
        Serial.print(" :");
        Serial.print(solverCompo.actions[i]);
        Serial.print(" ");
      }
      Serial.println();
      solved = true;

      break;
    }
    else
    {
      Serial.println("not valid");
    }
  }

  if (solved)
  {
    ScanAway();
    Serial.println(" Solving..");

    rotator.rotate(solverCompo.soulutions, solverCompo.actions, solverCompo.count);
  }
}
void loop()
{
  while (!leftPressed())
    ;

  led.on(0, 0, 40);
  byte cube[NFACE * 8];
  initialize();

  banner();

  init_cube(cube);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Reset scan...");

  ScanCal();

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Reset tilt...");

  TiltCal();

  // testSolve(cube);

  while (true)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Remove cube...");
    Serial.println("CubeInsert:");

    CubeInsert();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning...");

    Solve(cube);
    Serial.println("CubeRemove:");
    CubeRemove();
  }
}
