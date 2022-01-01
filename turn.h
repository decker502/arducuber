#ifndef TURN_H
#define TURN_H

#include "global.h"
#include "motor.h"

#define TURN_OFFSET 45
#define TURN_SPIN_POWER 80

void Spin45(bool wait = true)
{
    moveRel(M_TURN, 30, 45 * ratio[M_TURN], wait);
    // moveRel(M_TURN, 60, -TURN_OFFSET, wait);
}

void Spin(int rot)
{
    moveRel(M_TURN, TURN_SPIN_POWER, rot * (90 * ratio[M_TURN]));
}

// From MindCuber's TurnTurn function of spike version
void SpinFace(int rot, int rotn)
{
    //通过旋转偏移能提高没有弹簧自动回正功能魔方的回正率
    int extra = ratio[M_TURN] * TURN_OFFSET;
    int extran = ratio[M_TURN] * 0;

    if (rot < 0)
    {
        extra = -extra;
    }

    if (rotn < 0)
    {
        extra -= extran;
    }

    if (rotn > 0)
    {
        extra += extran;
    }

    moveRel(M_TURN, TURN_SPIN_POWER, rot * (90 * ratio[M_TURN]) + extra);

    if (extra != 0)
    {
        moveRel(M_TURN, TURN_SPIN_POWER, -extra);
    }
}

void Show()
{
    moveRel(M_TURN, 30, 4 * (90 * ratio[M_TURN]));
}
#endif