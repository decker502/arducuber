#ifndef GLOBAL_H
#define GLOBAL_H

// #define DEBUG

#define U 0
#define F 1
#define D 2
#define B 3
#define R 4
#define L 5

//根据 compo算法中输出顺序确定
// #define F 0
// #define B 1
// #define R 2
// #define L 3
// #define U 4
// #define D 5

//-----------------------------------------------------------------------------
// Background task and routines to use color sensor as flashing light
//-----------------------------------------------------------------------------

#define S_CLR IN_3

#define CLR_R 0
#define CLR_B 1
#define CLR_O 2
#define CLR_G 3
#define CLR_W 4
#define CLR_Y 5

#define L_DEL 1000

// #define T_TILT  85
// #define T_TAWY  25
// #define T_TREL  62
// #define T_THLD 160
// #define T_ADJ   3

// 原值乘以放大系数 1.2
// #define T_TILT  102
// #define T_TAWY  30
// #define T_TREL  75
// #define T_THLD 192
// #define T_ADJ   3

#define T_90 180
#define T_CUT 10
#define T_OVR 57

#define T_SOFF 10
#define T_SCNT 480
#define T_SCNR 380
#define T_SEDG 420

#define T_TILT 140
#define T_TAWY 20
#define T_TREL 62
#define T_THLD 200
#define T_ADJ 3

#define M_TURN 0
#define M_TILT 1
#define M_SCAN 2
#define MAX_M_POWER 255


#define P_LOW 160
#define P_HIGH 180

#define M_WAIT_MUL 4

#define NFACE 6 // number of faces in cube
#define POS(FF, OO) (((FF)*8) + (OO))

#define makeLong(hi, low) (((long)hi) << 16 | (low))
#define highWord(w) ((w) >> 16)
#define lowWord(w) ((w)&0xFFFF)

const int EEPROM_ID_ADDR = 0;
const int EEPROM_DATA_ADDR = 1;
const uint8_t EEPROM_ID = 0x99;

// 齿轮比
const int ratio[] = {3, 1, 3};

const int32_t tiltOffset = 12;

// 电机位置
int32_t positions[] = {0, 0, 0};

int32_t turnTablePosition = 0;
int32_t turnTableOffset = 24;

bool scanOK = true;

int32_t motor_base_pos = 0;
unsigned long start = 0;
    bool solved = false;


#endif
