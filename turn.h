#ifndef TURN_H
#define TURN_H

#include "global.h"
#include "motor.h"

#define TURN_OFFSET 36

void Spin45(bool wait = true)
{
    moveRel(M_TURN, 30, 45 * ratio[M_TURN], wait);
    // moveRel(M_TURN, 60, -TURN_OFFSET, wait);
}

void Spin(int n, int offset = TURN_OFFSET, bool wait = true)
{
    if (n < 0)
    {
        offset = -offset;
    }
    moveRel(M_TURN, 50, n * (90 * ratio[M_TURN]) + offset, wait);

    if (offset != 0)
    {
        moveRel(M_TURN, 50, -offset, wait);
    }
}

void Show()
{
    moveRel(M_TURN, 30, 4 * (90 * ratio[M_TURN]));
}
#endif