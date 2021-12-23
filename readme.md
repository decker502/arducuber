
## 触碰传感器的改造

BricktronicsButton 只支持 Nxt Touch Sensor, 要想使用 Ev3 Touch Sensor 需要进行改造

首先需要理解两者之间的区别 

Nxt Touch Sensor 机械开关连接在引脚 1 和 引脚 2 (GND) 之间，串联一个2.2K 的电阻.

Ev3 Touch Sensor 机械开关连接在引脚 4 （5V） 和 引脚 6  之间 ，串联一个2.2K 的电阻，引脚 1 连接GND， 串联 一个910 ohm 的电阻

可见两者在按下机械开关后，反应是不同的。Nxt Touch Sensor 按下后引脚 1 输出低电平 ， 而 Ev3 Touch Sensor 是引脚 6 输出高电平 。

BricktronicsButton 的 input pin 设为了 pullup模式， 即没有外接负载时，为高电平 。当按下机械开关后, 引脚连通GND，使电平发生变化，input pin即变为低电平 。详见 Arduino 的引脚模式设置。

改造思路

Ev3 Touch Sensor 的引脚4 由接 5V 改为 GND， 使机械开关按下后，引脚 6 输出为低电平 。其他不变。

只需改连接线即可，将排线的3,4线剪断， 连接Touch Sensor 一端的 4 线连到另一端的3线，注意此时排线只能单向使用。此时 Touch Sensor 一端的3线是断开状态，另一端的4线是断开状态。

![gaizao.png](assets/gaizao.png)  



### Ev3 Touch Sensor

参见资料 [ev3_hardware_developer_kit.pdf](./ev3_hardware_developer_kit.pdf)

The touch sensor is a very simple sensor which basically enables building a robot which is able to feel.
The sensor consists of a basic mechanical switch which can either be pressed or released. Given the
simplicity of the sensor, this sensor is also implemented as a pure analog sensor.
The mechanical switch is connected between pin 4, GND with a 2.2 K Ohm resistor in serial and pin 6
within the 6 pole connector. The EV3 P-brick continuously reads the AD-value on pin 6 and through
evaluating this value it is able to determined if the touch sensor is activated or not.
Pin 1 includes a 910 ohm resistor connected to GND, which is used as the ID for the touch sensor.

![ev3-touch-sensor.png](assets/ev3-touch-sensor.png)

### Nxt Touch Sensor

参见资料 [lego_nxt_hw_dev_kit.pdf](./lego_nxt_hw_dev_kit.pdf)

![nxt-btn-sensor](assets/nxt-btn-sensor.png)


### 转盘的改造

参考：
https://tengxiao.wang/index.php/archives/38/

进一步：
42610 x 8

### 颜色传感器

TCS34725 双孔版本

拼装所需乐高配件:

 41678 ｘ 1
 22961 x 2
 18654 x 2
 60483 x 2
6575 x 2

### 感谢

http://www.diy-robots.com/?p=9
https://rubiks-cube-solver.com/zh/
cube20.org
https://tomas.rokicki.com/cubecontest/
http://www.pazhong.net/?p=435