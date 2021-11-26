#include <iostream>
#include <string.h>

#include "../color.h"
#include "../solvercompo.h"
#include "../validator.h"
#include "../fakedata.h"

using namespace std;

CubeColors cubeColors;
CubeSolverCompo solver;
uint8_t cube[NFACE * 8];

void init_cube(uint8_t *cube)
{
    int o = 0;
    for (uint8_t f = 0; f < NFACE; f++)
        for (int i = 0; i < 8; i++)
            cube[o++] = f;
}

int main(int argc, char **argv)
{
    Color colors[NFACE * 9];

    for (int i = 0; i < NFACE; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            cubeColors.setRGB(i, j, fake_colors[3][i * 9 + j]);
        }
    }

    cubeColors.determine_colors(cube, 0);
    cout << "valid_pieces..." << endl;
    bool is_valid = valid_pieces(cube);
    if (is_valid)
    {
        solver.calcInput(cube);
        cout << solver.solverInput << endl;
        solver.solve();
        cout << "result:";
        for (int i = 0; i < solver.resultAmount; i++) {
            cout << "FBRLUD"[solver.resultFaces[i]] <<  solver.resultActions[i];

        }
        cout << endl;
    }
    else
    {
        cout << "not valid" << endl;
    }
}