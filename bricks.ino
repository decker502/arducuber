#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "device.h"
#include "global.h"
#include "banner.h"
#include "tilt.h"
#include "mcmoves.h"
#include "fakedata.h"


void setup()
{
  // Be sure to set your serial console to 115200 baud
  Serial.begin(115200);

  if (color.begin())
  {
    Serial.println("Found color sensor");
  }
  else
  {
    Serial.println("No GY-33 found ... check your connections");
    while (1)
      ;
  }
  lcd.begin(16, 2);

  // motor_base.setEpsilon(10);
  // motor_base._pid.SetOutputLimits(-230, 230);
  motors[M_TURN].begin();
  motors[M_TILT].begin();
  motors[M_SCAN].begin();

  btns[BTNCENTER].begin();
  btns[BTNLEFT].begin();
  btns[BTNRIGHT].begin();

  initialize();

  OCR0A = 0x7F;
  TIMSK0 |= _BV(OCIE0A);
}

// This function will be called every millisecond.
// It just calls update() for each motor.
ISR(TIMER0_COMPA_vect)
{
  static unsigned char count_ms = 0;
  if (++count_ms == 50)
  {
    motors[M_TURN].update();
    motors[M_TILT].update();
    motors[M_SCAN].update();
    count_ms = 0;
  }
}

void initialize()
{
  color.setLevel(8);
  lcd.scrollDisplayLeft();
  lcd.clear();

  // Initialize the motor connections
  motors[M_TURN].setPosition(0);
  motors[M_TILT].setPosition(0);
  motors[M_SCAN].setPosition(0);

  motors[M_TURN]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);
  motors[M_TILT]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);
  motors[M_SCAN]._pid.SetOutputLimits(-MAX_M_POWER, MAX_M_POWER);

  readWhiteRGB();
}




void readWhiteRGB()
{
  // Read the white calibration value from EEPROM
  byte id = EEPROM.read(EEPROM_ID_ADDR);
  if (id == EEPROM_ID)
  {
    for (int i = 0; i < 3; i++)
    {
      byte highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4));
      byte lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 1);
      int16_t highWord = word(highByte, lowByte);

      highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 2);
      lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 3);
      int16_t lowWord = word(highByte, lowByte);

      white_rgb[i] = makeLong(highWord, lowWord);
    }
  }
}

void writeWhiteRGB()
{
  // Read the white calibration value from EEPROM
  EEPROM.write(EEPROM_ID_ADDR, EEPROM_ID);

  for (int i = 0; i < 3; i++)
  {
    int16_t hw = highWord(white_rgb[i]);
    int16_t lw = lowWord(white_rgb[i]);
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4), highByte(hw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 1, lowByte(hw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 2, highByte(lw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 3, lowByte(lw));
  }
}

void init_cube(byte *cube)
{
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++)
      cube[o++] = f;
}

void loop()
{
  byte cube[NFACE * 8];
  initialize();

  banner();

  init_cube(cube);

  TiltCal();
  // motors[M_TURN].setFixedDrive(-255);
  
}
