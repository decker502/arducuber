#ifndef TURN_H
#define TURN_H

#include "global.h"
#include "motor.h"

// void Spin45()
// {

//     turnTablePosition -= 45 * ratio[M_TURN];
//     int32_t turnPos = turnTablePosition + turnTableOffset;

//     int32_t pos = getPosition(M_TURN);

// #ifdef DEBUG
//     Serial.print("Spin45 turnTablePosition:");
//     Serial.print(turnTablePosition);
//     Serial.print(" turnTableOffset:");
//     Serial.println(turnTableOffset);
//     Serial.print(" pos:");
//     Serial.println(pos);
//     Serial.print(" turnPos:");
//     Serial.println(turnPos);
//     Serial.print(" time:");
//     Serial.println(millis() - start);
//     Serial.println();
// #endif

//     if (pos < turnPos)
//     {
//         Serial.print(" time:");
//         Serial.println(millis() - start);

//         scanOK = false;
//         return;
//     }

//     while (pos >= turnPos)
//     {
//         pos = getPosition(M_TURN);
//     }
// }

void Spin45()
{
//     Serial.print("Spin45; turnTableOffset:");
//     Serial.print(turnTableOffset);
//     Serial.print(" turnTablePosition:");
//     Serial.print(turnTablePosition);

    turnTablePosition -= 45 * ratio[M_TURN];
    // moveAbs(M_TURN, 70, turnTablePosition + turnTableOffset);
    moveAbs(M_TURN, 70, turnTablePosition);

//     Serial.println(" ");
//     Serial.println(" ");
}

void Spin(int n)
{

    int32_t pos = 15 - getPosition(M_SCAN);
    if (pos < 0)
    {
        moveAbs(M_SCAN, 75, 15);
    };

    turnTablePosition -= n * (90 * ratio[M_TURN]);

    moveAbs(M_TURN, 100, turnTablePosition);
}

#endif