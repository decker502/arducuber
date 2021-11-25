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
#include "mcmoves.h"
#include "fakedata.h"

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

bool stable(int m)
{
}
// This function will be called every millisecond.
// It just calls update() for each motor.
ISR(TIMER0_COMPA_vect)
{
  static unsigned char count_ms = 0;
  static int stableFactor = 0;

  if (++count_ms == 50)
  {
    motors[M_TURN].update();
    // if (!motors[M_TURN].settledAtPosition(positions[M_TURN]))
    //   motors[M_TURN].update();
    // else
    // {
    //   motors[M_TURN].brake();
    // }
    motors[M_TILT].update();
    // if (!motors[M_TILT].settledAtPosition(positions[M_TILT]))
    //   motors[M_TILT].update();
    // else
    // {
    //   motors[M_TILT].brake();
    // }

    if (abs(positions[M_SCAN] - motors[M_SCAN].getPosition()) < 5)
    {
      stableFactor++;
      if (stableFactor > 5)
        stableFactor = 5;
    }
    else
    {
      stableFactor = 0;
    }

    if (stableFactor < 5)
    {
      motors[M_SCAN].update();
    }

    // if (motors[M_SCAN].settledAtPosition(positions[M_SCAN]))
    //   motors[M_SCAN].brake();
    // if (!motors[M_SCAN].settledAtPosition(positions[M_SCAN]))
    //   motors[M_SCAN].update();
    // else
    // {
    // }

    // motors[M_TURN].update();
    // motors[M_TILT].update();
    // motors[M_SCAN].update();

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

  readWhiteRGB();
}

void readWhiteRGB()
{
  // Read the white calibration value from EEPROM
  // byte id = EEPROM.read(EEPROM_ID_ADDR);
  // if (id == EEPROM_ID)
  // {
  //   for (int i = 0; i < 3; i++)
  //   {
  //     byte highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4));
  //     byte lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 1);
  //     int16_t highWord = word(highByte, lowByte);

  //     highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 2);
  //     lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 3);
  //     int16_t lowWord = word(highByte, lowByte);

  //     white_rgb[i] = makeLong(highWord, lowWord);
  //   }
  // }
}

void writeWhiteRGB()
{
  // Read the white calibration value from EEPROM
  // EEPROM.write(EEPROM_ID_ADDR, EEPROM_ID);

  // for (int i = 0; i < 3; i++)
  // {
  //   int16_t hw = highWord(white_rgb[i]);
  //   int16_t lw = lowWord(white_rgb[i]);
  //   EEPROM.write(EEPROM_DATA_ADDR + (i * 4), highByte(hw));
  //   EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 1, lowByte(hw));
  //   EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 2, highByte(lw));
  //   EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 3, lowByte(lw));
  // }
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

int find_edge(byte *cube, byte f0, byte f1)
{
  byte e0;
  FIND_EDGE(U, 1, B, 5, 0, 1);
  FIND_EDGE(U, 7, L, 1, 2, 3);
  FIND_EDGE(U, 5, F, 1, 4, 5);
  FIND_EDGE(U, 3, R, 1, 6, 7);
  FIND_EDGE(L, 3, F, 7, 8, 9);
  FIND_EDGE(B, 7, L, 7, 10, 11);
  FIND_EDGE(D, 7, L, 5, 12, 13);
  FIND_EDGE(R, 3, B, 3, 14, 15);
  FIND_EDGE(D, 5, B, 1, 16, 17);
  FIND_EDGE(F, 3, R, 7, 18, 19);
  FIND_EDGE(D, 3, R, 5, 20, 21);
  FIND_EDGE(D, 1, F, 5, 22, 23);
  return -1;
}
int find_corner(byte *cube, byte f0, byte f1, byte f2)
{
  byte c0;
  FIND_CORNER(U, 2, B, 4, R, 2, 0, 1, 2);
  FIND_CORNER(U, 0, L, 0, B, 6, 3, 4, 5);
  FIND_CORNER(U, 6, F, 0, L, 2, 6, 7, 8);
  FIND_CORNER(U, 4, R, 0, F, 2, 9, 10, 11);
  FIND_CORNER(D, 0, L, 4, F, 6, 12, 13, 14);
  FIND_CORNER(D, 6, B, 0, L, 6, 15, 16, 17);
  FIND_CORNER(D, 4, R, 4, B, 2, 18, 19, 20);
  FIND_CORNER(D, 2, F, 4, R, 6, 21, 22, 23);
  return -1;
}

bool valid_pieces(byte *cube)
{
  return (find_edge(cube, U, F) >= 0) &&
         (find_edge(cube, U, L) >= 0) &&
         (find_edge(cube, U, B) >= 0) &&
         (find_edge(cube, U, R) >= 0) &&
         (find_edge(cube, F, L) >= 0) &&
         (find_edge(cube, L, B) >= 0) &&
         (find_edge(cube, B, R) >= 0) &&
         (find_edge(cube, R, F) >= 0) &&
         (find_edge(cube, D, F) >= 0) &&
         (find_edge(cube, D, L) >= 0) &&
         (find_edge(cube, D, B) >= 0) &&
         (find_edge(cube, D, R) >= 0) &&
         (find_corner(cube, U, F, L) >= 0) &&
         (find_corner(cube, U, L, B) >= 0) &&
         (find_corner(cube, U, B, R) >= 0) &&
         (find_corner(cube, U, R, F) >= 0) &&
         (find_corner(cube, D, F, R) >= 0) &&
         (find_corner(cube, D, R, B) >= 0) &&
         (find_corner(cube, D, B, L) >= 0) &&
         (find_corner(cube, D, L, F) >= 0);
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

      solverCompo.calcInput(cube);

      Serial.print("Input:");
      Serial.println(solverCompo.solverInput);

      Serial.println("valid_pieces...");
      bool is_valid = cubeSolver.valid_pieces(cube);
      // bool is_valid = valid_pieces(cube);

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

    cubeSolver.uc = L;
    cubeSolver.fc = D;

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

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Solving...");
      lcd.setCursor(0, 1);
      lcd.print("Move of ");
      lcd.print(i + 1);
      lcd.print(" solve_n: ");
      lcd.print(cubeSolver.solve_n);

      cubeSolver.manipulate(cube, fi, ri, rn);
    }
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

void testColors()
{
  for (int i = 0; i < NFACE; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      cubeColors.setRGB(i, j, fake_rgb[cubeColors.pos(i, j)]);
    }
  }
}

void printRGB()
{
  Serial.println();
  Serial.print("setRGB; red:");
  Serial.print(rgb[0]);
  Serial.print(" green:");
  Serial.print(rgb[1]);
  Serial.print(" blue:");
  Serial.print(rgb[2]);
  Serial.print(" HEX: # ");
  Serial.print(rgb[0], HEX);
  Serial.print(rgb[1], HEX);
  Serial.print(rgb[2], HEX);
  Serial.println();
}
void testScan()
{
  uint8_t c = 0;

  moveAbs(M_SCAN, 100, -720); // middle
  delay(500);
  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(5000);

  moveAbs(M_TURN, 100, -112);
  moveAbs(M_SCAN, 80, -553); // corner
  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -249);
  moveAbs(M_SCAN, 80, -609); // edge

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -386);
  moveAbs(M_SCAN, 80, -562); // corner

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -523);
  moveAbs(M_SCAN, 80, -609); // edge

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -660);
  moveAbs(M_SCAN, 80, -566); // corner

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -797);
  moveAbs(M_SCAN, 80, -604); // edge

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -933);
  moveAbs(M_SCAN, 80, -562); // corner

  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);

  moveAbs(M_TURN, 100, -1069);
  moveAbs(M_SCAN, 80, -608); // edge
  colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
  printRGB();
  delay(2000);
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

  // lcd.clear();
  // lcd.setCursor(0, 1);
  // lcd.print("Reset RGB...");

  // ScanMiddle(0);
  // ScanCorner(0, 2);
  // ScanEdge(0, 2);

  // ScanCube();
  // ScanFace(3, 2);
  // cubeColors.print();
  // ScanFace(3, 2);

  //   ScanAway();

  // cubeColors.print();

  // Spin(1);
  // delay(1000);
  // Spin(-1);

  // int start = millis();
  //         moveRel(M_TURN, -38, ratio[M_TURN] * 360);
  //     Serial.print("time : ");
  //     Serial.println(millis() - start);

  // Tilt(2);
  // TiltAway();
  // delay(2000);
  // Solve(cube);
  // CalScanRGB();
  // testScan();
  // ScanMiddle(0);
  // while (!rightPressed())
  // {
  //   delay(2000);
  //   ScanRGB(0, 0, rgb);
  //   Serial.println();
  //   Serial.print("setRGB; red:");
  //   Serial.print(rgb[0]);
  //   Serial.print(" green:");
  //   Serial.print(rgb[1]);
  //   Serial.print(" blue:");
  //   Serial.print(rgb[2]);
  //   Serial.print(" HEX: #");
  //   Serial.print(rgb[0], HEX);
  //   Serial.print(rgb[1], HEX);
  //   Serial.print(rgb[2], HEX);
  //   Serial.println();
  // }

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
  //   motors[M_SCAN].goToPosition(180);
  // while (!motors[M_SCAN].settledAtPosition(180))
  //     ;
  // moveAbs(M_SCAN, 100, -180);

  // motors[M_TURN].setFixedDrive(100);
  // motors[M_SCAN].goToPositionWaitForArrival(680);
  // moveAbs(M_TILT, 100, 90);
  // motors[M_TURN].goToPositionWaitForArrival(270);
  //     Serial.print(" real:");
  //   Serial.println(motors[M_TURN].getPosition());
}
