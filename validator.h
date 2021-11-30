#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "global.h"

#define FIND_EDGE(F0, O0, F1, O1, I0, I1) \
    e0 = cube[POS(F0, O0)];               \
    if (e0 == f0)                         \
    {                                     \
        if (cube[POS(F1, O1)] == f1)      \
            return I1;                    \
    }                                     \
    else if (e0 == f1)                    \
    {                                     \
        if (cube[POS(F1, O1)] == f0)      \
            return I0;                    \
    }

#define FIND_CORNER(F0, O0, F1, O1, F2, O2, I0, I1, I2) \
    c0 = cube[POS(F0, O0)];                             \
    if (c0 == f0)                                       \
    {                                                   \
        if (cube[POS(F1, O1)] == f1 &&                  \
            cube[POS(F2, O2)] == f2)                    \
            return I2;                                  \
    }                                                   \
    else if (c0 == f2)                                  \
    {                                                   \
        if (cube[POS(F1, O1)] == f0 &&                  \
            cube[POS(F2, O2)] == f1)                    \
            return I1;                                  \
    }                                                   \
    else if (c0 == f1)                                  \
    {                                                   \
        if (cube[POS(F1, O1)] == f2 &&                  \
            cube[POS(F2, O2)] == f0)                    \
            return I0;                                  \
    }

class Validator
{
public:
    Validator() {}
    bool valid_pieces(uint8_t *cube)
    {
        return (find_edge(cube, U, F) >= 0) &&
               (find_edge(cube, U, L) >= 0) &&
               (find_edge(cube, U, B) >= 0) &&
               (find_edge(cube, U, R) >= 0) &&
               (find_edge(cube, F, L) >= 0) &&
               (find_edge(cube, L, B) >= 0) &&
               (find_edge(cube, B, R) >= 0) &&
               (find_edge(cube, R, F) >= 0) &&
               (find_edge(cube, D, F) >= 0) &&
               (find_edge(cube, D, L) >= 0) &&
               (find_edge(cube, D, B) >= 0) &&
               (find_edge(cube, D, R) >= 0) &&
               (find_corner(cube, U, F, L) >= 0) &&
               (find_corner(cube, U, L, B) >= 0) &&
               (find_corner(cube, U, B, R) >= 0) &&
               (find_corner(cube, U, R, F) >= 0) &&
               (find_corner(cube, D, F, R) >= 0) &&
               (find_corner(cube, D, R, B) >= 0) &&
               (find_corner(cube, D, B, L) >= 0) &&
               (find_corner(cube, D, L, F) >= 0);
    }

    int find_edge(uint8_t *cube, uint8_t f0, uint8_t f1)
    {
        uint8_t e0;
        FIND_EDGE(U, 1, B, 5, 0, 1);
        FIND_EDGE(U, 7, L, 1, 2, 3);
        FIND_EDGE(U, 5, F, 1, 4, 5);
        FIND_EDGE(U, 3, R, 1, 6, 7);
        FIND_EDGE(L, 3, F, 7, 8, 9);
        FIND_EDGE(B, 7, L, 7, 10, 11);
        FIND_EDGE(D, 7, L, 5, 12, 13);
        FIND_EDGE(R, 3, B, 3, 14, 15);
        FIND_EDGE(D, 5, B, 1, 16, 17);
        FIND_EDGE(F, 3, R, 7, 18, 19);
        FIND_EDGE(D, 3, R, 5, 20, 21);
        FIND_EDGE(D, 1, F, 5, 22, 23);
        return -1;
    }

    int find_corner(uint8_t *cube, uint8_t f0, uint8_t f1, uint8_t f2)
    {
        uint8_t c0;
        FIND_CORNER(U, 2, B, 4, R, 2, 0, 1, 2);
        FIND_CORNER(U, 0, L, 0, B, 6, 3, 4, 5);
        FIND_CORNER(U, 6, F, 0, L, 2, 6, 7, 8);
        FIND_CORNER(U, 4, R, 0, F, 2, 9, 10, 11);
        FIND_CORNER(D, 0, L, 4, F, 6, 12, 13, 14);
        FIND_CORNER(D, 6, B, 0, L, 6, 15, 16, 17);
        FIND_CORNER(D, 4, R, 4, B, 2, 18, 19, 20);
        FIND_CORNER(D, 2, F, 4, R, 6, 21, 22, 23);
        return -1;
    }
};

#endif