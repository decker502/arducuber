#ifndef ROTATOR_H
#define ROTATOR_H

#include "global.h"
#include "tilt.h"
#include "turn.h"

class Rotator
{
public:
    Rotator()
    {
    }
    void rotate(int *faces, int *actions, int amount)
    {
        for (int i = 0; i < amount; i++)
        {
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
            int f = cubeFaceColor[3];
            cubeFaceColor[3] = cubeFaceColor[0];
            cubeFaceColor[0] = cubeFaceColor[2];
            cubeFaceColor[2] = cubeFaceColor[1];
            cubeFaceColor[1] = f;
        }
        else
        {
            int f = cubeFaceColor[3];
            cubeFaceColor[3] = cubeFaceColor[1];
            cubeFaceColor[1] = cubeFaceColor[2];
            cubeFaceColor[2] = cubeFaceColor[0];
            cubeFaceColor[0] = f;
        }
    }

    void handleTilt()
    {
        int f = cubeFaceColor[2];
        cubeFaceColor[2] = cubeFaceColor[5];
        cubeFaceColor[5] = cubeFaceColor[3];
        cubeFaceColor[3] = cubeFaceColor[4];
        cubeFaceColor[4] = f;
    }

    int distance(int face)
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
        int d = distance(face);
        switch (d)
        {
        case 2:
            break;
        case 1:
            handleTurn();
            Tilt(1);
            break;
        case 0:
            handleTilt();
            handleTilt();
            Tilt(2);
            break;
        case 3:
            handleTurn(false);
            Tilt(1);
            break;
        case 4:
            handleTurn();
            handleTurn();
            Tilt(1);
            break;
        case 5:
            handleTilt();
            Tilt(1);
            break;
        }

        TiltHold();
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

    int cubeFaceColor[NFACE] = {F, B, R, L, U, D};
};

#endif