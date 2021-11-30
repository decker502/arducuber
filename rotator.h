#ifndef ROTATOR_H
#define ROTATOR_H

#include "global.h"
#include "tilt.h"
#include "turn.h"
#include "device.h"

class Rotator
{
public:
    Rotator()
    {
        for (int i = 0; i < NFACE; i++)
        {
            cubeFaceColor[i] = opposite[i];
        }
    }
    void rotate(int *faces, int *actions, int amount)
    {
        for (int i = 0; i < amount; i++)
        {
#ifdef __AVR__
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Solving...");
            lcd.setCursor(0, 1);
            lcd.print("Move of ");
            lcd.print(i + 1);
            lcd.print(" solve_n: ");
            lcd.print(amount);
#endif
            char face = faces[i];
            int action = actions[i];
            manipulate(face, action);
        }
    }

private:
    void handleTurn(bool colockwise = true)
    {
        if (colockwise)
        {
            int f = cubeFaceColor[0];
            cubeFaceColor[0] = cubeFaceColor[2];
            cubeFaceColor[2] = cubeFaceColor[1];
            cubeFaceColor[1] = cubeFaceColor[3];
            cubeFaceColor[3] = f;
        }
        else
        {
            int f = cubeFaceColor[0];
            cubeFaceColor[0] = cubeFaceColor[3];
            cubeFaceColor[3] = cubeFaceColor[1];
            cubeFaceColor[1] = cubeFaceColor[2];
            cubeFaceColor[2] = f;
        }
    }

    void handleTilt()
    {
        int f = cubeFaceColor[5];
        cubeFaceColor[5] = cubeFaceColor[3];
        cubeFaceColor[3] = cubeFaceColor[4];
        cubeFaceColor[4] = cubeFaceColor[2];
        cubeFaceColor[2] = f;
    }

    int cubePos(int face)
    {
        for (int i = 0; i < NFACE; i++)
        {
            if (cubeFaceColor[i] == face)
                return i;
        }
        return -1;
    }

    void manipulate(int face, int action)
    {

        int p = cubePos(face);
        switch (opposite[p])
        {
        case D:
            break;
        case F:
            TiltAway();
            handleTurn();
            Spin(-1);
            handleTilt();
            Tilt(1);
            break;
        case U:
            TiltAway();
            handleTilt();
            handleTilt();
            Tilt(2);
            break;
        case B:
            TiltAway();
            handleTurn(false);
            Spin(1);
            handleTilt();
            Tilt(1);
            break;
        case R:
            TiltAway();
            handleTurn();
            handleTurn();
            Spin(-2);
            handleTilt();
            Tilt(1);
            break;
        case L:
            TiltAway();
            handleTilt();
            Tilt(1);
            break;
        }

        TiltHold();
        // 旋转底面，顺逆时针的定义是从下向上看
        switch (action)
        {
        case 1:
            Spin(1);
            break;
        case 2:
            Spin(2);
            break;
        case 3:
            Spin(-1);
            break;
        }
    }

    int cubeFaceColor[NFACE];
};

#endif