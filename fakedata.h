#ifndef FAKEDATA_H
#define FAKEDATA_H

#define F_RGB_W   \
  {               \
    255, 255, 255 \
  }
#define F_RGB_R \
  {             \
    255, 0, 0   \
  }
#define F_RGB_Y \
  {             \
    255, 255, 0 \
  }
#define F_RGB_O \
  {             \
    255, 165, 0 \
  }
#define F_RGB_B \
  {             \
    0, 0, 255   \
  }
#define F_RGB_G \
  {             \
    0, 128, 0   \
  }

// R(Right)、L(Left)、U(Up)、D(Down)、F(Front)、B(Back)
//  #define U 0
//  #define F 1
//  #define D 2
//  #define B 3
//  #define R 4
//  #define L 5
//  fake_scan_face(1, 3, 2);
//  fake_scan_face(2, 2, 2);
//  fake_scan_face(3, 1, 2);
//  fake_scan_face(4, 0, 2);
//  turn(-1);
//  fake_scan_face(5, 4, 6);
//  fake_scan_face(6, 5, 2);

// 魔方初始状态， 摆放位置为 白色上， 红色前， 前朝向颜色传感器
// 扫描顺序对应于数组下标
// 6: 8 6 7 0 1 2 3 4 5
// 2: 8 2 3 4 5 6 7 0 1
//构造测试数据， 对应于魔方摆放位置 U - W,  F - R, D - Y , B - O, R - B , L - G，

// uint8_t fake_rgb[][3] = {
//     F_RGB_O, F_RGB_O, F_RGB_O,
//     F_RGB_O, F_RGB_O, F_RGB_O,
//     F_RGB_O, F_RGB_O, F_RGB_O,

//     F_RGB_Y, F_RGB_Y, F_RGB_Y,
//     F_RGB_Y, F_RGB_Y, F_RGB_Y,
//     F_RGB_Y, F_RGB_Y, F_RGB_Y,

//     F_RGB_R, F_RGB_R, F_RGB_R,
//     F_RGB_R, F_RGB_R, F_RGB_R,
//     F_RGB_R, F_RGB_R, F_RGB_R,

//     F_RGB_W, F_RGB_W, F_RGB_W,
//     F_RGB_W, F_RGB_W, F_RGB_W,
//     F_RGB_W, F_RGB_W, F_RGB_W,

//     F_RGB_G, F_RGB_G, F_RGB_G,
//     F_RGB_G, F_RGB_G, F_RGB_G,
//     F_RGB_G, F_RGB_G, F_RGB_G,

//     F_RGB_B, F_RGB_B, F_RGB_B,
//     F_RGB_B, F_RGB_B, F_RGB_B,
//     F_RGB_B, F_RGB_B, F_RGB_B,

// };

//魔方上层右旋一次后的数据
// uint8_t fake_rgb[][3] = {
//     F_RGB_G,
//     F_RGB_G,
//     F_RGB_G,
//     F_RGB_O,
//     F_RGB_O,
//     F_RGB_O,
//     F_RGB_O,
//     F_RGB_O,
//     F_RGB_O,

//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,
//     F_RGB_Y,

//     F_RGB_R,
//     F_RGB_R,
//     F_RGB_R,
//     F_RGB_R,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_R,
//     F_RGB_R,

//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,
//     F_RGB_W,

//     F_RGB_G,
//     F_RGB_G,
//     F_RGB_R,
//     F_RGB_R,
//     F_RGB_R,
//     F_RGB_G,
//     F_RGB_G,
//     F_RGB_G,
//     F_RGB_G,

//     F_RGB_O,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_B,
//     F_RGB_O,
//     F_RGB_O,
//     F_RGB_B,

// };

uint8_t fake_rgb[][3] = {
    {246, 88, 76},
    {76, 152, 90},
    {255, 101, 84},
    {89, 54, 55},
    {87, 53, 55},
    {249, 246, 125},
    {86, 52, 53},
    {71, 144, 87},
    {255, 137, 111},
    {45, 60, 76},
    {255, 103, 86},
    {70, 143, 85},
    {91, 54, 55},
    {241, 238, 121},
    {90, 54, 56},
    {170, 168, 177},
    {76, 156, 93},
    {255, 255, 167},
    {255, 98, 82},
    {51, 63, 82},
    {255, 101, 84},
    {255, 102, 85},
    {91, 54, 55},
    {207, 206, 218},
    {86, 53, 54},
    {45, 61, 79},
    {121, 63, 67},
    {78, 163, 95},
    {255, 115, 94},
    {45, 59, 75},
    {255, 99, 83},
    {184, 182, 193},
    {93, 54, 56},
    {255, 253, 128},
    {47, 65, 87},
    {255, 255, 255},
    {72, 147, 87},
    {224, 223, 116},
    {205, 203, 108},
    {186, 184, 195},
    {46, 60, 76},
    {209, 208, 220},
    {202, 200, 211},
    {243, 240, 123},
    {55, 74, 99},
    {190, 188, 102},
    {255, 255, 130},
    {70, 137, 84},
    {74, 152, 90},
    {217, 216, 228},
    {47, 63, 80},
    {45, 60, 77},
    {206, 204, 214},
    {91, 190, 110},
};

// 扫描完后的色块排列
// U D L R F B 代表上下左右前后

// nxt
//
// Face:offset  piece
// 3U:2         8 2 3 4 5 6 7 0 1
// 2R:2         8 2 3 4 5 6 7 0 1
// 1D:2         8 2 3 4 5 6 7 0 1
// 0L:2         8 2 3 4 5 6 7 0 1
// 4B:6         8 6 7 0 1 2 3 4 5
// 5F:2         8 2 3 4 5 6 7 0 1
//
// 扫描前
//       +--4B-+
//       |2 3 4|
//       |1 8 5|
//       |0 7 6|
// +--0L-+--3U-+--2R-+
// |6 7 0|6 7 0|6 7 0|
// |5 8 1|5 8 1|5 8 1|
// |4 3 2|4 3 2|4 3 2|
// +-----+--5F-+-----+
//       |2 3 4|
//       |1 8 5|
//       |0 7 6|
//       +--1D-+
//       |2 3 4|
//       |1 8 5|
//       |0 7 6|
//       +-----+
// 扫描后
//       +--3B-+
//       |2 3 4|
//       |1 8 5|
//       |0 7 6|
// +--2L-+--5U-+--0R-+
// |4 5 6|6 7 0|0 1 2|
// |3 8 7|5 8 1|7 8 3|
// |2 1 0|4 3 2|6 5 4|
// +-----+--1F-+-----+
//       |6 7 0|
//       |5 8 1|
//       |4 3 2|
//       +--4D-+
//       |6 7 0|
//       |5 8 1|
//       |4 3 2|
//       +-----+

// Spike
//
// i(display)
//   0 4 1 5 2 6 3 7
//
// Face:offset  piece
// 0U:4         8 4 5 6 7 0 1 2 3
// 4R:6         8 6 7 0 1 2 3 4 5
// 2D:0         8 0 1 2 3 4 5 6 7
// 3B:6         8 6 7 0 1 2 3 4 5
// 5L:4         8 4 5 6 7 0 1 2 3
// 1F:4         8 4 5 6 7 0 1 2 3
//
// 扫描前
//       +--3B-+
//       |2 3 4|
//       |1 8 5|
//       |0 7 6|
// +--5L-+--0U-+--4R-+
// |0 1 2|0 1 2|2 3 4|
// |7 8 3|7 8 3|1 8 5|
// |6 5 4|6 5 4|0 7 6|
// +-----+--1F-+-----+
//       |0 1 2|
//       |7 8 3|
//       |6 5 4|
//       +--2D-+
//       |4 5 6|
//       |3 8 7|
//       |2 1 0|
//       +-----+
// 扫描后
//       +--0B-+
//       |0 1 2|
//       |7 8 3|
//       |6 5 4|
// +--5L-+--1U-+--4R-+
// |0 1 2|0 1 2|0 1 2|
// |7 8 3|7 8 3|7 8 3|
// |6 5 4|6 5 4|6 5 4|
// +-----+--2F-+-----+
//       |0 1 2|
//       |7 8 3|
//       |6 5 4|
//       +--3D-+
//       |0 1 2|
//       |7 8 3|
//       |6 5 4|
//       +-----+

uint8_t fake_colors[][NFACE * 9][3] = {
    // qiyi
    {{180, 72, 65}, {224, 80, 72}, {142, 134, 134}, {152, 140, 139}, {65, 107, 124}, {66, 108, 124}, {154, 153, 83}, {146, 144, 79}, {167, 81, 73}, {77, 112, 68}, {108, 58, 57}, {107, 49, 53}, {161, 156, 155}, {88, 134, 79}, {93, 137, 79}, {156, 150, 83}, {167, 165, 164}, {206, 202, 198}, {47, 70, 79}, {143, 148, 149}, {136, 136, 134}, {106, 124, 79}, {116, 51, 56}, {91, 136, 79}, {161, 162, 87}, {58, 93, 108}, {255, 110, 104}, {212, 79, 72}, {207, 84, 73}, {60, 98, 113}, {106, 48, 52}, {94, 46, 50}, {168, 159, 87}, {145, 138, 137}, {79, 116, 126}, {254, 252, 124}, {153, 156, 84}, {63, 106, 125}, {78, 113, 69}, {80, 119, 71}, {147, 147, 145}, {162, 166, 91}, {223, 81, 72}, {225, 83, 73}, {116, 182, 101}, {56, 90, 103}, {105, 55, 61}, {116, 50, 55}, {115, 51, 56}, {215, 80, 72}, {209, 79, 70}, {71, 102, 63}, {147, 154, 84}, {90, 148, 168}},
    {{147, 144, 141}, {117, 164, 107}, {183, 184, 181}, {181, 185, 176}, {191, 188, 186}, {139, 64, 70}, {162, 160, 158}, {200, 203, 119}, {150, 223, 123}, {90, 130, 84}, {185, 196, 107}, {78, 123, 139}, {108, 160, 95}, {131, 62, 67}, {225, 219, 115}, {219, 88, 79}, {94, 13, 14}, {255, 255, 153}, {170, 174, 97}, {252, 112, 94}, {193, 192, 105}, {195, 196, 193}, {179, 179, 99}, {81, 137, 160}, {187, 182, 101}, {207, 213, 113}, {108, 180, 204}, {255, 95, 86}, {204, 201, 198}, {120, 58, 62}, {108, 159, 94}, {75, 121, 139}, {195, 197, 196}, {108, 161, 96}, {81, 134, 154}, {241, 241, 234}, {255, 98, 88}, {255, 100, 91}, {140, 62, 68}, {254, 98, 87}, {100, 146, 88}, {138, 72, 71}, {70, 110, 124}, {134, 67, 74}, {199, 75, 84}, {109, 56, 59}, {112, 142, 90}, {245, 94, 84}, {143, 65, 69}, {81, 134, 157}, {82, 136, 157}, {96, 140, 86}, {237, 99, 86}, {255, 128, 115}},
    // dayan
    {{215, 216, 113}, {46, 65, 86}, {83, 53, 53}, {76, 157, 92}, {45, 63, 78}, {74, 143, 87}, {159, 143, 84}, {255, 107, 87}, {255, 133, 107}, {44, 59, 75}, {255, 255, 130}, {255, 99, 82}, {255, 106, 87}, {162, 163, 169}, {87, 56, 56}, {88, 54, 54}, {46, 63, 82}, {255, 255, 157}, {53, 94, 63}, {87, 143, 87}, {225, 83, 70}, {93, 54, 55}, {210, 210, 221}, {90, 54, 55}, {255, 97, 81}, {87, 56, 56}, {115, 59, 60}, {67, 126, 78}, {224, 223, 115}, {68, 48, 48}, {45, 63, 83}, {226, 225, 117}, {196, 197, 207}, {46, 62, 80}, {176, 180, 187}, {255, 255, 255}, {188, 189, 197}, {255, 105, 86}, {70, 139, 84}, {246, 245, 125}, {45, 62, 79}, {201, 203, 213}, {74, 156, 91}, {191, 195, 107}, {92, 198, 111}, {90, 54, 55}, {219, 220, 232}, {255, 105, 87}, {255, 97, 81}, {171, 174, 181}, {49, 65, 82}, {178, 179, 97}, {77, 164, 95}, {53, 74, 99}},
    {{141, 144, 82}, {46, 65, 87}, {84, 53, 54}, {90, 55, 56}, {92, 54, 55}, {255, 99, 84}, {149, 151, 158}, {255, 116, 88}, {255, 130, 106}, {200, 77, 66}, {229, 230, 243}, {227, 226, 118}, {249, 249, 126}, {255, 101, 83}, {52, 68, 75}, {173, 175, 96}, {255, 255, 130}, {255, 255, 157}, {70, 143, 85}, {220, 222, 231}, {207, 205, 110}, {91, 53, 54}, {84, 52, 53}, {90, 54, 55}, {74, 154, 90}, {77, 164, 94}, {118, 59, 62}, {255, 102, 84}, {232, 233, 123}, {168, 169, 177}, {204, 206, 216}, {165, 167, 174}, {255, 255, 127}, {255, 106, 87}, {224, 225, 236}, {255, 255, 255}, {44, 59, 75}, {45, 61, 78}, {40, 52, 61}, {94, 55, 57}, {64, 124, 77}, {77, 164, 94}, {73, 154, 89}, {72, 150, 89}, {92, 201, 113}, {46, 60, 76}, {46, 65, 86}, {46, 64, 85}, {255, 100, 83}, {74, 50, 50}, {255, 107, 88}, {151, 154, 160}, {78, 168, 96}, {51, 72, 99}},
    // Mi
    {{44, 63, 70}, {155, 153, 158}, {65, 73, 53}, {75, 41, 42}, {49, 72, 82}, {160, 157, 162}, {75, 85, 59}, {77, 41, 42}, {165, 120, 91}, {129, 126, 130}, {73, 83, 59}, {141, 138, 142}, {102, 55, 52}, {107, 55, 52}, {50, 74, 85}, {79, 41, 43}, {108, 56, 53}, {94, 106, 71}, {46, 65, 74}, {101, 54, 52}, {66, 75, 54}, {118, 89, 70}, {49, 72, 82}, {110, 56, 53}, {71, 80, 57}, {126, 94, 73}, {203, 199, 205}, {77, 41, 42}, {75, 86, 59}, {103, 54, 51}, {80, 41, 43}, {120, 91, 71}, {51, 76, 87}, {114, 86, 68}, {82, 44, 45}, {66, 102, 119}, {79, 41, 43}, {79, 89, 61}, {102, 54, 52}, {148, 145, 150}, {116, 114, 118}, {78, 89, 64}, {122, 120, 124}, {144, 143, 147}, {133, 68, 65}, {87, 49, 46}, {51, 69, 78}, {67, 39, 40}, {122, 92, 71}, {131, 98, 75}, {51, 75, 87}, {132, 98, 75}, {127, 95, 73}, {95, 47, 49}}};

#endif