#define F_RGB_W {255,255,255}
#define F_RGB_R {255,0,0}
#define F_RGB_Y {255,255,0}
#define F_RGB_O {255,165,0}
#define F_RGB_B {0,0,255}
#define F_RGB_G {0,128,0}



//R(Right)、L(Left)、U(Up)、D(Down)、F(Front)、B(Back)
// #define U 0
// #define F 1
// #define D 2
// #define B 3
// #define R 4
// #define L 5
  // fake_scan_face(1, 3, 2);
  // fake_scan_face(2, 2, 2);
  // fake_scan_face(3, 1, 2);
  // fake_scan_face(4, 0, 2);
  // turn(-1);
  // fake_scan_face(5, 4, 6);
  // fake_scan_face(6, 5, 2);

// 魔方初始状态， 摆放位置为 白色上， 红色前， 前朝向颜色传感器
// 扫描顺序对应于数组下标
// 6: 8 6 7 0 1 2 3 4 5
//2: 8 2 3 4 5 6 7 0 1
//构造测试数据， 对应于魔方摆放位置 U - W,  F - R, D - Y , B - O, R - B , L - G， 

// int fake_rgb[][3] = {
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


//魔方上层右旋一次后的数据 
int fake_rgb[][3] = {
    F_RGB_G, F_RGB_G, F_RGB_G, 
    F_RGB_O, F_RGB_O, F_RGB_O, 
    F_RGB_O, F_RGB_O, F_RGB_O,

    F_RGB_Y, F_RGB_Y, F_RGB_Y, 
    F_RGB_Y, F_RGB_Y, F_RGB_Y, 
    F_RGB_Y, F_RGB_Y, F_RGB_Y,

    F_RGB_R, F_RGB_R, F_RGB_R, 
    F_RGB_R, F_RGB_B, F_RGB_B, 
    F_RGB_B, F_RGB_R, F_RGB_R,

    F_RGB_W, F_RGB_W, F_RGB_W, 
    F_RGB_W, F_RGB_W, F_RGB_W, 
    F_RGB_W, F_RGB_W, F_RGB_W,

    F_RGB_G, F_RGB_G, F_RGB_R, 
    F_RGB_R, F_RGB_R, F_RGB_G, 
    F_RGB_G, F_RGB_G, F_RGB_G,

    F_RGB_O, F_RGB_B, F_RGB_B, 
    F_RGB_B, F_RGB_B, F_RGB_B, 
    F_RGB_O, F_RGB_O, F_RGB_B,

};




// 扫描完后的色块排列( 0-8 数字代码扫描次序)
// 0-5 代表扫描次序
// U D L R F B 代表上下左右前后
// 观看 视频发现F为面向颜色传感器的一面
// 下面为魔方展开图
//             +--3B-+
//             |4 5 6|
//             |3 0 7|
//             |2 1 8|
// +--2D-+--5L-+--0U-+--1F-+
// |6 7 8|8 1 2|5 6 7|5 6 7|
// |5 0 1|7 0 3|4 0 8|4 0 8|
// |4 3 2|6 5 4|3 2 1|3 2 1|
// +-----+-----+--1F-+-----+
//             |6 7 8|
//             |5 0 1|
//             |4 3 2|
//             +-----+
