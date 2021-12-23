#ifndef TILT_H
#define TILT_H

#include "global.h"
#include "motor.h"

void TiltAway()
{
    if (20 < getPosition(M_TILT))
    {
        moveAbs(M_TILT, 80, 20);
    }

    moveAbs(M_TILT, 70, 10);
}

void TiltCal()
{
    // moveRel(M_TILT, 40, 10);
    endstop(M_TILT, -20);

    reset(M_TILT);

    TiltAway();
}

void TiltHold()
{
    moveAbs(M_TILT, 60, 97);
}

void Tilt(int n = 1)
{
    TiltHold();

    for (int i = 0; i < n; i++)
    {
        // 使用原参数时，底座横轴与主横梁平行时，底座会发生倾斜式跳动，导致魔方会翻转过头
        // 参见 readme 中 转盘的改造说明
        moveAbs(M_TILT, 80, 160);
        TiltHold();
    }
}

#endif