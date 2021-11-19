#ifndef SCAN_H
#define SCAN_H

#include "global.h"
#include "motor.h"
#include "turn.h"
#include "tilt.h"
#include "color.h"
#include "fakedata.h"

uint8_t rgb[3];
int scanRed[NFACE * 8];
int scanGreen[NFACE * 8];
int scanBlue[NFACE * 8];

void ScanAway()
{
    moveAbs(M_SCAN, 100, -340);
}

void ScanCal()
{
    //根据实际测试，比 mindcuber的值略大
    endstop(M_SCAN, 70);

    moveRel(M_SCAN, 70, -100);
    endstop(M_SCAN, 50, 500);
    reset(M_SCAN);
    ScanAway();
}

void ScanRGB(int face, int piece, uint8_t *rgb)
{
    int scanRGBDelay = 100;
    // int scanRGBDelay = 0;
    // int sum = 0;

    // do
    // {
    //     delay(scanRGBDelay / 1000);
    // colorSensor.readColor(&rgb[0], &rgb[1], &rgb[2]);
    uint8_t c = 0;
#ifdef DEBUG
    unsigned long start = millis();
    Serial.println();
    Serial.print(" scan pos before:");
    Serial.print(getPosition(M_SCAN));
    Serial.print(" _pidOutput :");
    Serial.print(motors[M_SCAN]._pidOutput);
    Serial.print(" turn pos:");
    Serial.print(getPosition(M_TURN));
    Serial.println();
#endif

    colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
#ifdef DEBUG

    Serial.print(" after :");
    Serial.print(getPosition(M_SCAN));
    Serial.print(" _pidOutput :");
    Serial.print(motors[M_SCAN]._pidOutput);
    Serial.print(" positions:");
    Serial.print(positions[M_SCAN]);
    Serial.print(" turn pos:");
    Serial.print(getPosition(M_TURN));
    Serial.print(" time:");
    Serial.print(millis() - start);

    Serial.println();
#endif

    // sum = rgb[0] + rgb[1] + rgb[2];
    // if (sum < 0)
    // {
    //     scanRGBDelay = (scanRGBDelay + 1) % 20;
    // }

    cubeColors.setRGB(face, piece, rgb);
    // cubeColors.setRGB(face, piece, fake_rgb[cubeColors.pos(face, piece)]);

    cubeColors.getClr(face, piece);

    // const uint8_t *rgb = clrMapping[cubeColors.getClr(face, piece)];
    // led.on(rgb[0]*125, rgb[1]*20, rgb[2]*20);
    // led.on(rgb[0], rgb[1], rgb[2]);

    // } while (sum < 0);
}

// void CalScanRGB()
// {
//     move(M_SCAN, -70);

//     int i = 0;
//     do
//     {
//         i++;
//         ScanRGB(rgb);
//     } while (i < 10);

//     moveAbs(M_SCAN, 75, 0);
// }

void ScanPiece(int face, int piece)
{
#ifdef DEBUG
    Serial.print("ScanPiece; face:");
    Serial.print(face);
    Serial.print(" piece:");
    Serial.print(piece);

#endif
    // delay(101);
    int32_t pos = getPosition(M_TURN);

    if (scanOK)
    {
        int index = face * 9 + piece;

        ScanRGB(face, piece, rgb);

        uint8_t r1 = rgb[0];
        uint8_t g1 = rgb[1];
        uint8_t b1 = rgb[2];

        // ScanRGB(rgb);
        // ScanRGB(face, piece, rgb);

        // uint8_t r2 = rgb[0];
        // uint8_t g2 = rgb[1];
        // uint8_t b2 = rgb[2];

        // scanRed[index] = r1 + r2;
        // scanGreen[index] = g1 + g2;
        // scanBlue[index] = b1 + b2;
    }

    int32_t offset = pos - getPosition(M_TURN);
#ifdef DEBUG

    cubeColors.print(face);
#endif

    if (offset > 2 && offset < 100)
    {
        turnTableOffset = (turnTableOffset * 18 + offset) / 20;
    }
#ifdef DEBUG

    Serial.println();
    Serial.println();
#endif
}
const int scanDelay = 300;

void ScanMiddle(int face)
{
#ifdef DEBUG
    Serial.print("ScanMiddle; face:");
    Serial.println(face);
    Serial.println();

    start = millis();
#endif
    moveAbs(M_SCAN, 100, -720, false);
    waitForArrival(M_SCAN);
    delay(100);
    ScanPiece(face, 8);
}

void ScanCorner(int face, int piece)
{
    start = millis();
#ifdef DEBUG
    Serial.print("ScanCorner; face:");
    Serial.print(face);
    Serial.print(" piece:");
    Serial.print(piece);
    Serial.println();
#endif
    if (scanOK)
    {

        start = millis();
        moveAbs(M_SCAN, 100, -570, false);
        // delay(scanDelay);

        // Spin45();
        moveRel(M_TURN, 70, -45 * ratio[M_TURN], false);
#ifdef DEBUG
        Serial.print("ScanCorner; wait scan");
        Serial.println();
#endif
        // waitForArrival(M_SCAN, positions[M_SCAN]);
        waitForArrival(M_SCAN);
        // waitForArrival(M_TURN, positions[M_TURN]);
        waitForArrival(M_TURN);
        delay(100);                                                                                                                                                 
        ScanPiece(face, piece);
    }
    else
    {
        Spin45();
    }
}

void ScanEdge(int face, int piece)
{
#ifdef DEBUG
    start = millis();
    Serial.print("ScanEdge; face:");
    Serial.print(face);
    Serial.print(" piece:");
    Serial.println(piece);
    Serial.println();
#endif

    if (scanOK)
    {
        start = millis();
        moveAbs(M_SCAN, 100, -610, false);
        // delay(scanDelay);

        // Spin45();
        moveRel(M_TURN, 70, -45 * ratio[M_TURN], false);
#ifdef DEBUG
        Serial.print("ScanEdge; wait scan");
        Serial.println();
#endif

        // waitForArrival(M_SCAN, positions[M_SCAN]);
        // waitForArrival(M_TURN, positions[M_TURN]);
        waitForArrival(M_SCAN);
        waitForArrival(M_TURN);

        delay(100);
        ScanPiece(face, piece);
    }
    else
    {
        Spin45();
    }
}
void ScanFace(int face, int offset)
{
#ifdef DEBUG
    Serial.print("ScanFace; face:");
    Serial.print(face);
    Serial.print(" offset:");
    Serial.println(offset);
    Serial.println();
#endif

    TiltAway();

    do
    {
        scanOK = true;
        ScanMiddle(face);

        brake(M_TILT);
        // move(M_SCAN, 100);

        // while (getPosition(M_SCAN) <= -650)
        // {
        //     delay(1);
        // }
        // move(M_TURN, -70);
        // moveRel(M_TURN, 38, -ratio[M_TURN] * 360, false);
        start = millis();
        // for (int i = 0; i < 4; i++)
        // {
        //     int p = ((offset + i) * 2) % 8;
        //     ScanCorner(face, p);
        //     p++;
        //     ScanEdge(face, p);
        // }

        for (int i = 0; i < 4; i++)
        {
            int p = offset;
            ScanCorner(face, p);
            p++;
            ScanEdge(face, p);
            offset = (offset + 2) & 7;
        }
        // hold(M_TURN);

        //         int32_t pos = turnTablePosition - getPosition(M_TURN);
        // #ifdef DEBUG
        //         Serial.print(" turnTablePosition:");
        //         Serial.println(turnTablePosition);
        //         Serial.print(" pos:");
        //         Serial.print(pos);
        // #endif

        //         if (pos > 0)
        //         {
        //             turnTablePosition -= (pos - (pos % (360 * 36 / 12)));
        //         }

        //         moveAbs(M_TURN, 75, turnTablePosition);

    } while (!scanOK);
#ifdef DEBUG

    Serial.println();
    Serial.println();
#endif
}

// NXT
bool ScanCube()
{
    ScanFace(3, 2);
    cubeColors.print(3);
    ScanAway();
    Tilt(1);
    ScanFace(2, 2);
    cubeColors.print(2);

    ScanAway();
    Tilt(1);
    ScanFace(1, 2);
    cubeColors.print(1);

    ScanAway();
    Tilt(1);
    ScanFace(0, 2);
    cubeColors.print(0);

    ScanAway();
    Spin(1);
    Tilt(1);
    ScanFace(4, 6);
    cubeColors.print(4);

    ScanAway();
    Tilt(2);
    ScanFace(5, 2);
    cubeColors.print(5);
}

// Spike
// bool ScanCube()
// {
//     ScanFace(0, 4);

//     ScanAway();
//     Tilt(1);
//     ScanFace(4, 6);

//     ScanAway();
//     Tilt(1);
//     ScanFace(2, 0);

//     ScanAway();
//     Tilt(1);
//     Spin(-1);
//     ScanFace(3, 6);

//     ScanAway();
//     Tilt(1);
//     Spin(1);
//     ScanFace(5, 4);

//     ScanAway();
//     Tilt(1);
//     ScanFace(1, 4);
// }

// bool ScanSolve(byte *cube)
// {
//     // motors_off();
//     // flash_red();
//     bool solved = false;
//     lcd.clear();
//     lcd.setCursor(1, 0);
//     lcd.print("Processing...");
//     Serial.println("Processing...");

//     for (int i = 0; i < 6; i++)
//     {
//         determine_colors(cube, i);
//         bool is_valid = valid_pieces(cube);

//         if (is_valid)
//         {
//             pieces_valid++;
//             valid_i = i;
//             // display_cube(cube);
//             if (solve(cube))
//             {
//                 solved = true;
//                 break;
//             }
//         }
//     }

//     // // ClearScreen();
//     // display_cube(cube);
//     if (solved)
//     {
//         lcd.clear();
//         lcd.setCursor(1, 0);
//         lcd.print("Solving...");
//         // delay(500);
//         scan_away();
//         // flash_off();
//         scan_wait();
//     }
//     else
//     {
//         lcd.clear();
//         lcd.setCursor(1, 0);
//         lcd.print("Scan error...");
//         delay(500);
//         if (pieces_valid > 1)
//         {
//             lcd.setCursor(0, 1);
//             lcd.print("Cube ");
//             lcd.print(pieces_valid);
//             delay(1000);
//             // flash_blue();
//         }
//         else
//         {
//             // flash_red();
//         }
//     }
//     uc = L;
//     fc = D;
//     return solved;
// }

#endif