#ifndef TILT_H
#define TILT_H

#include "global.h"
#include "motor.h"

void TiltAway()
{
    if (20 < getPosition(M_TILT))
    {
        moveAbs(M_TILT, 70, 20);
    }
    Serial.println("TiltAway2");

    moveAbs(M_TILT, 20, 10);
    Serial.println("TiltAway3");
}

void TiltCal()
{
    moveRel(M_TILT, 40, 10);
    endstop(M_TILT, -20);
    Serial.print("reset:");

    reset(M_TILT);

    Serial.println("TiltAway");

    TiltAway();
}

void TiltHold()
{
    moveAbs(M_TILT, 60, tiltOffset + 85);
}

void Tilt(int n = 1)
{
    TiltHold();

    for (int i = 0; i < n; i++)
    {
        // 使用原参数时，底座横轴与主横梁平行时，底座会发生倾斜式跳动，导致魔方会翻转过头
        // 参见 readme 中 转盘的改造说明
        // moveAbs(M_TILT, 100, tiltOffset + 195);
        moveAbs(M_TILT, 80, tiltOffset + 150);
        // moveRel(M_TILT, 75, -15);
        // delay(5);
        // moveAbs(M_TILT, 75, tiltOffset + 60);
        TiltHold();
        // moveAbs(M_TILT, 60, tiltOffset + 85);
        // delay(5);
        // TiltHold();
    }
}

#endif