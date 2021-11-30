#ifndef CUBE_SOLVER_COMPO_H
#define CUBE_SOLVER_COMPO_H
// #include <iostream>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#include "global.h"
#include "moves.h"
#include "movestable.h"

class CubeSolverCompo
{
public:
    CubeSolverCompo()
    {
    }

    const char *table(int index)
    {
        switch (index)
        {
        case 1:
            return table1;
        case 2:
            return table2;
        case 3:
            return table3;
        case 4:
            return table4;
        case 5:
            return table5;
        case 6:
            return table6;
        case 7:
            return table7;
        }
        return table0;
    }

    void
    calcInput(uint8_t *cube)
    {
        //各个面的中心块代表该面的颜色方向
        // 扫描顺序和 jaap 算法共同决定了c2r的数组元素顺序
        char c2r[NFACE] = {'R', 'F', 'L', 'B', 'D', 'U'};
        // UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR
        //边块位置,每2个数字代表一个边块
        int edgemap[48] = {
            5, 3, 1, 7, // UF
            5, 1, 0, 7, // UR
            5, 7, 3, 7, // UB
            5, 5, 2, 7, // UL
            4, 7, 1, 3, // DF
            4, 1, 0, 3, // DR
            4, 3, 3, 3, // DB
            4, 5, 2, 3, // DL
            1, 1, 0, 5, // FR
            1, 5, 2, 1, // FL
            3, 5, 0, 1, // BR
            3, 1, 2, 5, // BL
        };

        //角块位置,每3个数字代表一个角块
        int cornermap[48] = {
            5, 2, 1, 0, 0, 6, // UFR
            5, 0, 0, 0, 3, 6, // URB
            5, 6, 3, 0, 2, 6, // UBL
            5, 4, 2, 0, 1, 6, // ULF
            4, 0, 0, 4, 1, 2, // DRF
            4, 6, 1, 4, 2, 2, // DFL
            4, 4, 2, 4, 3, 2, // DLB
            4, 2, 3, 4, 0, 2, // DBR
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

    // Pruned tree search. recursive.
    bool searchphase(int movesleft, int movesdone, int lastmove)
    {
        const char *tb = table(phase);
        const char *tb1 = table(phase + 1);
#ifdef DEBUG
#ifdef __AVR__
        // Serial.println();

        // Serial.print(" searchphase:");
        // Serial.print(" phase:");
        // Serial.print(phase);
        // Serial.print(" moves.getposition(phase):");
        // Serial.print(moves.getposition(phase));
        // Serial.print(" moves.getposition(phase+1):");
        // Serial.print(moves.getposition(phase + 1));
        // Serial.print(" p1: ");
        // Serial.print(tb[moves.getposition(phase)] - 1);
        // Serial.print(" p2: ");
        // Serial.print(tb1[moves.getposition(phase + 1)] - 1);
        // Serial.print(" movesleft:");
        // Serial.print(movesleft);
        // Serial.print(" movesdone:");
        // Serial.print(movesdone);
        // Serial.print(" lastmove:");
        // Serial.print(lastmove);
#else
        std::cout << " searchphase phase:" << phase << " moves.getposition(phase):" << moves.getposition(phase) << " moves.getposition(phase + 1):" << moves.getposition(phase + 1) << " p1: " << tb[moves.getposition(phase)] - 1 << " p2:" << tb1[moves.getposition(phase + 1)] - 1;
#endif
#endif
        // prune - position must still be solvable in the remaining moves available
#ifdef __AVR__
        if (pgm_read_byte_near(tb + moves.getposition(phase)) - 1 > movesleft ||
            pgm_read_byte_near(tb1 + moves.getposition(phase + 1)) - 1 > movesleft)
            return false;
#else
        if (tb[moves.getposition(phase)] - 1 > movesleft ||
            tb1[moves.getposition(phase + 1)] - 1 > movesleft)
            return false;
#endif
        // if (tables[phase][moves.getposition(phase)] - 1 > movesleft ||
        //     tables[phase + 1][moves.getposition(phase + 1)] - 1 > movesleft)
        //     return false;

        // If no moves left to do, we have solved this phase
        if (!movesleft)
            return true;

        // not solved. try each face move
        for (int i = 6; i--;)
        {
            // do not repeat same face, nor do opposite after DLB.
            if (i - lastmove && (i - lastmove + 1 || i | 1))
            {
                movename[movesdone] = i;
                // try 1,2,3 quarter turns of that face
                for (int j = 0; ++j < 4;)
                {
                    // do move and remember it
                    moves.domove(i);

                    moveamount[movesdone] = j;
#ifdef DEBUG
#ifdef __AVR__

                    // Serial.print(" moveamount:");
                    // Serial.print(" i:");
                    // Serial.print(i);
                    // Serial.print(" j:");
                    // Serial.print(j);
                    // Serial.print(" movesdone:");
                    // Serial.print(movesdone);
                    // Serial.print(" movesleft:");
                    // Serial.print(movesleft);
                    // Serial.print(" moveamount[movesdone]:");
                    // Serial.print(moveamount[movesdone]);
                    // Serial.println();
#else
                    std::cout << "moveamount i:" << i << " j:" << j << " movesdone:" << movesdone << " movesleft:" << movesleft << " moveamount[movesdone]:" << moveamount[movesdone] << std::endl;

#endif
#endif
                    // Check if phase only allows half moves of this face
                    if ((j == 2 || i >= phase) &&
                        // search on
                        searchphase(movesleft - 1, movesdone + 1, i))
                        return true;
                }
                // put face back to original position.
                moves.domove(i);
            }
        }
        // no solution found
        return false;
    }

    bool solve()
    {
        int f, i = 0, j = 0, k = 0, index, pc, mor;

        // read input, 20 pieces worth
        for (; i < 20; i++)
        {
            f = pc = k = mor = 0;
            for (; f < moves.val[i]; f++)
            {
                // read input from stdin, or...
                //     do{cin>>c;}while(c==' ');
                //     j=strchr(faces,c)-faces;
                // ...from command line and get face number of facelet

                if (moves.val[i] == 2)
                {
                    index = (i * (moves.val[i] + 1)) + f;
                }
                else
                {
                    index = (i * (moves.val[i] + 1)) + f - 12;
                }

                // cout << i <<  " f: "<< f << " val: " << int(moves.val[i]) << " arg:" << solverInput[index] << endl;
                j = strchr(faces, solverInput[index]) - faces;
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
            moves.pos[order[i] - CHAROFFSET] = f;
            moves.ori[order[i] - CHAROFFSET] = mor % moves.val[i];
        }

        for (int i = 0; i < 20; i++)
        {
#ifdef DEBUG
#ifdef __AVR__
            Serial.println();
            Serial.print(" solve:");
            Serial.print(" i:");
            Serial.print(i);
            Serial.print(" moves.pos:");
            Serial.print(int(moves.pos[i]));
            Serial.print(" moves.ori:");
            Serial.print(int(moves.ori[i]));
#else
            std::cout << "solve:" << i << " moves.pos: " << int(moves.pos[i])
                      << " moves.ori:" << int(moves.ori[i]) << std::endl;
#endif
#endif
        }
        // solve the cube
        // four phases
        count = 0;
        for (; phase < 8; phase += 2)
        {

#ifdef DEBUG
#ifdef __AVR__
            unsigned long start = millis();
            Serial.print(" phase:");
            Serial.print(phase);
#else
            std::cout << "solve phase:" << phase
                      << std::endl;
#endif
#endif

            // try each depth till solved
            for (j = 0; !searchphase(j, 0, 9); j++)
                ;
#ifdef DEBUG
#ifdef __AVR__
            Serial.print(" time:");
            Serial.print(millis() - start);
#else

#endif
#endif
            // output result of this phase
            for (i = 0; i < j; i++)
            {
                if (count > MOVE_MAX)
                {
                    return false;
                }

                soulutions[count] = opposite[movename[i]];
                // soulutions[count] = movename[i];
                actions[count] = moveamount[i];

#ifdef DEBUG
#ifdef __AVR__
                Serial.print(" count:");
                Serial.print(count);
                Serial.print(" movename[i]:");
                Serial.print(movename[i]);
                Serial.print(" opposite[movename[i]]:");
                Serial.print(opposite[movename[i]]);
                Serial.print(" soulutions[count]");
                Serial.print(soulutions[count]);
                Serial.print(" moveamount[i]:");
                Serial.print(moveamount[i]);
                Serial.print(" actions[count]:");
                Serial.print(actions[count]);
                Serial.println();
#endif
#endif

                count++;
            }
            //     cout << "FBRLUD"[movename[i]] << moveamount[i];
            // cout<<" ";
        }

        return true;
    }

public:
    CubeMoves moves;
    bool valid;

    char solverInput[24 + 12 + 24 + 7];
    //解法步骤，存储需要操作的面, F, B, R, L, U, D
    int soulutions[MOVE_MAX];
    //解法动作，1 代表顺时针90度， 2 顺时针180度， 3 逆时针90度
    int actions[MOVE_MAX];
    //解法步数
    int count;

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

    // current phase solution
    int movename[20], moveamount[20];
    // current phase being searched (0,2,4,6 for phases 1 to 4)
    int phase = 0;

    // Length of pruning tables. (one dummy in phase 1);
    // int tablesize[8] = {1, 4096, 6561, 4096, 256, 1536, 13824, 576};
};

#endif
