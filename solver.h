#ifndef CUBE_SOLVER_H
#define CUBE_SOLVER_H

#include <avr/pgmspace.h>

#include "global.h"
#include "mcmoves.h"
#include "tilt.h"
#include "turn.h"

// Use very ugly and unsafe macro to swap items instead of classic routine with
//   pointers for the sole reason of brevity
#define SWAP(a, b) \
    TEMP = a;      \
    a = b;         \
    b = TEMP;
// number 65='A' is often subtracted to convert char ABC... to number 0,1,2,...
#define CHAROFFSET 65

// Target moves for solve to finish early to save time
// Average: 42 moves in 5 seconds
// Range:   about 37 to 47 moves in 1 to 10 seconds

#define TARGET 42
#define NPIECE 3   // maximum number of pieces indexed in one phase
#define MV_MAX 100 // maximum number of moves per solve

#define U 0
#define F 1
#define D 2
#define B 3
#define R 4
#define L 5

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

const byte opposite[] = {D, B, U, F, L, R};

#define MAP(UU, FF) (imap[((UU)*NFACE) + (FF)] * NFACE)

const byte imap[] = {
    /*       U   F   D   B   R   L
  /* U */
    -1, 0, -1, 1, 2, 3,
    /* F */ 4, -1, 5, -1, 6, 7,
    /* D */ -1, 8, -1, 9, 10, 11,
    /* B */ 12, -1, 13, -1, 14, 15,
    /* R */ 16, 17, 18, 19, -1, -1,
    /* L */ 20, 21, 22, 23, -1, -1};

const byte fmap[] = {
    /* -- */
    /* UF */ U, F, D, B, R, L,
    /* -- */
    /* UB */ U, B, D, F, L, R,
    /* UR */ U, R, D, L, B, F,
    /* UL */ U, L, D, R, F, B,

    /* FU */ F, U, B, D, L, R,
    /* -- */
    /* FD */ F, D, B, U, R, L,
    /* -- */
    /* FR */ F, R, B, L, U, D,
    /* FL */ F, L, B, R, D, U,

    /* -- */
    /* DF */ D, F, U, B, L, R,
    /* -- */
    /* DB */ D, B, U, F, R, L,
    /* DR */ D, R, U, L, F, B,
    /* DL */ D, L, U, R, B, F,

    /* BU */ B, U, F, D, R, L,
    /* -- */
    /* BD */ B, D, F, U, L, R,
    /* -- */
    /* BR */ B, R, F, L, D, U,
    /* BL */ B, L, F, R, U, D,

    /* RU */ R, U, L, D, F, B,
    /* RF */ R, F, L, B, D, U,
    /* RD */ R, D, L, U, B, F,
    /* RB */ R, B, L, F, U, D,
    /* -- */
    /* -- */

    /* LU */ L, U, R, D, B, F,
    /* LF */ L, F, R, B, U, D,
    /* LD */ L, D, R, U, F, B,
    /* LB */ L, B, R, F, D, U
    /* -- */
    /* -- */
};

const byte omap[] = {
    /* -- */
    /* UF */ 0, 0, 0, 0, 0, 0,
    /* -- */
    /* UB */ 4, 4, 4, 4, 0, 0,
    /* UR */ 6, 0, 2, 4, 4, 0,
    /* UL */ 2, 0, 6, 4, 0, 4,

    /* FU */ 4, 4, 4, 4, 2, 6,
    /* -- */
    /* FD */ 0, 0, 0, 0, 6, 2,
    /* -- */
    /* FR */ 6, 6, 2, 6, 4, 0,
    /* FL */ 2, 2, 6, 2, 0, 4,

    /* -- */
    /* DF */ 4, 4, 4, 4, 4, 4,
    /* -- */
    /* DB */ 0, 0, 0, 0, 4, 4,
    /* DR */ 6, 4, 2, 0, 4, 0,
    /* DL */ 2, 4, 6, 0, 0, 4,

    /* BU */ 0, 0, 0, 0, 2, 6,
    /* -- */
    /* BD */ 4, 4, 4, 4, 6, 2,
    /* -- */
    /* BR */ 6, 2, 2, 2, 4, 0,
    /* BL */ 2, 6, 6, 6, 0, 4,

    /* RU */ 4, 2, 0, 6, 2, 2,
    /* RF */ 2, 2, 2, 6, 2, 2,
    /* RD */ 0, 2, 4, 6, 2, 2,
    /* RB */ 6, 2, 6, 6, 2, 2,
    /* -- */
    /* -- */

    /* LU */ 4, 6, 0, 2, 6, 6,
    /* LF */ 6, 6, 6, 2, 6, 6,
    /* LD */ 0, 6, 4, 2, 6, 6,
    /* LB */ 2, 6, 2, 2, 6, 6,
    /* -- */
    /* -- */
};

class CubeSolver
{
public:
    CubeSolver()
    {
    }

    int find_edge(byte *cube, byte f0, byte f1)
    {
        byte e0;
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

    int find_corner(byte *cube, byte f0, byte f1, byte f2)
    {
        byte c0;
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

    void add_mv(int f, int r)
    {
        int i = mv_n;
        bool mrg = false;
        while (i > 0)
        {
            i--;
            int fi = mv_f[i];
            if (f == fi)
            {
                r += mv_r[i];
                // r = rfix(r);
                r = rfix(r);
                if (r != 0)
                {
                    mv_r[i] = r;
                }
                else
                {
                    mv_n--;
                    while (i < mv_n)
                    {
                        mv_f[i] = mv_f[i + 1];
                        mv_r[i] = mv_r[i + 1];
                        i++;
                    }
                }
                mrg = true;
                break;
            }
            if (opposite[f] != fi)
                break;
        }
        if (!mrg)
        {
            mv_f[mv_n] = f;
            mv_r[mv_n] = rfix(r);
            mv_n++;
        }
    }

    void rot_edges(byte *cube, int r,
                   int f0, int o0, int f1, int o1,
                   int f2, int o2, int f3, int o3)
    {
        f0 *= 8;
        f1 *= 8;
        f2 *= 8;
        f3 *= 8;
        o0 += f0;
        o1 += f1;
        o2 += f2;
        o3 += f3;
        byte p;
        switch (r)
        {
        case 1:
            p = cube[o3];
            cube[o3] = cube[o2];
            cube[o2] = cube[o1];
            cube[o1] = cube[o0];
            cube[o0] = p;
            o0++;
            o1++;
            o2++;
            o3++;
            p = cube[o3];
            cube[o3] = cube[o2];
            cube[o2] = cube[o1];
            cube[o1] = cube[o0];
            cube[o0] = p;
            o0 = f0 + ((o0 + 1) & 7);
            o1 = f1 + ((o1 + 1) & 7);
            o2 = f2 + ((o2 + 1) & 7);
            o3 = f3 + ((o3 + 1) & 7);
            p = cube[o3];
            cube[o3] = cube[o2];
            cube[o2] = cube[o1];
            cube[o1] = cube[o0];
            cube[o0] = p;
            break;

        case 2:
            p = cube[o0];
            cube[o0] = cube[o2];
            cube[o2] = p;
            p = cube[o1];
            cube[o1] = cube[o3];
            cube[o3] = p;
            o0++;
            o1++;
            o2++;
            o3++;
            p = cube[o0];
            cube[o0] = cube[o2];
            cube[o2] = p;
            p = cube[o1];
            cube[o1] = cube[o3];
            cube[o3] = p;
            o0 = f0 + ((o0 + 1) & 7);
            o1 = f1 + ((o1 + 1) & 7);
            o2 = f2 + ((o2 + 1) & 7);
            o3 = f3 + ((o3 + 1) & 7);
            p = cube[o0];
            cube[o0] = cube[o2];
            cube[o2] = p;
            p = cube[o1];
            cube[o1] = cube[o3];
            cube[o3] = p;
            break;

        case 3:
            p = cube[o0];
            cube[o0] = cube[o1];
            cube[o1] = cube[o2];
            cube[o2] = cube[o3];
            cube[o3] = p;
            o0++;
            o1++;
            o2++;
            o3++;
            p = cube[o0];
            cube[o0] = cube[o1];
            cube[o1] = cube[o2];
            cube[o2] = cube[o3];
            cube[o3] = p;
            o0 = f0 + ((o0 + 1) & 7);
            o1 = f1 + ((o1 + 1) & 7);
            o2 = f2 + ((o2 + 1) & 7);
            o3 = f3 + ((o3 + 1) & 7);
            p = cube[o0];
            cube[o0] = cube[o1];
            cube[o1] = cube[o2];
            cube[o2] = cube[o3];
            cube[o3] = p;
            break;

        default:
            Serial.print("error rot_edges:");
            Serial.println(r);
        }
    }

    void rotate(byte *cube, int f, int r)
    {
        r &= 3;
        switch (f)
        {
        case U:
            rot_edges(cube, r, B, 4, R, 0, F, 0, L, 0);
            break;
        case F:
            rot_edges(cube, r, U, 4, R, 6, D, 0, L, 2);
            break;
        case D:
            rot_edges(cube, r, F, 4, R, 4, B, 0, L, 4);
            break;
        case B:
            rot_edges(cube, r, D, 4, R, 2, U, 0, L, 6);
            break;
        case R:
            rot_edges(cube, r, U, 2, B, 2, D, 2, F, 2);
            break;
        case L:
            rot_edges(cube, r, U, 6, F, 6, D, 6, B, 6);
            break;
        default:
            Serial.print("error rot face:");
            Serial.println(f);
        }
        f *= 8;
        byte p;
        switch (r)
        {
        case 1:
            p = cube[f + 7];
            cube[f + 7] = cube[f + 5];
            cube[f + 5] = cube[f + 3];
            cube[f + 3] = cube[f + 1];
            cube[f + 1] = p;
            p = cube[f + 6];
            cube[f + 6] = cube[f + 4];
            cube[f + 4] = cube[f + 2];
            cube[f + 2] = cube[f];
            cube[f] = p;
            break;

        case 2:
            p = cube[f + 1];
            cube[f + 1] = cube[f + 5];
            cube[f + 5] = p;
            p = cube[f + 3];
            cube[f + 3] = cube[f + 7];
            cube[f + 7] = p;
            p = cube[f];
            cube[f] = cube[f + 4];
            cube[f + 4] = p;
            p = cube[f + 2];
            cube[f + 2] = cube[f + 6];
            cube[f + 6] = p;
            break;

        case 3:
            p = cube[f + 1];
            cube[f + 1] = cube[f + 3];
            cube[f + 3] = cube[f + 5];
            cube[f + 5] = cube[f + 7];
            cube[f + 7] = p;
            p = cube[f];
            cube[f] = cube[f + 2];
            cube[f + 2] = cube[f + 4];
            cube[f + 4] = cube[f + 6];
            cube[f + 6] = p;
            break;

        default:
            Serial.print("error rot:");
            Serial.println(r);
        }
    }

    void index_init()
    {
        idx_nc = 0;
        idx_ne = 0;
        idx = 0;
    }

    void index_last()
    {
        idx = ((idx >> 2) << 1) | (idx & 1);
    }
    int index_corner(byte *cube, byte f0, byte f1, byte f2)
    {
        int ic = find_corner(cube, f0, f1, f2);
        for (int i = 0; i < idx_nc; i++)
        {
            if (ic > idx_idx[i])
                ic -= 3;
        }
        idx = (idx * idx_ic) + ic;
        idx_idx[idx_nc++] = ic;
        idx_ic -= 3;
    }

    void index_edge(byte *cube, byte f0, byte f1)
    {
        int ie = find_edge(cube, f0, f1);
        for (int i = 0; i < idx_ne; i++)
        {
            if (ie > idx_idx[i])
                ie -= 2;
        }
        idx = (idx * idx_ie) + ie;
        idx_idx[idx_ne++] = ie;
        idx_ie -= 2;
    }

    bool valid_pieces(byte *cube)
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

    void copy_cube(byte *cube0, byte *cube1)
    {
        int o = 0;
        for (byte f = 0; f < NFACE; f++)
            for (int i = 0; i < 8; i++)
            {
                cube0[o] = cube1[o];
                o++;
            }
    }

    // #define RFIX(RR) ((((RR) + 1) & 3) - 1) // Normalise to range -1 to 2
    int rfix(int rr)
    {
        return ((rr + 1) & 3) - 1;
    }

    void solve_phase(byte *cube, int mtb, const byte *mtd, int len, bool dorot = true)
    {

        int sz = len / mtb;
        idx = sz - idx;

        if (idx > 0)
        {
            int i = (idx - 1) * mtb;
            byte b = pgm_read_byte_near(mtd + i);
            i++;

            if (b != 0xFF)
            {
                const int mvm = mtb * 2 - 1;
                int mv = 0;
                int f0 = b / 3;
                int r0 = rfix(b - (f0 * 3) + 1);
                add_mv(f0, r0);
                if (dorot)
                    rotate(cube, f0, r0);
                mv++;
                while (mv < mvm)
                {
                    b >>= 4;
                    if ((mv & 1) != 0)
                    {
                        b = pgm_read_byte_near(mtd + i);
                        i++;
                    }
                    byte b0 = b & 0xF;
                    if (b0 == 0xF)
                        break;
                    int f1 = b0 / 3;
                    r0 = rfix(b0 - (f1 * 3) + 1);
                    if (f1 >= f0)
                        f1++;
                    f0 = f1;
                    add_mv(f0, r0);
                    if (dorot)
                        rotate(cube, f0, r0);
                    mv++;
                }
            }
        }
    }

    void solve_one(byte *cube, bool dorot)
    {
        mv_n = 0;

        idx_ic = 24;
        idx_ie = 24;

        index_init();
        index_edge(cube, D, F);
        index_edge(cube, D, R);
        solve_phase(cube, mtb0, mtd0, sizeof(mtd0) / sizeof(mtd0[0]));

        index_init();
        index_corner(cube, D, F, R);
        index_edge(cube, F, R);
        solve_phase(cube, mtb1, mtd1, sizeof(mtd1) / sizeof(mtd1[0]));

        index_init();
        index_edge(cube, D, B);
        solve_phase(cube, mtb2, mtd2, sizeof(mtd2) / sizeof(mtd2[0]));

        index_init();
        index_corner(cube, D, R, B);
        index_edge(cube, R, B);
        solve_phase(cube, mtb3, mtd3, sizeof(mtd3) / sizeof(mtd3[0]));

        index_init();
        index_edge(cube, D, L);
        solve_phase(cube, mtb4, mtd4, sizeof(mtd4) / sizeof(mtd4[0]));

        index_init();
        index_corner(cube, D, B, L);
        index_edge(cube, B, L);
        solve_phase(cube, mtb5, mtd5, sizeof(mtd5) / sizeof(mtd5[0]));

        index_init();
        index_corner(cube, D, L, F);
        index_edge(cube, L, F);
        solve_phase(cube, mtb6, mtd6, sizeof(mtd6) / sizeof(mtd6[0]));

        index_init();
        index_corner(cube, U, R, F);
        index_corner(cube, U, F, L);
        index_corner(cube, U, L, B);
        solve_phase(cube, mtb7, mtd7, sizeof(mtd7) / sizeof(mtd7[0]));

        index_init();
        index_edge(cube, U, R);
        index_edge(cube, U, F);
        index_edge(cube, U, L);
        index_last();
        solve_phase(cube, mtb8, mtd8, sizeof(mtd8) / sizeof(mtd8[0]), dorot);
    }

    bool solved(byte *cube)
    {
        int o = 0;
        for (byte f = 0; f < NFACE; f++)
            for (int i = 0; i < 8; i++)
            {
                if (cube[o++] != f)
                    return false;
            }
        return true;
    }

    bool solve_nomap(byte *cube)
    {
        Serial.println("solve_nomap:");

        solve_n = 0;
        copy_cube(solve_cube, cube);
        solve_one(solve_cube, true);

        if (!solved(solve_cube))
        {
            Serial.println("false");
            return false;
        }

        // Record the solution
        solve_n = mv_n;
        for (int i = 0; i < mv_n; i++)
        {
            solve_fce[i] = mv_f[i];
            solve_rot[i] = mv_r[i];
        }
        return true;
    }

    void solve_remap(byte *cube, int map)
    {
        for (int f = 0; f < NFACE; f++)
            pmap[fmap[map + f]] = f;

        for (int f = 0; f < NFACE; f++)
        {
            int fs = fmap[map + f] * 8;
            int fd = f * 8;
            int os = omap[map + f];
            for (int od = 0; od < 8; od++)
            {
                solve_cube[fd + od] = pmap[cube[fs + os]];
                os = (os + 1) & 7;
            }
        }

        solve_one(solve_cube, false);
        if (mv_n < solve_n)
        {
            solve_n = mv_n;
            for (int i = 0; i < mv_n; i++)
            {
                solve_fce[i] = fmap[map + mv_f[i]];
                solve_rot[i] = mv_r[i];
            }
        }
    }

    bool solve(byte *cube)
    {
        if (!solve_nomap(cube))
            return false;

        // flash_blue();

        if (solve_n <= TARGET)
            return true;

        for (int i = 1; i < NFACE * 4; i++)
        {
            solve_remap(cube, i * NFACE);
            if (solve_n <= TARGET)
                return true;
        }

        return true;
    }

    void manipulate(byte *cube, int f, int r, int rn)
    {
        int map = MAP(uc, fc);
        if (fmap[map + U] == f)
        {
            uc = fmap[map + D];
            fc = fmap[map + B];
            delay(50);
            Tilt(2);
        }
        else if (fmap[map + F] == f)
        {
            uc = fmap[map + B];
            fc = fmap[map + U];
            delay(50);
            Tilt();
        }
        else if (fmap[map + D] == f)
        {
            TiltHold();
        }
        else if (fmap[map + B] == f)
        {
            uc = fmap[map + F];
            fc = fmap[map + U];
            TiltAway();
            Spin(-2);
            Tilt();
        }
        else if (fmap[map + R] == f)
        {
            uc = fmap[map + L];
            fc = fmap[map + U];
            TiltAway();
            Spin(-1);
            Tilt();
        }
        else
        { // fmap[map+L] == f
            uc = fmap[map + R];
            fc = fmap[map + U];
            TiltAway();
            Spin(1);
            Tilt();
        }
        delay(150);
        rotate(cube, f, r);
        // display_cube(cube);

        Serial.print("manipulate; r:");
        Serial.print(r);
        Serial.print(" rn:");
        Serial.println(rn);

        Spin(r);
    }

    void
    calcInput(byte *cube)
    {
        //各个面的中心块代表该面的颜色方向

        char c2r[NFACE] = {'R', 'B', 'L', 'F', 'D', 'U'};
        // UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR
        //边块位置,每2个数字代表一个边块
        int edgemap[48] = {
            5, 3, 3, 3, // UF
            5, 1, 0, 3, // UR
            5, 7, 1, 3, // UB
            5, 5, 2, 3, // UL
            4, 7, 3, 7, // DF
            4, 1, 0, 7, // DR
            4, 3, 1, 7, // DB
            4, 5, 2, 7, // DL
            3, 5, 0, 1, // FR
            3, 1, 2, 5, // FL
            1, 1, 0, 5, // BR
            1, 5, 2, 1, // BL
        };

        //角块位置,每3个数字代表一个角块
        int cornermap[48] = {
            5, 2, 3, 4, 0, 2, // UFR
            5, 0, 0, 4, 1, 2, // URB
            5, 6, 1, 4, 2, 2, // UBL
            5, 4, 2, 4, 3, 2, // ULF
            4, 0, 0, 0, 3, 6, // DRF
            4, 6, 3, 0, 2, 6, // DFL
            4, 4, 2, 0, 1, 6, // DLB
            4, 2, 1, 0, 0, 6, // DBR
        };

        // Edge
        for (int i = 0; i < 12; i++)
        {
            int cubePos = POS(edgemap[i * 4], edgemap[i * 4 + 1]);
            solverInput[i * 3] = c2r[cube[cubePos]];
            cubePos = POS(edgemap[i * 4 + 2], edgemap[i * 4 + 3]);
            solverInput[i * 3 + 1] = c2r[cube[cubePos]];
            solverInput[i * 3 + 2] = ' ';
        }

        int cornerStart = 12 * 3;

        for (int i = 0; i < 8; i++)
        {
            int cubePos = POS(cornermap[i * 6], cornermap[i * 6 + 1]);
            solverInput[i * 4 + cornerStart] = c2r[cube[cubePos]];
            cubePos = POS(cornermap[i * 6 + 2], cornermap[i * 6 + 3]);
            solverInput[i * 4 + 1 + cornerStart] = c2r[cube[cubePos]];
            cubePos = POS(cornermap[i * 6 + 4], cornermap[i * 6 + 5]);
            solverInput[i * 4 + 2 + cornerStart] = c2r[cube[cubePos]];

            if (i != 7)
            {
                solverInput[i * 4 + 3 + cornerStart] = ' ';
            }
            else
            {
                solverInput[i * 4 + 3 + cornerStart] = '\0';
            }
        }
    }

    // Cycles 4 pieces in array p, the piece indices given by a[0..3].
    void cycle(char *p, char *a)
    {
        SWAP(p[*a - CHAROFFSET], p[a[1] - CHAROFFSET]);
        SWAP(p[*a - CHAROFFSET], p[a[2] - CHAROFFSET]);
        SWAP(p[*a - CHAROFFSET], p[a[3] - CHAROFFSET]);
    }

    // twists i-th piece a+1 times.
    void twist(int i, int a)
    {
        i -= CHAROFFSET;
        ori[i] = (ori[i] + a + 1) % val[i];
    }

    // set cube to solved position
    void reset()
    {
        for (int i = 0; i < 20; pos[i] = i, ori[i++] = 0)
            ;
    }

    // convert permutation of 4 chars to a number in range 0..23
    int permtonum(char *p)
    {
        int n = 0;
        for (int a = 0; a < 4; a++)
        {
            n *= 4 - a;
            for (int b = a; ++b < 4;)
                if (p[b] < p[a])
                    n++;
        }
        return n;
    }

    // convert number in range 0..23 to permutation of 4 chars.
    void numtoperm(char *p, int n, int o)
    {
        p += o;
        p[3] = o;
        for (int a = 3; a--;)
        {
            p[a] = n % (4 - a) + o;
            n /= 4 - a;
            for (int b = a; ++b < 4;)
                if (p[b] >= p[a])
                    p[b]++;
        }
    }

    // get index of cube position from table t
    int getposition(int t)
    {
        int i = -1, n = 0;
        switch (t)
        {
        // case 0 does nothing so returns 0
        case 1: // edgeflip
            // 12 bits, set bit if edge is flipped
            for (; ++i < 12;)
                n += ori[i] << i;
            break;
        case 2: // cornertwist
            // get base 3 number of 8 digits - each digit is corner twist
            for (i = 20; --i > 11;)
                n = n * 3 + ori[i];
            break;
        case 3: // middle edge choice
            // 12 bits, set bit if edge belongs in Um middle slice
            for (; ++i < 12;)
                n += (pos[i] & 8) ? (1 << i) : 0;
            break;
        case 4: // ud slice choice
            // 8 bits, set bit if UD edge belongs in Fm middle slice
            for (; ++i < 8;)
                n += (pos[i] & 4) ? (1 << i) : 0;
            break;
        case 5: // tetrad choice, twist and parity
            int corn[8], j, k, l, corn2[4];
            // 8 bits, set bit if corner belongs in second tetrad.
            // also separate pieces for twist/parity determination
            k = j = 0;
            for (; ++i < 8;)
                if ((l = pos[i + 12] - 12) & 4)
                {
                    corn[l] = k++;
                    n += 1 << i;
                }
                else
                    corn[j++] = l;
            // Find permutation of second tetrad after solving first
            for (i = 0; i < 4; i++)
                corn2[i] = corn[4 + corn[i]];
            // Solve one piece of second tetrad
            for (; --i;)
                corn2[i] ^= corn2[0];

            // encode parity/tetrad twist
            n = n * 6 + corn2[1] * 2 - 2;
            if (corn2[3] < corn2[2])
                n++;
            break;
        case 6: // two edge and one corner orbit, permutation
            n = permtonum(pos) * 576 + permtonum(pos + 4) * 24 + permtonum(pos + 12);
            break;
        case 7: // one edge and one corner orbit, permutation
            n = permtonum(pos + 8) * 24 + permtonum(pos + 16);
            break;
        }
        return n;
    }

    // sets cube to any position which has index n in table t
    void setposition(int t, int n)
    {
        int i = 0, j = 12, k = 0;
        char *corn = "QRSTQRTSQSRTQTRSQSTRQTSR";
        reset();
        switch (t)
        {
        // case 0 does nothing so leaves cube solved
        case 1: // edgeflip
            for (; i < 12; i++, n >>= 1)
                ori[i] = n & 1;
            break;
        case 2: // cornertwist
            for (i = 12; i < 20; i++, n /= 3)
                ori[i] = n % 3;
            break;
        case 3: // middle edge choice
            for (; i < 12; i++, n >>= 1)
                pos[i] = 8 * n & 8;
            break;
        case 4: // ud slice choice
            for (; i < 8; i++, n >>= 1)
                pos[i] = 4 * n & 4;
            break;
        case 5: // tetrad choice,parity,twist
            corn += n % 6 * 4;
            n /= 6;
            for (; i < 8; i++, n >>= 1)
                pos[i + 12] = n & 1 ? corn[k++] - CHAROFFSET : j++;
            break;
        case 6: // slice permutations
            numtoperm(pos, n % 24, 12);
            n /= 24;
            numtoperm(pos, n % 24, 4);
            n /= 24;
            numtoperm(pos, n, 0);
            break;
        case 7: // corner permutations
            numtoperm(pos, n / 24, 8);
            numtoperm(pos, n % 24, 16);
            break;
        }
    }

    // do a clockwise quarter turn cube move
    void domove(int m)
    {
        char *p = perm + 8 * m, i = 8;
        // cycle the edges
        cycle(pos, p);
        cycle(ori, p);
        // cycle the corners
        cycle(pos, p + 4);
        cycle(ori, p + 4);
        // twist corners if RLFB
        if (m < 4)
            for (; --i > 3;)
                twist(p[i], i & 1);
        // flip edges if FB
        if (m < 2)
            for (i = 4; i--;)
                twist(p[i], 0);
    }

    // calculate a pruning table
    void filltable(int ti)
    {
        int n = 1, l = 1, tl = tablesize[ti];
        // alocate table memory
        char *tb = tables[ti] = new char[tl];
        // clear table
        memset(tb, 0, tl);
        // mark solved position as depth 1
        reset();
        tb[getposition(ti)] = 1;

        // while there are positions of depth l
        while (n)
        {
            n = 0;
            // find each position of depth l
            for (int i = 0; i < tl; i++)
            {
                if (tb[i] == l)
                {
                    // construct that cube position
                    setposition(ti, i);
                    // try each face any amount
                    for (int f = 0; f < 6; f++)
                    {
                        for (int q = 1; q < 4; q++)
                        {
                            domove(f);
                            // get resulting position
                            int r = getposition(ti);
                            // if move as allowed in that phase, and position is a new one
                            if ((q == 2 || f >= (ti & 6)) && !tb[r])
                            {
                                // mark that position as depth l+1
                                tb[r] = l + 1;
                                n++;
                            }
                        }
                        domove(f);
                    }
                }
            }
            l++;
        }
    }

    // Pruned tree search. recursive.
    bool searchphase(int movesleft, int movesdone, int lastmove)
    {
        // prune - position must still be solvable in the remaining moves available
        if (tables[phase][getposition(phase)] - 1 > movesleft ||
            tables[phase + 1][getposition(phase + 1)] - 1 > movesleft)
            return false;

        // If no moves left to do, we have solved this phase
        if (!movesleft)
            return true;

        // not solved. try each face move
        for (int i = 6; i--;)
        {
            // do not repeat same face, nor do opposite after DLB.
            if (i - lastmove && (i - lastmove + 1 || i | 1))
            {
                moveme[movesdone] = i;
                // try 1,2,3 quarter turns of that face
                for (int j = 0; ++j < 4;)
                {
                    // do move and remember it
                    domove(i);
                    moveamount[movesdone] = j;
                    // Check if phase only allows half moves of this face
                    if ((j == 2 || i >= phase) &&
                        // search on
                        searchphase(movesleft - 1, movesdone + 1, i))
                        return true;
                }
                // put face back to original position.
                domove(i);
            }
        }
        // no solution found
        return false;
    }

    bool solve()
    {
        int f, i = 0, j = 0, k = 0, pc, mor;

        // initialise tables
        for (; k < 20; k++)
            val[k] = k < 12 ? 2 : 3;
        for (; j < 8; j++)
            filltable(j);

        // read input, 20 pieces worth
        for (; i < 20; i++)
        {
            f = pc = k = mor = 0;
            for (; f < val[i]; f++)
            {
                // read input from stdin, or...
                //     do{cin>>c;}while(c==' ');
                //     j=strchr(faces,c)-faces;
                // ...from command line and get face number of facelet
                j = strchr(faces, solverInput[i * val[i] + f]) - faces;
                // keep track of principal facelet for orientation
                if (j > k)
                {
                    k = j;
                    mor = f;
                }
                // construct bit hash code
                pc += 1 << j;
            }
            // find which cubelet it belongs, i.e. the label for this piece
            for (f = 0; f < 20; f++)
                if (pc == bithash[f] - 64)
                    break;
            // store piece
            pos[order[i] - CHAROFFSET] = f;
            ori[order[i] - CHAROFFSET] = mor % val[i];
        }

        // solve the cube
        // four phases
        for (; phase < 8; phase += 2)
        {
            // try each depth till solved
            for (j = 0; !searchphase(j, 0, 9); j++)
                ;
            // output result of this phase
            // for (i = 0; i < j; i++)
            //     cout << "FBRLUD"[moveme[i]] << moveamount[i];
            // cout<<" ";
        }
    }

public:
    bool valid;
    int solve_n;
    byte solve_fce[MV_MAX];
    int solve_rot[MV_MAX];
    byte solve_cube[NFACE * 8];
    int mv_n;
    byte mv_f[MV_MAX];
    int mv_r[MV_MAX];
    int idx_ic;
    int idx_ie;
    int idx_idx[NPIECE];
    int idx_nc;
    int idx_ne;
    int idx;

    int uc = U;
    int fc = F;

    byte pmap[NFACE];

    char solverInput[24 + 12 + 24 + 7];

    // RLFBUD is the face order used for input, so that a correctly oriented
    // piece in the input has its 'highest value' facelet first. The rest of the
    // program uses moves in FBRLUD order.
    char *faces = "RLFBUD";
    // I use char arrays here cause they can be initialised with a string
    // which is shorter than initialising other arrays.
    // Internally cube uses slightly different ordering to the input so that
    //  orbits of stage 4 are contiguous. Note also that the two corner orbits
    //  are diametrically opposite each other.
    // input:  UF UR UB UL  DF DR DB DL  FR FL BR BL  UFR URB UBL ULF   DRF DFL DLB DBR
    //        A  B  C  D   E  F  G  H   I  J  K  L   M   N   O   P     Q   R   S   T
    //        A  E  C  G   B  F  D  H   I  J  K  L   M   S   N   T     R   O   Q   P
    // intrnl: UF DF UB DB  UR DR UL DL  FR FL BR BL  UFR UBL DFL DBR   DLB DRF URB ULF
    char *order = "AECGBFDHIJKLMSNTROQP";
    // To quickly recognise the pieces, I construct an integer by setting a bit for each
    //  facelet. The unique result is then found on the list below to map it to the correct
    //  cubelet of the cube.
    // intrnl: UF DF UB DB  UR DR UL DL  FR FL BR BL  UFR UBL DFL DBR   DLB DRF URB ULF
    // bithash:20,36,24,40, 17,33,18,34, 5, 6, 9, 10, 21, 26, 38, 41,   42, 37, 25, 22
    char *bithash = "TdXhQaRbEFIJUZfijeYV";
    // Each move consists of two 4-cycles. This string contains these in FBRLUD order.
    // intrnl: UF DF UB DB  UR DR UL DL  FR FL BR BL  UFR UBL DFL DBR   DLB DRF URB ULF
    //         A  B  C  D   E  F  G  H   I  J  K  L   M   N   O   P     Q   R   S   T
    char *perm = "AIBJTMROCLDKSNQPEKFIMSPRGJHLNTOQAGCEMTNSBFDHORPQ";

    // current cube position
    char pos[20], ori[20], val[20];
    char TEMP;
    // pruning tables, 2 for each phase
    char *tables[8];
    // current phase solution
    int moveme[20], moveamount[20];
    // current phase being searched (0,2,4,6 for phases 1 to 4)
    int phase = 0;

    // Length of pruning tables. (one dummy in phase 1);
    int tablesize[8] = {1, 4096, 6561, 4096, 256, 1536, 13824, 576};
};

#endif
