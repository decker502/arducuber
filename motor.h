#ifndef MOTOR_H
#define MOTOR_H

#include "global.h"

BricktronicsMotor motors[] = {
    BricktronicsMotor(BricktronicsMegashield::MOTOR_1),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_2),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_3)};

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

// Go to the specified position using PID, but wait until the motor arrives
void waitForArrival(int m, int32_t position)
{

    while (!motors[m].settledAtPosition(position))
    {
        delay(1);
    }
}

void waitForArrival(int m)
{
    int factor = 0;
    while (factor < 10)
    {
        if (abs(positions[m] - motors[m].getPosition()) < 5)
        {
            factor++;
        }
        delay(1);
    }
}

bool busy(int m)
{
    return !motors[m].settledAtPosition(positions[m]);
}

// 绝对位移将忽略 power 参数的符号
void moveAbs(int m, int power, int16_t degree, bool wait = true)
{
#ifdef DEBUG
    Serial.print(F("moveAbs m:"));
    Serial.print(m);
    Serial.print(F(" power:"));
    Serial.print(power);
    Serial.print(F(" degree:"));
    Serial.print(degree);
#endif
    // BricktronicsMotor 旋转一圈为 720度， 转化为 BricktronicsMotor 的;
    // BricktronicsMotor 旋转旋转方向与EV3相反, 需要取反

    int32_t pos = int32_t(abs(degree)) * 2;
    //正向
    if ((power > 0 && degree > 0) || (power < 0 && degree < 0))
    {
        pos = -pos;
    }

    motors[m]._pid.SetOutputLimits(-abs(power) * MAX_M_POWER / 100, abs(power) * MAX_M_POWER / 100);
#ifdef DEBUG

    Serial.print(F(" before:"));
    Serial.print(positions[m]);
    Serial.print(F(" after:"));
    Serial.print(pos);
#endif

    positions[m] = pos;

    motors[m].goToPosition(positions[m]);
    if (wait)
    {
        waitForArrival(m);

    }
#ifdef DEBUG

    Serial.print(F(" real:"));
    Serial.println(motors[m].getPosition());
#endif
}

void moveRel(int m, int power, int16_t degree, bool wait = true)
{
#ifdef DEBUG
    Serial.print(F("moveRel m:"));
    Serial.print(m);
    Serial.print(F(" power:"));
    Serial.print(power);
    Serial.print(F(" degree:"));
    Serial.print(degree);
#endif

    // BricktronicsMotor 旋转一圈为 720度， 转化为 BricktronicsMotor 的;
    // BricktronicsMotor 旋转旋转方向与EV3相反, 需要取反

    int32_t pos = abs(degree) * 2;
    //正向
    if ((power > 0 && degree > 0) || (power < 0 && degree < 0))
    {
        pos = -pos;
    }

    motors[m]._pid.SetOutputLimits(-abs(power) * MAX_M_POWER / 100, abs(power) * MAX_M_POWER / 100);

#ifdef DEBUG
    Serial.print(F(" before:"));
    Serial.print(positions[m]);
    Serial.print(F(" :"));
    Serial.print(motors[m].getPosition());
#endif

    if (isOverwrite(positions[m], pos))
    {
        positions[m] = pos;
        motors[m].setPosition(0);
    }
    else
    {
        positions[m] += pos;
    }
#ifdef DEBUG
    Serial.print(F(" after:"));
    Serial.print(positions[m]);
#endif
    // motors[m].goToPositionWaitForArrivalOrTimeout(positions[m], 3000);
    motors[m].goToPosition(positions[m]);
    if (wait)
    {
        waitForArrival(m);
    }
#ifdef DEBUG
    Serial.print(F(" :"));
    Serial.println(motors[m].getPosition());
#endif
}

void move(int m, int power)
{
    power = -power;

    motors[m].setFixedDrive(power * MAX_M_POWER / 100);
}

void hold(int m)
{
    motors[m].hold();
}
void brake(int m)
{
    motors[m].brake();
}
void endstop(int m, int power, int wait = 200)
{
    // BricktronicsMotor 旋转旋转方向与EV3相反, 需要取反
    power = -power;

    int32_t p0, p1;
    motors[m].setFixedDrive(power * MAX_M_POWER / 100);

    p1 = motors[m].getPosition();

    while (true)
    {
        delay(wait);
        p0 = p1;
        p1 = motors[m].getPosition();

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
    positions[m] = motors[m].getPosition();

    Serial.println(" ");
}

void reset(int m)
{
    positions[m] = 0;
    motors[m].setPosition(0);
}

int32_t getPosition(int m)
{
    // BricktronicsMotor 旋转一圈为 720度， EV3 一圏为 360 度;
    // BricktronicsMotor 旋转旋转方向与EV3相反, 需要取反
    return -motors[m].getPosition() / 2;
}

int32_t getTargetPosition(int m)
{
    // BricktronicsMotor 旋转一圈为 720度， EV3 一圏为 360 度;
    // BricktronicsMotor 旋转旋转方向与EV3相反, 需要取反
    return -positions[m] / 2;
}

#endif