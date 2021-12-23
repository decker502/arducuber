#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "global.h"
#include "device.h"
#include "banner.h"
#include "tilt.h"
#include "scan.h"
#include "color.h"
#include "mcmoves.h"
#include "fakedata.h"

void setup()
{
  // Be sure to set your serial console to 115200 baud
  Serial.begin(115200);

  lcd.init(); // initialize the lcd

  if (colorSensor.begin())
  {
    Serial.println("Found sensor");
  }
  else
  {
    Serial.println("No Color Sensor found ... check your connections");
    while (1)
      ; // halt!
  }

  btn.begin();

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

  bool scanOK = true;

  readWhiteRGB();
}

void init_cube(byte *cube)
{
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++)
      cube[o++] = f;
}

void readWhiteRGB()
{
  // Read the white calibration value from EEPROM
  byte id = EEPROM.read(EEPROM_ID_ADDR);
  if (id == EEPROM_ID)
  {
    for (int i = 0; i < 3; i++)
    {
      white_rgb[i] = EEPROM.read(EEPROM_DATA_ADDR + i + 1);
    }
  }
}

void writeWhiteRGB()
{
  // Read the white calibration value from EEPROM
  EEPROM.write(EEPROM_ID_ADDR, EEPROM_ID);

  for (int i = 0; i < 3; i++)
  {
    EEPROM.write(EEPROM_DATA_ADDR + i + 1, white_rgb[i]);
  }
}

bool CubeSense()
{
  float cm = sensorDist.get();

  return cm > 0 && cm < 16;
}

void CubeInsert()
{
  int count = 0;
  while (count < 150)
  {
    count++;
    if (!CubeSense())
    {
      count = 0;
    }
    if (btn.isPressed(BTN_LEFT))
    {
      moveRel(M_TURN, 75, 2 * ratio[M_TURN], true);
    }
    if (btn.isPressed(BTN_RIGHT))
    {
      moveRel(M_TURN, 75, -2 * ratio[M_TURN], true);
    }
    delay(10);
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
    delay(10);
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

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Processing...");
    Serial.println("Processing...");

    int t = -1;
    // spike 版本增加到12
    for (int i = 0; i < 12; i++)
    {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Determine Clr...");

      Serial.println("determine_colors...");
      cubeColors.determine_colors(cube, i);

      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Check valid ...");

      Serial.println("valid_pieces...");
      bool is_valid = validator.valid_pieces(cube);

      if (is_valid)
      {
        t = i;
        pieces_valid++;
        Serial.println("is_valid");
        cubeColors.print();

        // display_cube(cube);
        if (cubeSolver.solve(cube))
        {
          solved = true;
          break;
        }
      }
      else
      {
        Serial.print(F(" not valid: "));
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

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Solving ...");

    Serial.println(" Solving..");

    cubeSolver.uc = CUBE_LEFT;
    cubeSolver.fc = CUBE_DOWN;

    for (int i = 0; i < cubeSolver.solve_n; i++)
    {
      int fi = cubeSolver.solve_fce[i];
      int ri = cubeSolver.solve_rot[i];
      int fo = opposite[fi];
      int rn = 0;
      for (int j = i + 1; rn == 0 && j < cubeSolver.solve_n; j++)
      {
        if (cubeSolver.solve_fce[j] != fo)
          rn = cubeSolver.solve_rot[j];
      }

      lcd.setCursor(0, 1);
      lcd.print("Move ");
      lcd.print(i + 1);
      lcd.print(" of ");
      lcd.print(cubeSolver.solve_n);

      cubeSolver.manipulate(cube, fi, ri, rn);
    }
  }
  else
  {

    Serial.print(F("Scan error..."));
    delay(500);

    cubeColors.print();
    if (pieces_valid > 1)
    {
      Serial.print(F("Cube "));
      Serial.println(pieces_valid);
      delay(1000);
    }
    else
    {
      Serial.println(pieces_valid);
    }
  }

  ScanAway();
  TiltAway();

  return solved;
}

void calibrate_white()
{
  moveAbs(M_SCAN, 100, T_SCNT, false);
  waitForArrival(M_SCAN);
  delay(100);

  float red, green, blue;

  while (true)
  {
    delay((256 - TCS34725_INTEGRATIONTIME_101MS) * 12 / 5 + 1);
    colorSensor.getRGB(&red, &green, &blue);
    white_rgb[0] = uint8_t(red);
    white_rgb[1] = uint8_t(green);
    white_rgb[2] = uint8_t(blue);

    if (red > 250 || green > 250 || blue > 250)
      break;
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Cal White suc");
#ifdef DEBUG
  Serial.print(F("Calibrate white successful R: "));
  Serial.print(white_rgb[0]);
  Serial.print(F(" G: "));
  Serial.print(white_rgb[1]);
  Serial.print(F(" B: "));
  Serial.println(white_rgb[2]);
#endif
  writeWhiteRGB();
}

void loop()
{
  while (!btn.isPressed(BTN_CONFIRM))
  {
    delay(10);
  }

  byte cube[NFACE * 8];
  initialize();

  banner();

  init_cube(cube);

  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Reset scan...");

  ScanCal();
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Reset tilt...");

  TiltCal();
  delay(500);

  bool cal_white = false;

  while (true)
  {
    ScanAway();
    TiltAway();
    delay(500);

    if (!cal_white && btn.isPressed(BTN_UP))
    {
      cal_white = true;

      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Cal White...");

      Serial.println("Calibrate white");

      calibrate_white();
      continue;
    }

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Insert cube...");

    Serial.println("CubeInsert:");
    CubeInsert();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning...");

    Solve(cube);

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Remove cube...");

    Serial.println("CubeRemove:");
    CubeRemove();
  }
}
