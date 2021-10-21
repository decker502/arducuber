#ifndef MOTOR_H
#define MOTOR_H

#include "global.h"
#include "device.h"

bool isOverwrite(int32_t a, int32_t b)
{
    if (b > 0)
    {
        if (a > INT32_MAX - b)
        {
            return true;
        }
    }
    if (b < 0)
    {
        if (a < INT32_MIN - b)
        {
            return true;
        }
    }
    return false;
}

void moveAbs(int m, int power, int16_t degree, bool hold = true)
{

    // BricktronicsMotor 旋转一圈为 720度， 转化为 BricktronicsMotor 的;
    int32_t pos = abs(degree * 2);
    // BricktronicsMotor 旋转方向定义与Ev3 相反
    power = -power;
    // 为保持和ev3相同的定义 ，忽略 degree的符号，方向由power控制，
    if (power < 0)
        pos = -pos;

    motors[m]._pid.SetOutputLimits(-abs(power) * MAX_M_POWER / 100, abs(power) * MAX_M_POWER / 100);

    //实际移动距离
    uint32_t moveCount = 0;
    if ((positions[m] >= 0 && pos >= 0) || (positions[m] <= 0 && pos <= 0))
    {
        moveCount = abs(pos * multipliers[m] - positions[m]);
    }
    if ((positions[m] >= 0 && pos <= 0) || (positions[m] <= 0 && pos >= 0))
    {
        moveCount = abs(pos * multipliers[m] + positions[m]);
    }
    Serial.print("m:");
    Serial.print(m);
    Serial.print(" positions[m]:");
    Serial.print(positions[m]);
    Serial.print(" pos:");
    Serial.print(pos);

    positions[m] = pos * multipliers[m];
    motors[m].goToPosition(positions[m]);

    delay(moveCount * M_WAIT_MUL);

    Serial.print(" curpos:");
    Serial.print(motors[m].getPosition());
    Serial.print(" pos:");
    Serial.print(pos);
    Serial.print(" ms:");
    Serial.println(moveCount * M_WAIT_MUL);
    // motors[m].delayUpdateMS(200);

    // delay(abs(real) * 1000 / 180);
    if (hold)
        motors[m].hold();
}

void moveRel(int m, int16_t pos, int16_t off = 0)
{
    if ((pos + off) == 0)
        return;

    uint32_t moveCount = abs(pos + off) * multipliers[m];

    int32_t real = (pos + off) * multipliers[m];
    if (isOverwrite(positions[m], real))
    {
        positions[m] = real;
        motors[m].setPosition(0);
    }
    else
    {
        positions[m] += real;
    }

    motors[m].goToPosition(positions[m]);

    // motors[m].delayUpdateMS(200);
    delay(moveCount * M_WAIT_MUL);
    Serial.print("move_rel m:");
    Serial.print(m);
    Serial.print(" curpos:");
    Serial.print(motors[m].getPosition());
    Serial.print(" positions[m]:");
    Serial.print(positions[m]);
    Serial.print(" pos:");
    Serial.print(pos);
    Serial.print(" off:");
    Serial.print(off);
    Serial.print(" real:");
    Serial.print(real);
    Serial.print(" ms:");
    Serial.println(moveCount * M_WAIT_MUL);
    if (off != 0)
    {
        uint32_t moveCount = abs(off) * multipliers[m];
        real = -off * multipliers[m];
        if (isOverwrite(positions[m], real))
        {
            positions[m] = real;
            motors[m].setPosition(0);
        }
        else
        {
            positions[m] += real;
        }
        motors[m].goToPosition(positions[m]);

        delay(moveCount * M_WAIT_MUL);
        // delay(real * 5);
    }
    // motors[m].hold();
}

void endstop(int m, int power)
{
    motors[m]._pid.SetOutputLimits(-power * MAX_M_POWER / 100, power * MAX_M_POWER / 100);

    int32_t p0, p1;
    // BricktronicsMotor 和 ev3的旋转方向是相反的
    motors[m].setFixedDrive(-power * MAX_M_POWER / 100);

    p1 = motors[m].getPosition();

    while (true)
    {
        delay(200);
        p0 = p1;
        p1 = motors[m].getPosition();
        Serial.print("endstop; m:");
        Serial.print(m);
        Serial.print(" p0:");
        Serial.print(p0);
        Serial.print(" p1:");
        Serial.println(p1);
        if (power < 0)
        {
            if (p0 <= p1)
            {
                break;
            }
        }
        else
        {
            if (p0 >= p1)
            {
                break;
            }
        }
    };

    positions[m] = 0;
    motors[m].setPosition(0);
}
#endif