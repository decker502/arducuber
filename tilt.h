#ifndef TILT_H
#define TILT_H

#include "global.h"
#include "motor.h"

void TiltAway()
{
    int32_t cur = motors[M_TILT].getPosition();
    int32_t pos = (20 - cur);
    if (pos < 0)
    {
        moveAbs(M_TILT, 70, pos);
    }

    cur = motors[M_TILT].getPosition();
    moveAbs(M_TILT, 20, 10 - cur);
}

void TiltCal()
{
    moveAbs(M_TILT, 40, 10);
    endstop(M_TILT, -20);
    TiltAway();
}
#endif