#ifndef VALIDATOR_H
#define VALIDATOR_H

#define CUBE_UP 0
#define CUBE_FRONT 1
#define CUBE_DOWN 2
#define CUBE_BACK 3
#define CUBE_RIGHT 4
#define CUBE_LEFT 5

#define POS(FF, OO) (((FF)*8) + (OO))

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
        return (find_edge(cube, CUBE_UP, CUBE_FRONT) >= 0) &&
               (find_edge(cube, CUBE_UP, CUBE_LEFT) >= 0) &&
               (find_edge(cube, CUBE_UP, CUBE_BACK) >= 0) &&
               (find_edge(cube, CUBE_UP, CUBE_RIGHT) >= 0) &&
               (find_edge(cube, CUBE_FRONT, CUBE_LEFT) >= 0) &&
               (find_edge(cube, CUBE_LEFT, CUBE_BACK) >= 0) &&
               (find_edge(cube, CUBE_BACK, CUBE_RIGHT) >= 0) &&
               (find_edge(cube, CUBE_RIGHT, CUBE_FRONT) >= 0) &&
               (find_edge(cube, CUBE_DOWN, CUBE_FRONT) >= 0) &&
               (find_edge(cube, CUBE_DOWN, CUBE_LEFT) >= 0) &&
               (find_edge(cube, CUBE_DOWN, CUBE_BACK) >= 0) &&
               (find_edge(cube, CUBE_DOWN, CUBE_RIGHT) >= 0) &&
               (find_corner(cube, CUBE_UP, CUBE_FRONT, CUBE_LEFT) >= 0) &&
               (find_corner(cube, CUBE_UP, CUBE_LEFT, CUBE_BACK) >= 0) &&
               (find_corner(cube, CUBE_UP, CUBE_BACK, CUBE_RIGHT) >= 0) &&
               (find_corner(cube, CUBE_UP, CUBE_RIGHT, CUBE_FRONT) >= 0) &&
               (find_corner(cube, CUBE_DOWN, CUBE_FRONT, CUBE_RIGHT) >= 0) &&
               (find_corner(cube, CUBE_DOWN, CUBE_RIGHT, CUBE_BACK) >= 0) &&
               (find_corner(cube, CUBE_DOWN, CUBE_BACK, CUBE_LEFT) >= 0) &&
               (find_corner(cube, CUBE_DOWN, CUBE_LEFT, CUBE_FRONT) >= 0);
    }

    int find_edge(uint8_t *cube, uint8_t f0, uint8_t f1)
    {
        uint8_t e0;
        FIND_EDGE(CUBE_UP, 1, CUBE_BACK, 5, 0, 1);
        FIND_EDGE(CUBE_UP, 7, CUBE_LEFT, 1, 2, 3);
        FIND_EDGE(CUBE_UP, 5, CUBE_FRONT, 1, 4, 5);
        FIND_EDGE(CUBE_UP, 3, CUBE_RIGHT, 1, 6, 7);
        FIND_EDGE(CUBE_LEFT, 3, CUBE_FRONT, 7, 8, 9);
        FIND_EDGE(CUBE_BACK, 7, CUBE_LEFT, 7, 10, 11);
        FIND_EDGE(CUBE_DOWN, 7, CUBE_LEFT, 5, 12, 13);
        FIND_EDGE(CUBE_RIGHT, 3, CUBE_BACK, 3, 14, 15);
        FIND_EDGE(CUBE_DOWN, 5, CUBE_BACK, 1, 16, 17);
        FIND_EDGE(CUBE_FRONT, 3, CUBE_RIGHT, 7, 18, 19);
        FIND_EDGE(CUBE_DOWN, 3, CUBE_RIGHT, 5, 20, 21);
        FIND_EDGE(CUBE_DOWN, 1, CUBE_FRONT, 5, 22, 23);
        return -1;
    }

    int find_corner(uint8_t *cube, uint8_t f0, uint8_t f1, uint8_t f2)
    {
        uint8_t c0;
        FIND_CORNER(CUBE_UP, 2, CUBE_BACK, 4, CUBE_RIGHT, 2, 0, 1, 2);
        FIND_CORNER(CUBE_UP, 0, CUBE_LEFT, 0, CUBE_BACK, 6, 3, 4, 5);
        FIND_CORNER(CUBE_UP, 6, CUBE_FRONT, 0, CUBE_LEFT, 2, 6, 7, 8);
        FIND_CORNER(CUBE_UP, 4, CUBE_RIGHT, 0, CUBE_FRONT, 2, 9, 10, 11);
        FIND_CORNER(CUBE_DOWN, 0, CUBE_LEFT, 4, CUBE_FRONT, 6, 12, 13, 14);
        FIND_CORNER(CUBE_DOWN, 6, CUBE_BACK, 0, CUBE_LEFT, 6, 15, 16, 17);
        FIND_CORNER(CUBE_DOWN, 4, CUBE_RIGHT, 4, CUBE_BACK, 2, 18, 19, 20);
        FIND_CORNER(CUBE_DOWN, 2, CUBE_FRONT, 4, CUBE_RIGHT, 6, 21, 22, 23);
        return -1;
    }
};

#endif