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
    moveAbs(M_SCAN, 50, -40);
    delay(200);
    brake(M_SCAN);
}

void ScanCal()
{
    //根据实际测试，比 mindcuber的值略大
    colorEndstop(M_SCAN, 50);

    reset(M_SCAN);
    // moveAbs(M_SCAN, 60, -60);
    // delay(200);
    ScanAway();
}

void calibrateRGB()
{

    for (int i = 0; i < 3; i++)
    {
        if (rgb[i] > white_rgb[i])
        {
            rgb[i] = white_rgb[i];
        }
        rgb[i] = map(rgb[i], 0, white_rgb[i], 0, 255);
    }
}

void ScanRGB(int face, int piece, uint8_t *rgb)
{
    uint8_t c = 0;
#ifdef DEBUG
    unsigned long start = millis();
    Serial.println();
    Serial.print(F(" scan pos before:"));
    Serial.print(getPosition(M_SCAN));
    Serial.print(F(" _pidOutput :"));
    Serial.print(motors[M_SCAN]._pidOutput);
    Serial.print(F(" turn pos:"));
    Serial.print(getPosition(M_TURN));
    Serial.println();
#endif

    // colorSensor.getRGB(&rgb[0], &rgb[1], &rgb[2], &c);
    // delay((256 - TCS34725_INTEGRATIONTIME_24MS) * 12 / 5 + 1);
    float red, green, blue;
    colorSensor.getRGB(&red, &green, &blue);
    rgb[0] = uint8_t(red);
    rgb[1] = uint8_t(green);
    rgb[2] = uint8_t(blue);

    calibrateRGB();
#ifdef DEBUG

    Serial.print(F(" after :"));
    Serial.print(getPosition(M_SCAN));
    Serial.print(F(" _pidOutput :"));
    Serial.print(motors[M_SCAN]._pidOutput);
    Serial.print(F(" positions:"));
    Serial.print(positions[M_SCAN]);
    Serial.print(F(" turn pos:"));
    Serial.print(getPosition(M_TURN));
    Serial.print(F(" time:"));
    Serial.print(millis() - start);

    Serial.println();
#endif

    cubeColors.setRGB(face, piece, rgb);

    cubeColors.getClr(face, piece);
}

void ScanPiece(int face, int piece)
{
#ifdef DEBUG
    Serial.print(F("ScanPiece; face:"));
    Serial.print(face);
    Serial.print(F(" piece:"));
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
    }

    int32_t offset = pos - getPosition(M_TURN);
#ifdef DEBUG

    cubeColors.print(face);

    Serial.println();
#endif
}
const int scanDelay = 300;

void ScanMiddle(int face)
{
#ifdef DEBUG
    Serial.print(F("ScanMiddle; face:"));
    Serial.println(face);
    Serial.println();

    start = millis();
#endif
    moveAbs(M_SCAN, 100, T_SCNT, false);
    waitForArrival(M_SCAN);
    delay(100);
    ScanPiece(face, 8);
}

void ScanCorner(int face, int piece)
{
    start = millis();
#ifdef DEBUG
    Serial.print(F("ScanCorner:"));
    Serial.println();
#endif
    if (scanOK)
    {

        start = millis();
        moveAbs(M_SCAN, 100, T_SCNR, false);

        Spin45(false);
#ifdef DEBUG
        Serial.print(F("ScanCorner; wait scan"));
        Serial.println();
#endif
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

void ScanEdge(int face, int piece)
{
#ifdef DEBUG
    start = millis();
    Serial.print(F("ScanEdge:"));
    Serial.println();
#endif

    if (scanOK)
    {
        start = millis();
        moveAbs(M_SCAN, 100, T_SEDG, false);

        Spin45(false);
#ifdef DEBUG
        Serial.print(F("ScanEdge; wait scan"));
        Serial.println();
#endif

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
    Serial.print(F("ScanFace; face:"));
    Serial.print(face);
    Serial.print(F(" offset:"));
    Serial.println(offset);
    Serial.println();
#endif

    TiltAway();

    do
    {
        scanOK = true;
        ScanMiddle(face);

        // brake(M_TILT);

        start = millis();

        for (int i = 0; i < 4; i++)
        {
            int p = offset;
            ScanCorner(face, p);
            p++;
            ScanEdge(face, p);
            offset = (offset + 2) & 7;
        }

    } while (!scanOK);
#ifdef DEBUG

    Serial.println();
#endif
}

// NXT
bool ScanCube()
{
    ScanFace(3, 2);
    cubeColors.print(3);
    ScanAway();
    Tilt(1, false);
    ScanFace(2, 2);
    cubeColors.print(2);

    ScanAway();
    Tilt(1, false);
    ScanFace(1, 2);
    cubeColors.print(1);

    ScanAway();
    Tilt(1, false);
    ScanFace(0, 2);
    cubeColors.print(0);

    ScanAway();
    Spin(1);
    delay(100);
    Tilt(1, false);
    ScanFace(4, 6);
    cubeColors.print(4);

    ScanAway();
    Tilt(2, false);
    ScanFace(5, 2);
    cubeColors.print(5);

    ScanAway();
}

#endif