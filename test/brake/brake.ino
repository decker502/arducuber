#include <EEPROM.h>
#include <avr/pgmspace.h>

#include <BricktronicsMegashield.h>
#include <BricktronicsMotor.h>
#include <BricktronicsButton.h>

// BricktronicsMotor motor = BricktronicsMotor(BricktronicsMegashield::MOTOR_3);
BricktronicsMotor motor(7, 6, 5, 3, 2);
BricktronicsButton btn = BricktronicsButton(BricktronicsMegashield::SENSOR_2);
volatile int32_t pos = 0;
void setup()
{
    // Be sure to set your serial console to 115200 baud
    Serial.begin(115200);

    btn.begin();

    motor.begin();
    motor.pidSetTunings(2.64, 0, 0.1207317073);

    initialize();

    OCR0A = 0x7F;
    TIMSK0 |= _BV(OCIE0A);
}

// This function will be called every millisecond.
// It just calls update() for each motor.
ISR(TIMER0_COMPA_vect)
{
    static unsigned char count_ms = 0;
    if (++count_ms == 5)
    {
        // if (!motor.settledAtPosition(pos))
        // {
            motor.update();
        // }
        count_ms = 0;
    }
}

void initialize()
{
    motor.coast();
    motor.setPosition(0);
    // motor._pid.SetSampleTime(10);
    motor._pid.SetOutputLimits(-255, 255);
}

#define BTN_AUTO_REPEAT_FIRST 1000
#define BTN_AUTO_REPEAT 30

bool btn_pressed = false;
bool btn_left = false;
long btn_time = 0;
bool btn_auto_repeat(bool &btn_pressed)
{
    if (btn.isPressed())
    {
        if (btn_pressed)
        {
            long diff = millis() - btn_time;
            if (diff >= 0)
            {
                btn_time += BTN_AUTO_REPEAT;
                return true;
            }
        }
        else
        {
            btn_pressed = true;
            btn_time = millis() + BTN_AUTO_REPEAT_FIRST;
            return true;
        }
    }
    else
        btn_pressed = false;
    return false;
}

bool leftPressed()
{
    return btn_auto_repeat(btn_left);
}

bool waitForArrivalOrTimeout(int32_t position, uint32_t timeoutMS)
{
    motor.goToPosition(position);
    timeoutMS += millis(); // future time when we timeout
    int stableFactor = 0;
    while (stableFactor < 5)
    {
        Serial.print(F("settledAtPosition :"));
        Serial.print(motor.settledAtPosition(position));
        Serial.print(F(" pos:"));
        Serial.print(motor.getPosition());
        Serial.print(F(" output:"));
        Serial.print(motor._pidOutput);
        Serial.println();

            delay(10);

        while (!motor.settledAtPosition(position))
        {
            stableFactor = 0;
            // motor.update();
        }
        stableFactor++;
    }
// motor.hold();
    // motor.goToPosition(motor.getPosition());

    if (millis() >= timeoutMS)
    {
        return false;
    }
    return true;
}

void loop()
{
    // while (!leftPressed())
    //     ;

    initialize();

    delay(1000);

    for (int s = 0; s <= 10; s++)
    {
        motor._pid.SetOutputLimits(-255, 255);
        // bool arrival = motor.goToPositionWaitForArrivalOrTimeout(0, 2000);
        pos = 0;
        // motor.goToPosition(pos);
        bool arrival = waitForArrivalOrTimeout(pos, 2000);
        if (!arrival)
        {
            Serial.print(F("fail"));
            Serial.println();
            continue;
        }

        delay(2000);

        int speed = (50 + s * 5) * 255 / 100;
        Serial.print(F("speed :"));
        Serial.print(50 + s * 5);
        Serial.println();

        motor._pid.SetOutputLimits(-speed, speed);
        // arrival = motor.goToPositionWaitForArrivalOrTimeout(400, 2000);
        pos = 400;
        arrival = waitForArrivalOrTimeout(pos, 2000);
        if (!arrival)
        {
            Serial.print(F("timout"));
            Serial.println();
            continue;
        }
        // motor.goToPosition(pos);
        // motor.brake();

        for (int i = 0; i < 20; i++)
        {

            Serial.print(F("delay :"));
            Serial.print(i * 50);
            Serial.print(F(" pos:"));
            Serial.print(motor.getPosition());
            Serial.print(F(" output:"));
            Serial.print(motor._pidOutput);
            Serial.println();

            delay(i * 50);
        }
    }
    pos = 0;
    motor.goToPosition(pos);

    // waitForArrivalOrTimeout(pos, 1000);
    delay(2000);
}
