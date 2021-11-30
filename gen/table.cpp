
#include <iostream>
#include <string.h>

#include "../moves.h"

CubeMoves moves;
char	*tables[8];
int	tablesize[]={1,4096,  6561,4096,  256,1536,  13824,576};

// calculate a pruning table
void filltable(int ti){
	int n=1,l=1, tl=tablesize[ti];
	// alocate table memory
	char* tb = tables[ti]=new char[tl];
	//clear table
	memset( tb, 0, tl);
	//mark solved position as depth 1
	moves.reset();
	tb[moves.getposition(ti)]=1;
	
	// while there are positions of depth l
	while(n){
		n=0;
		// find each position of depth l
		for(int i=0;i<tl;i++){
			if( tb[i]==l ){
				//construct that cube position
				moves.setposition(ti,i);
				// try each face any amount
				for( int f=0; f<6; f++){
					for( int q=1;q<4;q++){
						moves.domove(f);
						// get resulting position
						int r=moves.getposition(ti);
						// if move as allowed in that phase, and position is a new one
						if( ( q==2 || f>=(ti&6) ) && !tb[r]){
							// mark that position as depth l+1
							tb[r]=l+1;
							n++;
						}
					}
					moves.domove(f);
				}
			}
		}
		l++;
	}
}


int main(int argc, char **argv)
{
    for (int i = 0; i < 8; i++)
    {
        filltable(i);
    }
    printf("#ifndef MOVES_TABLE_H\n");
    printf("#define MOVES_TABLE_H\n");
    for (int i = 0; i < 8; i++)
    {

        printf("#ifdef __AVR__\n");
        printf("const char table%d[] PROGMEM = {\n\t", i);
        for (int j = 0; j < tablesize[i]; j++)
        {
            printf("%d, ", tables[i][j]);
        }
        printf("\n};\n");
        printf("#else\n");
        printf("const char table%d[] = {\n\t", i);
        for (int j = 0; j < tablesize[i]; j++)
        {
            printf("%d, ", tables[i][j]);
        }
        printf("\n};\n");
        printf("#endif\n");
    }
    printf("#endif\n");
}