

#include "global.h"
#include "device.h"

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