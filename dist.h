#ifndef SENSOR_DIST_H
#define SENSOR_DIST_H

#include <HCSR04.h>

#include "global.h"

class SensorDist
{
public:
  SensorDist(int trigPin = 25, int echoPin = 29 ):
  hc(trigPin, echoPin)
  {
  }

  void begin()
  {
  }

  float get()
  {
    float cm =  hc.dist();
    delay(5);
    return cm;
  }


private:
  HCSR04 hc;
};

#endif