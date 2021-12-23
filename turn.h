#ifndef TURN_H
#define TURN_H

#include "global.h"
#include "motor.h"


void Spin45()
{
    moveRel(M_TURN, 70, -45 * ratio[M_TURN]);
}

void Spin(int n)
{

    int32_t pos = 15 - getPosition(M_SCAN);
    if (pos < 0)
    {
        moveAbs(M_SCAN, 75, 15);
    };

    moveRel(M_TURN, 100, -n * (90 * ratio[M_TURN]));
}

#endif