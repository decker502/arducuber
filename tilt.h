#ifndef TILT_H
#define TILT_H

#include "global.h"
#include "motor.h"

void TiltAway()
{
    // if (20 < getPosition(M_TILT))
    // {
    //     moveAbs(M_TILT, 60, 20);
    // }

    moveAbs(M_TILT, 60, 10);
}

void TiltCal()
{
    // moveRel(M_TILT, 40, 10);
    endstop(M_TILT, -20);

    reset(M_TILT);

    brake(M_TILT);
    // TiltAway();
}

void TiltHold()
{
    // moveAbs(M_TILT, 50, 90);
    // moveAbs(M_TILT, 60, 90);
    moveAbs(M_TILT, 50, 115);
}

void Tilt(int n = 1, bool hold = true)
{
    // TiltHold();

    for (int i = 0; i < n; i++)
    {
        // 使用原参数时，底座横轴与主横梁平行时，底座会发生倾斜式跳动，导致魔方会翻转过头
        // 参见 readme 中 转盘的改造说明
        moveAbs(M_TILT, 60, 90);
        delay(100);
        // moveAbs(M_TILT, 60, 120);
        // delay(100);
        moveAbs(M_TILT, 80, 150);
        delay(100);
        moveAbs(M_TILT, 60, 90);
        delay(100);
        if (hold)
        {
            TiltHold();
        }
        // TiltHold();
        delay(200);
    }
}

#endif