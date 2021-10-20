#include <EEPROM.h>
#include <BricktronicsMegashield.h>
#include <BricktronicsMotor.h>
#include <BricktronicsButton.h>
#include <LiquidCrystal.h>
#include <HCSR04.h>
#include <avr/pgmspace.h>

#include <Color.h>

#include "mcmoves.h"
#include "fakedata.h"

Color color = Color();
const int rs = 49, en = 48, d4 = 47, d5 = 46, d6 = 45, d7 = 44;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
HCSR04 hc(25, 29);//initialisation class HCSR04 (trig pin , echo pin)

BricktronicsButton btns[] = {
  BricktronicsButton(BricktronicsMegashield::SENSOR_2),
  BricktronicsButton(BricktronicsMegashield::SENSOR_3),
  BricktronicsButton(BricktronicsMegashield::SENSOR_4)
};

// Select the desired motor port (MOTOR_1 through MOTOR_6) in the constructor below.
// BricktronicsMotor motor_base(BricktronicsMegashield::MOTOR_1);
// BricktronicsMotor motor_arm(BricktronicsMegashield::MOTOR_2);

BricktronicsMotor motors[] = {
    BricktronicsMotor(BricktronicsMegashield::MOTOR_1),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_2),
    BricktronicsMotor(BricktronicsMegashield::MOTOR_3)};

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
#define M_ARM 1
#define M_SCAN 2

#define BTNCENTER 0
#define BTNLEFT 1
#define BTNRIGHT 2

#define P_LOW 160
#define P_HIGH 180

#define M_WAIT_MUL 4

const int EEPROM_ID_ADDR = 0;
const int EEPROM_DATA_ADDR = 1;
const byte EEPROM_ID = 0x99;

int multipliers[] = {3, 1, 3};
int32_t positions[] = {0, 0, 0};

int32_t motor_base_pos = 0;

void setup()
{
  // Be sure to set your serial console to 115200 baud
  Serial.begin(115200);

  if (color.begin())
  {
    Serial.println("Found color sensor");
  }
  else
  {
    Serial.println("No GY-33 found ... check your connections");
    while (1)
      ;
  }
  lcd.begin(16, 2);

  // motor_base.setEpsilon(10);
  // motor_base._pid.SetOutputLimits(-230, 230);
  motors[M_TURN].begin();
  motors[M_ARM].begin();
  motors[M_SCAN].begin();

  btns[BTNCENTER].begin();
  btns[BTNLEFT].begin();
  btns[BTNRIGHT].begin();

  initialize();

  OCR0A = 0x7F;
  TIMSK0 |= _BV(OCIE0A);
}

// This function will be called every millisecond.
// It just calls update() for each motor.
ISR(TIMER0_COMPA_vect)
{
  static unsigned char count_ms = 0;
  if (++count_ms == 50)
  {
    motors[M_TURN].update();
    motors[M_ARM].update();
    motors[M_SCAN].update();
    count_ms = 0;
  }
}

void initialize()
{
  color.setLevel(8);
  lcd.scrollDisplayLeft();
  lcd.clear();

  // Initialize the motor connections
  motors[M_TURN].setPosition(0);
  motors[M_ARM].setPosition(0);
  motors[M_SCAN].setPosition(0);

  motors[M_TURN]._pid.SetOutputLimits(-P_HIGH, P_HIGH);
  motors[M_ARM]._pid.SetOutputLimits(-150, 150);
  motors[M_SCAN]._pid.SetOutputLimits(-P_HIGH, P_HIGH);

  readWhiteRGB();
}

bool isOverwrite(int32_t a, int32_t b)
{
  if (b > 0)
  {
    if (a > INT32_MAX - b)
    {
      return true;
    }
  }
  if (b < 0)
  {
    if (a < INT32_MIN - b)
    {
      return true;
    }
  }
  return false;
}
void move_abs(int m, int16_t pos)
{

  //实际移动距离
  uint32_t moveCount = 0;
  if ((positions[m] >= 0 && pos >= 0) || (positions[m] <= 0 && pos <= 0))
  {
    moveCount = abs(pos * multipliers[m] - positions[m]);
  }
  if ((positions[m] >= 0 && pos <= 0) || (positions[m] <= 0 && pos >= 0))
  {
    moveCount = abs(pos * multipliers[m] + positions[m]);
  }
  Serial.print("m:");
  Serial.print(m);
  Serial.print(" positions[m]:");
  Serial.print(positions[m]);
  Serial.print(" pos:");
  Serial.print(pos);

  positions[m] = pos * multipliers[m];
  motors[m].goToPosition(positions[m]);

  delay(moveCount * M_WAIT_MUL);

  Serial.print(" curpos:");
  Serial.print(motors[m].getPosition());
  Serial.print(" pos:");
  Serial.print(pos);
  Serial.print(" ms:");
  Serial.println(moveCount * M_WAIT_MUL);
  // motors[m].delayUpdateMS(200);

  // delay(abs(real) * 1000 / 180);
  // motors[m].hold();
}

void move_rel(int m, int16_t pos, int16_t off = 0)
{
  if ((pos + off) == 0)
    return;

  uint32_t moveCount = abs(pos + off) * multipliers[m];

  int32_t real = (pos + off) * multipliers[m];
  if (isOverwrite(positions[m], real))
  {
    positions[m] = real;
    motors[m].setPosition(0);
  }
  else
  {
    positions[m] += real;
  }

  motors[m].goToPosition(positions[m]);

  // motors[m].delayUpdateMS(200);
  delay(moveCount * M_WAIT_MUL);
  Serial.print("move_rel m:");
  Serial.print(m);
  Serial.print(" curpos:");
  Serial.print(motors[m].getPosition());
  Serial.print(" positions[m]:");
  Serial.print(positions[m]);
  Serial.print(" pos:");
  Serial.print(pos);
  Serial.print(" off:");
  Serial.print(off);
  Serial.print(" real:");
  Serial.print(real);
  Serial.print(" ms:");
  Serial.println(moveCount * M_WAIT_MUL);
  if (off != 0)
  {
    uint32_t moveCount = abs(off) * multipliers[m];
    real = -off * multipliers[m];
    if (isOverwrite(positions[m], real))
    {
      positions[m] = real;
      motors[m].setPosition(0);
    }
    else
    {
      positions[m] += real;
    }
    motors[m].goToPosition(positions[m]);

    delay(moveCount * M_WAIT_MUL);
    // delay(real * 5);
  }
  // motors[m].hold();
}

void endstop(int m, bool reverse = false)
{
  int32_t p0, p1;
  // 基于ev3 搭建的话， 扫描臂旋转方向不同
  if (reverse)
  {
    motors[m].setFixedDrive(-P_LOW);
  }
  else
  {
    motors[m].setFixedDrive(P_LOW);
  }
  p1 = motors[m].getPosition();

  while (true)
  {
    delay(500);
    p0 = p1;
    p1 = motors[m].getPosition();
    Serial.print("endstop; m:");
    Serial.print(m);
    Serial.print(" p0:");
    Serial.print(p0);
    Serial.print(" p1:");
    Serial.println(p1);
    if (reverse)
    {
      if (p0 <= p1)
      {
        break;
      }
    }
    else
    {
      if (p0 >= p1)
      {
        break;
      }
    }
  };

  positions[m] = 0;
  motors[m].setPosition(0);
  delay(200);
}

bool holding = false;

void tilt_away()
{
  holding = false;
  move_abs(M_ARM, -T_TAWY);
}
void tilt_release()
{
  holding = false;
  move_abs(M_ARM, -T_TAWY);
}
void tilt_hold()
{
  holding = true;
  move_abs(M_ARM, -T_THLD);
}

void tilt(int n = 1)
{
  if (holding)
  {
    delay(100);
  }
  else
  {
    tilt_hold();
  }
  for (int i = 0; i < n; i++)
  {
    if (i > 0)
    {
      delay(350);
    }

    int t = -T_TILT;
    move_rel(M_ARM, t);
    delay(150);
    move_rel(M_ARM, -t, -20);
  }
}

int apply_sign(int n, int d)
{
  return (n < 0) ? -d : ((n > 0) ? d : 0);
}

void turn(int r)
{
  move_rel(M_TURN, -r * T_90);
}

void turn45(void)
{
  move_rel(M_TURN, T_90 / 2);
}

void turn_face(int r, int rn)
{
  Serial.print("turn_face r:");
  Serial.print(r);
  Serial.print(" rn:");
  Serial.print(rn);
  Serial.print(" apply_sign(r, T_OVR):");
  Serial.print(apply_sign(r, T_OVR));
  Serial.print(" apply_sign(rn, T_CUT):");
  Serial.print(apply_sign(rn, T_CUT));
  Serial.print(" sum:");
  Serial.println(apply_sign(r, T_OVR) + apply_sign(rn, T_CUT));

  move_rel(M_TURN, -r * T_90, -(apply_sign(r, T_OVR) + apply_sign(rn, T_CUT)));
}

// 基于ev3 搭建的话， 扫描臂旋转方向不同
void scan_to_center()
{
  move_abs(M_SCAN, T_SCNT);
}

void scan_to_corner()
{
  move_abs(M_SCAN, T_SCNR);
}

void scan_to_edge()
{
  move_abs(M_SCAN, T_SEDG);
}

void scan_away()
{
  move_abs(M_SCAN, 250);
}

void scan_wait()
{
}

void scan_turn_wait()
{
}

//-----------------------------------------------------------------------------
// Routines for color space conversion and color sorting
//-----------------------------------------------------------------------------

#define NFACE 6 // number of faces in cube
#define POS(FF, OO) (((FF)*8) + (OO))
// int pos(int ff, int oo) {
//   return ff * 8 + oo;
// }

const long cmax = 1024;

int sc_r[NFACE * 9];
int sc_g[NFACE * 9];
int sc_b[NFACE * 9];
int sc_h[NFACE * 9];
int sc_s[NFACE * 9];
int sc_l[NFACE * 9];
int sc_sl[NFACE * 9];
char sc_clr[NFACE * 9];

void set_rgb(int o, int *rgb)
{
  long r = rgb[0];
  long g = rgb[1];
  long b = rgb[2];

  Serial.print("setrgb:");
  Serial.print(" r:");
  Serial.print(r);
  Serial.print(" g:");
  Serial.print(g);
  Serial.print(" b:");
  Serial.println(b);
  // Convert to hsl
  long h = 0;
  long s = 0;
  long l = 0;
  long sl = 0;
  long v = r;
  if (g > v)
    v = g;
  if (b > v)
    v = b;
  long m = r;
  if (g < m)
    m = g;
  if (b < m)
    m = b;
  l = (v + m) / 2;
  if (l > 0)
  {
    long vm = v - m;
    s = vm;
    if (s > 0)
    {
      long vf = v + m;
      if (l <= (cmax / 2))
        vf = (2 * cmax) - vf;
      long r2 = (v - r) * vf / vm;
      long g2 = (v - g) * vf / vm;
      long b2 = (v - b) * vf / vm;
      if (r == v)
        h = (g == m) ? 5 * cmax + b2 : 1 * cmax - g2;
      else if (g == v)
        h = (b == m) ? 1 * cmax + r2 : 3 * cmax - b2;
      else
        h = (r == m) ? 3 * cmax + g2 : 5 * cmax - r2;
      h += cmax; // rotate so R/B either side of 0
      h /= 6;
      if (h < 0)
        h += cmax;
      if (h >= cmax)
        h -= cmax;
      sl = s * cmax / l;
    }
  }
  sc_r[o] = r;
  sc_g[o] = g;
  sc_b[o] = b;
  sc_h[o] = h;
  sc_s[o] = s;
  sc_l[o] = l;
  sc_sl[o] = sl;
}

int clr_ratio(long c0, long c1)
{
  int ratio = 0;
  if (c0 < c1)
    ratio = -(2000 * (c1 - c0) / (c1 + c0));
  else if (c0 > c1)
    ratio = (2000 * (c0 - c1) / (c1 + c0));
  return ratio;
}

#define CMP_H 0
#define CMP_S 1
#define CMP_SL 2
#define CMP_SLR 3
#define CMP_L 4
#define CMP_LR 5
#define CMP_R_G 6
#define CMP_R_B 7
#define CMP_B_G 8

bool compare_clrs(const int cmp_fn, const int c0, const int c1)
{
  bool cmp = false;
  switch (cmp_fn)
  {
  case CMP_H:
    cmp = (sc_h[c1] > sc_h[c0]);
    break;
  case CMP_S:
    cmp = (sc_s[c1] > sc_s[c0]);
    break;
  case CMP_SL:
    cmp = (sc_sl[c1] > sc_sl[c0]);
    break;
  case CMP_SLR:
    cmp = (sc_sl[c1] < sc_sl[c0]);
    break;
  case CMP_L:
    cmp = (sc_l[c1] > sc_l[c0]);
    break;
  case CMP_LR:
    cmp = (sc_l[c1] < sc_l[c0]);
    break;
  case CMP_R_G:
    cmp = (clr_ratio(sc_r[c1], sc_g[c1]) < clr_ratio(sc_r[c0], sc_g[c0]));
    break;
  case CMP_R_B:
    cmp = (clr_ratio(sc_r[c1], sc_b[c1]) < clr_ratio(sc_r[c0], sc_b[c0]));
    break;
  case CMP_B_G:
    cmp = (clr_ratio(sc_b[c1], sc_g[c1]) < clr_ratio(sc_b[c0], sc_g[c0]));
    break;
  default:
    Serial.print("error cmp_fn:");
    Serial.println(cmp_fn);
    break;
  }

  return cmp;
}

void sort_clrs(int *co, const int s, const int n, const int cmp_fn)
{
  const int e = s + n - 2;
  int is = s;
  int ie = e;
  do
  {
    int il = e + 2;
    int ih = s - 2;
    for (int i = is; i <= ie; i++)
    {
      if (compare_clrs(cmp_fn, co[i + 1], co[i]))
      {
        int o = co[i];
        co[i] = co[i + 1];
        co[i + 1] = o;
        if (i < il)
          il = i;
        if (i > ih)
          ih = i;
      }
    }
    is = il - 1;
    if (is < s)
      is = s;
    ie = ih + 1;
    if (ie > e)
      ie = e;
  } while (is <= ie);
}

void sort_colors(int *co, const int t, const int s)
{
#ifdef BLACK_FACE
  sort_clrs(co, 0 * s, 6 * s,
#ifdef WHITE_LOGO
            (s == 1) ?
                     // Saturation
                CMP_SL
                     :
#endif
                     // Saturation (un-weighted by Lightness for black)
                CMP_S);
#else
  // Lightness
  sort_clrs(co, 0 * s, 6 * s, CMP_LR);
  // Saturation
  sort_clrs(co, 0 * s, 3 * s, CMP_SL);
#endif // BLACK_FACE
  // Hue
  sort_clrs(co, 1 * s, 5 * s, CMP_H);
  // Red/Orange
  switch (t)
  {
  case 0: /* already sorted by hue */
    break;
  case 1:
    sort_clrs(co, 1 * s, 2 * s, CMP_R_G);
    break;
  case 2:
    sort_clrs(co, 1 * s, 2 * s, CMP_B_G);
    break;
  case 3:
    sort_clrs(co, 1 * s, 2 * s, CMP_R_B);
    break;
  case 4:
    sort_clrs(co, 1 * s, 2 * s, CMP_SLR);
    break;
  case 5:
    sort_clrs(co, 1 * s, 2 * s, CMP_L);
    break;
  }
  int i;
  for (i = 0; i < s; i++)
    sc_clr[co[i]] = CLR_W;
  for (; i < 2 * s; i++)
    sc_clr[co[i]] = CLR_R;
  for (; i < 3 * s; i++)
    sc_clr[co[i]] = CLR_O;
  for (; i < 4 * s; i++)
    sc_clr[co[i]] = CLR_Y;
  for (; i < 5 * s; i++)
    sc_clr[co[i]] = CLR_G;
  for (; i < 6 * s; i++)
    sc_clr[co[i]] = CLR_B;
}

//-----------------------------------------------------------------------------
// Routines to scan a face of the cube and calibrate white balance
//-----------------------------------------------------------------------------

long white_rgb[] = {910, 960, 1024};
uint8_t rgbn[4];

void sample_rgb(int *rgb, int f, int o, bool cal)
{
  const int scan_max = 5;
  delay(10);
  for (int i = 0; i < 3; i++)
    rgb[i] = 0;
  for (int n = 0; n < scan_max; n++)
  {
    color.getRGB(&rgbn[0], &rgbn[1], &rgbn[2], &rgbn[3]);
    Serial.print("sample_rgb");
    Serial.println(rgbn[3]);
    rgb[0] += rgbn[0];
    rgb[1] += rgbn[1];
    rgb[2] += rgbn[2];
  }
  for (int i = 0; i < 3; i++)
  {
    if (cal)
    {
      rgb[i] /= scan_max;
      white_rgb[i] += rgb[i];
    }
    else
    {
      rgb[i] = (white_rgb[i] * rgb[i]) / (scan_max * cmax);
    }
  }
}

int scan_rgb[3];

void scan_face(int n, int f, int o, bool cal = false)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Face ");
  lcd.print(n);

  tilt_release();
  move_rel(M_TURN, -T_SOFF);
  scan_to_center();
  sample_rgb(scan_rgb, f, 8, cal);
  int orgb = f * 9 + 8;
  for (int i = 0; i < 4; i++)
  {
    turn45();
    scan_to_corner();
    set_rgb(orgb, scan_rgb);
    scan_turn_wait();
    sample_rgb(scan_rgb, f, o, cal);
    orgb = f * 9 + o;
    turn45();
    scan_to_edge();
    set_rgb(orgb, scan_rgb);
    scan_turn_wait();
    sample_rgb(scan_rgb, f, o + 1, cal);
    orgb += 1;
    o = (o + 2) & 7;
  }
  move_rel(M_TURN, T_SOFF);
  if (n != 6)
    scan_away();
  set_rgb(orgb, scan_rgb);
}
// fake_scan_face(1, 3, 2);
// fake_scan_face(2, 2, 2);
// fake_scan_face(3, 1, 2);
// fake_scan_face(4, 0, 2);
// turn(-1);
// fake_scan_face(5, 4, 6);
// fake_scan_face(6, 5, 2);
void fake_scan_face(int n, int f, int o, bool cal = false)
{
  Serial.println("fake_scan_face:");

  tilt_release();
  int orgb = f * 9 + 8;
  for (int i = 0; i < 4; i++)
  {
    // scan_to_corner();
    set_rgb(orgb, fake_rgb[orgb]); // 8 3 5 7 1
    orgb = f * 9 + o;
    set_rgb(orgb, fake_rgb[orgb]); // 2 4 6 0
    orgb += 1;
    o = (o + 2) & 7;
  }

  if (n != 6)
    scan_away();
  set_rgb(orgb, fake_rgb[orgb]);
}

//-----------------------------------------------------------------------------
// Face indices
//-----------------------------------------------------------------------------
// R(Right)、L(Left)、U(Up)、D(Down)、F(Front)、B(Back)
#define U 0
#define F 1
#define D 2
#define B 3
#define R 4
#define L 5

//-----------------------------------------------------------------------------
// Routines to display, transform and solve a cube
//-----------------------------------------------------------------------------

// Target moves for solve to finish early to save time
// Average: 42 moves in 5 seconds
// Range:   about 37 to 47 moves in 1 to 10 seconds

#define TARGET 42

#define NPIECE 3   // maximum number of pieces indexed in one phase
#define MV_MAX 100 // maximum number of moves per solve

// #define RFIX(RR) ((((RR) + 1) & 3) - 1) // Normalise to range -1 to 2
int rfix(int rr)
{
  return ((rr + 1) & 3) - 1;
}
#define CHR_R 'r'
#define CHR_B 'b'
#define CHR_O 'o'
#define CHR_G 'g'
#define CHR_W 'w'
#define CHR_Y 'y'

char clr_chr[] = {CHR_R, CHR_B, CHR_O, CHR_G, CHR_W, CHR_Y};

void display_face(byte *cube, int f, int o)
{
  int b = f * 8;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Face ");
  lcd.print(f);
  lcd.print(" ");

  lcd.setCursor(0, 1);

  lcd.print(clr_chr[cube[b + o]]);
  lcd.print(clr_chr[cube[b + (o + 1)]]);
  lcd.print(clr_chr[cube[b + ((o + 2) & 7)]]);
  lcd.print(" ");

  lcd.print(clr_chr[cube[b + ((o + 7) & 7)]]);
  lcd.print(clr_chr[f]);
  lcd.print(clr_chr[cube[b + ((o + 3) & 7)]]);
  lcd.print(" ");

  lcd.print(clr_chr[cube[b + ((o + 6) & 7)]]);
  lcd.print(clr_chr[cube[b + ((o + 5) & 7)]]);
  lcd.print(clr_chr[cube[b + ((o + 4) & 7)]]);
  lcd.print(" ");
  // delay(3000);
}

void display_cube(byte *cube)
{
  display_face(cube, U, 2);
  display_face(cube, F, 2);
  display_face(cube, D, 2);
  display_face(cube, L, 4);
  display_face(cube, B, 0);
  display_face(cube, R, 4);
}

void init_cube(byte *cube)
{
  int o = 0;
  for (byte f = 0; f < NFACE; f++)
    for (int i = 0; i < 8; i++)
      cube[o++] = f;
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

int mv_n;
byte mv_f[MV_MAX];
int mv_r[MV_MAX];

const byte opposite[] = {D, B, U, F, L, R};

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

int idx_ic;
int idx_ie;
int idx_idx[NPIECE];
int idx_nc;
int idx_ne;
int idx;

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

#define FIND_CORNER(F0, O0, F1, O1, F2, O2, I0, I1, I2) \
  c0 = cube[POS(F0, O0)];                               \
  if (c0 == f0)                                         \
  {                                                     \
    if (cube[POS(F1, O1)] == f1 &&                      \
        cube[POS(F2, O2)] == f2)                        \
      return I2;                                        \
  }                                                     \
  else if (c0 == f2)                                    \
  {                                                     \
    if (cube[POS(F1, O1)] == f0 &&                      \
        cube[POS(F2, O2)] == f1)                        \
      return I1;                                        \
  }                                                     \
  else if (c0 == f1)                                    \
  {                                                     \
    if (cube[POS(F1, O1)] == f2 &&                      \
        cube[POS(F2, O2)] == f0)                        \
      return I0;                                        \
  }
int find_corner_fn(byte *cube, byte f0, byte o0, byte f1, byte o1, byte f2, byte o2, byte i0, byte i1, byte i2)
{
  int c0 = cube[POS(f0, o0)];
  if (c0 == f0)
  {
    if (cube[POS(f1, o1)] == f1 &&
        cube[POS(f2, o2)] == f2)
    {
      return i2;
    }
  }

  if (c0 == f2)
  {
    if (cube[POS(f1, o1)] == f0 &&
        cube[POS(f2, o2)] == f1)
    {
      return i1;
    }
  }

  if (c0 == f1)
  {
    if (cube[POS(f1, o1)] == f2 &&
        cube[POS(f2, o2)] == f0)
    {
      return i0;
    }
  }

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

  // int c;
  // c = find_corner_fn(cube, U, 2, B, 4, R, 2, 0, 1, 2);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, U, 0, L, 0, B, 6, 3, 4, 5);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, U, 6, F, 0, L, 2, 6, 7, 8);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, U, 4, R, 0, F, 2, 9, 10, 11);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, D, 0, L, 4, F, 6, 12, 13, 14);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, D, 6, B, 0, L, 6, 15, 16, 17);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, D, 4, R, 4, B, 2, 18, 19, 20);
  // if (c != -1)
  //   return c;

  // c = find_corner_fn(cube, D, 2, F, 4, R, 6, 21, 22, 23);
  // if (c != -1)
  //   return c;
  Serial.print("find_corner fail:");
  Serial.print(" f0:");
  Serial.print(f0);
  Serial.print(" f1:");
  Serial.print(f1);
  Serial.print(" f2:");
  Serial.println(f2);
  return -1;
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

#define FIND_EDGE(F0, O0, F1, O1, I0, I1) \
  e0 = cube[POS(F0, O0)];                 \
  if (e0 == f0)                           \
  {                                       \
    if (cube[POS(F1, O1)] == f1)          \
      return I1;                          \
  }                                       \
  else if (e0 == f1)                      \
  {                                       \
    if (cube[POS(F1, O1)] == f0)          \
      return I0;                          \
  }
// int find_edge_fn(byte *cube, byte f0, byte o0, byte f1, byte o1, byte i0, byte i1)
// {
//   int e0 = cube[POS(f0, o0)];
//   if (e0 == f0)
//   {
//     if (cube[POS(f1, o1)] == f1)
//     {
//       return i1;
//     }
//   }
//   if (e0 == f1)
//   {
//     if (cube[POS(f1, o1)] == f0)
//     {
//       return i0;
//     }
//   }
//   return -1;
// }
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
  // int e;
  // e = find_edge_fn(cube, U, 1, B, 5, 0, 1);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, U, 7, L, 1, 2, 3);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, U, 5, F, 1, 4, 5);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, U, 3, R, 1, 6, 7);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, L, 3, F, 7, 8, 9);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, B, 7, L, 7, 10, 11);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, D, 7, L, 5, 12, 13);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, R, 3, B, 3, 14, 15);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, D, 5, B, 1, 16, 17);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, F, 3, R, 7, 18, 19);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, D, 3, R, 5, 20, 21);
  // if (e != -1)
  //   return e;
  // e = find_edge_fn(cube, D, 1, F, 5, 22, 23);
  // if (e != -1)
  //   return e;

  Serial.print("find_edge fail:");
  Serial.print(" f0:");
  Serial.print(f0);
  Serial.print(" f1:");
  Serial.println(f1);

  return -1;
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

void solve_phase(byte *cube, int mtb, const byte *mtd, int len, bool dorot = true)
{

  int sz = len / mtb;
  idx = sz - idx;

  if (idx > 0)
  {
    int i = (idx - 1) * mtb;
    byte b = pgm_read_byte_near(mtd + i);
    i++;
    Serial.print("solve_phase0:");
    Serial.println(b, HEX);
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

int solve_n;
byte solve_fce[MV_MAX];
int solve_rot[MV_MAX];
byte solve_cube[NFACE * 8];

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

byte pmap[NFACE];

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

//-----------------------------------------------------------------------------
// High level routines to scan, solve and manipulate cube
//-----------------------------------------------------------------------------

// clr_map 记录中心块所在的面
byte clr_map[] = {0, 1, 2, 3, 4, 5};
int clr_ord[NFACE * 4];

void determine_colors(byte *cube, const int t)
{
  for (int i = 0; i < NFACE; i++)
  {
    clr_ord[i] = 9 * i + 8;
  }
  sort_colors(clr_ord, t, 1);
  for (int i = 0; i < NFACE; i++)
  {
    clr_ord[4 * i + 0] = 9 * i + 0;
    clr_ord[4 * i + 1] = 9 * i + 2;
    clr_ord[4 * i + 2] = 9 * i + 4;
    clr_ord[4 * i + 3] = 9 * i + 6;
  }
  sort_colors(clr_ord, t, 4);
  for (int i = 0; i < NFACE; i++)
  {
    clr_ord[4 * i + 0] = 9 * i + 1;
    clr_ord[4 * i + 1] = 9 * i + 3;
    clr_ord[4 * i + 2] = 9 * i + 5;
    clr_ord[4 * i + 3] = 9 * i + 7;
  }
  sort_colors(clr_ord, t, 4);
  for (int f = 0; f < NFACE; f++)
  {
    // clr_map 记录中心块所在的面
    clr_map[sc_clr[f * 9 + 8]] = f;
  }
  clr_chr[clr_map[CLR_R]] = CHR_R;
  clr_chr[clr_map[CLR_B]] = CHR_B;
  clr_chr[clr_map[CLR_O]] = CHR_O;
  clr_chr[clr_map[CLR_G]] = CHR_G;
  clr_chr[clr_map[CLR_W]] = CHR_W;
  clr_chr[clr_map[CLR_Y]] = CHR_Y;
  for (int f = 0; f < NFACE; f++)
  {
    for (int o = 0; o < 8; o++)
    {
      cube[POS(f, o)] = clr_map[sc_clr[f * 9 + o]];
    }
  }
}

int uc = U;
int fc = F;

int pieces_valid = 0;
int valid_i = 0;

bool scan_solve_cube(byte *cube)
{
  // flash_off();
  // color_sensor_on();
  // fake_scan_face(1, 3, 2);
  scan_face(1, 3, 2);
  tilt();
  // fake_scan_face(2, 2, 2);
  scan_face(2, 2, 2);
  tilt();
  // fake_scan_face(3, 1, 2);
  scan_face(3, 1, 2);
  tilt();
  // fake_scan_face(4, 0, 2);
  scan_face(4, 0, 2);
  turn(-1);
  tilt();
  // fake_scan_face(5, 4, 6);
  scan_face(5, 4, 6);
  tilt(2);
  // fake_scan_face(6, 5, 2);
  scan_face(6, 5, 2);
  // motors_off();
  // flash_red();
  bool solved = false;
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Processing...");
  Serial.println("Processing...");

  for (int i = 0; i < 6; i++)
  {
    determine_colors(cube, i);
    bool is_valid = valid_pieces(cube);

    if (is_valid)
    {
      pieces_valid++;
      valid_i = i;
      // display_cube(cube);
      if (solve(cube))
      {
        solved = true;
        break;
      }
    }
  }

  // // ClearScreen();
  // display_cube(cube);
  if (solved)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Solving...");
    // delay(500);
    scan_away();
    // flash_off();
    scan_wait();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Scan error...");
    delay(500);
    if (pieces_valid > 1)
    {
      lcd.setCursor(0, 1);
      lcd.print("Cube ");
      lcd.print(pieces_valid);
      delay(1000);
      // flash_blue();
    }
    else
    {
      // flash_red();
    }
  }
  uc = L;
  fc = D;
  return solved;
}

void manipulate(byte *cube, int f, int r, int rn)
{
  int map = MAP(uc, fc);
  if (fmap[map + U] == f)
  {
    uc = fmap[map + D];
    fc = fmap[map + B];
    delay(50);
    tilt(2);
  }
  else if (fmap[map + F] == f)
  {
    uc = fmap[map + B];
    fc = fmap[map + U];
    delay(50);
    tilt();
  }
  else if (fmap[map + D] == f)
  {
    tilt_hold();
  }
  else if (fmap[map + B] == f)
  {
    uc = fmap[map + F];
    fc = fmap[map + U];
    tilt_release();
    turn(2);
    tilt();
  }
  else if (fmap[map + R] == f)
  {
    uc = fmap[map + L];
    fc = fmap[map + U];
    tilt_release();
    turn(1);
    tilt();
  }
  else
  { // fmap[map+L] == f
    uc = fmap[map + R];
    fc = fmap[map + U];
    tilt_release();
    turn(-1);
    tilt();
  }
  delay(150);
  rotate(cube, f, r);
  // display_cube(cube);

  Serial.print("manipulate; r:");
  Serial.print(r);
  Serial.print(" rn:");
  Serial.println(rn);

  turn_face(-r, -rn);
}

//-----------------------------------------------------------------------------
// Main task controlling solve and calibration
//-----------------------------------------------------------------------------

int recal = 0;

void calibrate_tilt()
{
  if (recal <= 0)
  {
    // Calibrate tilt position at start and every 5 attempts
    // in case of tacho drift
    recal = 5;
    endstop(M_ARM);
    tilt_away();
  }
  recal--;
}

void banner()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MindCuber v2.2");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
}

#define CD_DIST   16
#define CD_DELAY  200
#define CD_COUNT  5

bool cd_on    = false;
long cd_time  = 0;
int  cd_count = 0;

bool check_cube_detect() {
      Serial.print(" check_cube_detect:");
  if (!cd_on) {
    cd_on    = true;
    cd_time  = millis();
    cd_count = 0;

  }
  long diff = millis()-cd_time;
      Serial.print(" millis():");
      Serial.print(millis());
            Serial.print(" cd_time:");
      Serial.print(cd_time);
            Serial.print(" diff:");
  Serial.println(diff);

  if (diff > 0) {
    cd_time += CD_DELAY;
    return true;
  }
  return false;
}

void cube_detect_off() {
  cd_on = false;
}


bool cube_present()
{
      Serial.print("cube_present:");
  if (check_cube_detect()) {
      Serial.print(" dist:");
  Serial.print(hc.dist());
    if (hc.dist() < CD_DIST)
      cd_count ++;
    else
      cd_count = 0;
  }
      Serial.print(" cd_count:");
  Serial.println(cd_count);
  return cd_count >= CD_COUNT;
}

bool cube_absent()
{
      Serial.print("cube_absent:");

  if (check_cube_detect()) {
  Serial.print(" dist:");
  Serial.print(hc.dist());


    if (hc.dist() >= CD_DIST)
      cd_count ++;
    else
      cd_count = 0;
  }
    Serial.print(" cd_count:");
  Serial.println(cd_count);
  return cd_count >= CD_COUNT;
}

//-----------------------------------------------------------------------------
// Routines to detect button presses with auto-repeat when held
//-----------------------------------------------------------------------------

#define BTN_AUTO_REPEAT_FIRST 1000
#define BTN_AUTO_REPEAT 30

long btn_time = 0;
bool btn_left = false;
bool btn_right = false;
bool btn_center = false;

bool btn_auto_repeat(byte btn, bool &btn_pressed)
{
  if (btns[btn].isPressed()) {
    if (btn_pressed) {
      long diff = millis()-btn_time;
      if (diff >= 0) {
        btn_time += BTN_AUTO_REPEAT;
        return true;
      }
    } else {
      btn_pressed = true;
      btn_time = millis()+BTN_AUTO_REPEAT_FIRST;
      return true;
    }
  } else
    btn_pressed = false;
  return false;
}

bool left_pressed()
{
  return btn_auto_repeat(BTNLEFT, btn_left);
}

bool right_pressed()
{
  return btn_auto_repeat(BTNRIGHT, btn_right);
}

bool center_pressed()
{
  return btn_auto_repeat(BTNCENTER, btn_center);
}

void calibrate_white()
{
  for (int i = 0; i < 3; i++)
    white_rgb[i] = 0;
  scan_face(1, 3, 2, true);
  long cmin = white_rgb[0];
  for (int i = 1; i < 3; i++)
  {
    if (white_rgb[i] < cmin)
      cmin = white_rgb[i];
  }
  lcd.setCursor(0, 0);
  lcd.print("Calibrate white");
  for (int i = 0; i < 3; i++)
  {
    white_rgb[i] = (cmax * cmin) / white_rgb[i];

    lcd.setCursor(4 * i, 1);
    lcd.print(white_rgb[i]);
  }
  // Save white calibration value to EEPROM
  writeWhiteRGB();
}

#define makeLong(hi, low) (((long)hi) << 16 | (low))
#define highWord(w) ((w) >> 16)
#define lowWord(w) ((w)&0xFFFF)
void readWhiteRGB()
{
  // Read the white calibration value from EEPROM
  byte id = EEPROM.read(EEPROM_ID_ADDR);
  if (id == EEPROM_ID)
  {
    for (int i = 0; i < 3; i++)
    {
      byte highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4));
      byte lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 1);
      int16_t highWord = word(highByte, lowByte);

      highByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 2);
      lowByte = EEPROM.read(EEPROM_DATA_ADDR + (i * 4) + 3);
      int16_t lowWord = word(highByte, lowByte);

      white_rgb[i] = makeLong(highWord, lowWord);
    }
  }
}

void writeWhiteRGB()
{
  // Read the white calibration value from EEPROM
  EEPROM.write(EEPROM_ID_ADDR, EEPROM_ID);

  for (int i = 0; i < 3; i++)
  {
    int16_t hw = highWord(white_rgb[i]);
    int16_t lw = lowWord(white_rgb[i]);
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4), highByte(hw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 1, lowByte(hw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 2, highByte(lw));
    EEPROM.write(EEPROM_DATA_ADDR + (i * 4) + 3, lowByte(lw));
  }
}

void loop()
{
  byte cube[NFACE * 8];
  initialize();

  banner();

  // while (btn.isReleased())
  //   ;

  Serial.println("pressed");
  delay(100);

  init_cube(cube);

  calibrate_tilt();
  endstop(M_SCAN, true);
  scan_away();
  scan_wait();

  // tilt();
  // turn(3);
  // turn45();
  // turn45();
  // turn(1);
  // turn(2);

  while (true)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Remove cube...");

    bool cal_white = false;
    while (!cube_absent())
    {
      if (!cal_white && center_pressed())
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Calibrate white");
        lcd.setCursor(0, 1);
        lcd.print("Remove cube...");
        cal_white = true;
      }
      delay(1);
    }
    cube_detect_off();
    calibrate_tilt();

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Insert cube...");

    while (!cube_present())
    {
      if (left_pressed())
      {
        move_rel(M_TURN, -T_ADJ);
      }
      if (right_pressed())
      {
        move_rel(M_TURN, T_ADJ);
      }
      delay(1);
    }

    cube_detect_off();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning...");

    bool solved = false;
    pieces_valid = 0;

    if (cal_white)
    {
      calibrate_white();
    }
    else
    {
      unsigned long start_time = millis();
      for (int tries = 0; !solved && tries < 3; tries++)
      {
        if (scan_solve_cube(cube))
        {

          for (int i = 0; i < solve_n; i++)
          {
            int fi = solve_fce[i];
            int ri = solve_rot[i];
            int fo = opposite[fi];
            int rn = 0;
            for (int j = i + 1; rn == 0 && j < solve_n; j++)
            {
              if (solve_fce[j] != fo)
                rn = solve_rot[j];
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Solving...");
            lcd.setCursor(0, 1);
            lcd.print("Move of ");
            lcd.print(i + 1);
            lcd.print(solve_n);

            manipulate(cube, fi, ri, rn);
          }

          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Solved! ");
          lcd.print(" Moves ");
          lcd.print(solve_n);
          lcd.print(" Time ");
          lcd.print((millis() - start_time) / 1000);

          solved = true;
        }
      }

      scan_away();
      tilt_away();
      scan_wait();
      if (solved)
      {
        turn(8);
      }
    }
  }
}
