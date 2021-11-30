#ifndef CUBE_MOVES_H
#define CUBE_MOVES_H
#include "global.h"

// Use very ugly and unsafe macro to swap items instead of classic routine with
//   pointers for the sole reason of brevity
#define SWAP(a, b) \
    TEMP = a;      \
    a = b;         \
    b = TEMP;
// number 65='A' is often subtracted to convert char ABC... to number 0,1,2,...
#define CHAROFFSET 65
#define MOVE_MAX 100

class CubeMoves
{
public:
    CubeMoves()
    {
        for (int k = 0; k < 20; k++)
            val[k] = k < 12 ? 2 : 3;
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

public:
    
    // Each move consists of two 4-cycles. This string contains these in FBRLUD order.
    // intrnl: UF DF UB DB  UR DR UL DL  FR FL BR BL  UFR UBL DFL DBR   DLB DRF URB ULF
    //         A  B  C  D   E  F  G  H   I  J  K  L   M   N   O   P     Q   R   S   T
    char *perm = "AIBJTMROCLDKSNQPEKFIMSPRGJHLNTOQAGCEMTNSBFDHORPQ";

    // current cube position
    char pos[20], ori[20], val[20];
    char TEMP;

};

#endif
