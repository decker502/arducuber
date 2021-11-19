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
    motor.pidSetTunings(2.64, 23.6, 0.1207317073);
    // motor.pidSetTunings(2.64, 14.432, 0.1207317073);
    initialize();

    // OCR0A = 0x7F;
    // TIMSK0 |= _BV(OCIE0A);
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
        Serial.print(" pos:");
        Serial.println(motor._pidOutput);
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

bool waitForArrivalOrTimeout(int32_t position, uint32_t timeoutMS)
{
    motor.goToPosition(position);
    timeoutMS += millis(); // future time when we timeout

    int i = 0;
    while (!motor.settledAtPosition(position))
    {
        motor.update();
        Serial.print(" index : ");
        Serial.print(i);
        Serial.print(" pos:");
        Serial.println(motor.getPosition());
    }
    // motor.hold();
    // motor.goToPosition(motor.getPosition());

    if (millis() >= timeoutMS)
    {
        return false;
    }
    return true;
}

void endstop(int power, int wait = 200)
{

    int32_t p0, p1;
    motor.setFixedDrive(power * 255 / 100);

    p1 = motor.getPosition();

    while (true)
    {
        delay(wait);
        p0 = p1;
        p1 = motor.getPosition();

        if (power < 0)
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
}

void delayUpdateMS(uint32_t delayMS)
{
    unsigned long endTime = millis() + delayMS;
    bool changed = false;
    int i = 0;
    while (millis() < endTime)
    {
        motor._pidInput = motor._encoder.read();
        changed = motor._pid.Compute();
        if (changed)
        {
            motor._rawSetSpeed(motor._pidOutput);
            Serial.print(" index : ");
            Serial.print(i++);
            Serial.print(" _pidOutput:");
            Serial.print(motor._pidOutput);
            Serial.print(" pos:");
            Serial.println(motor.getPosition());
        }

        // We could put a delay(5) here, but the PID library already has a
        // "sample time" parameter to only run so frequent, you know?
    }
}

unsigned char sign = 0;
int ready = 0;
void loop()
{

    if (ready == 1)
    {

        ready = 0;
        endstop(-80);
        initialize();
        motor.goToPositionWaitForArrival(680);

        delay(1000);

        // motor._pid.SetOutputLimits(-255, 255);
        // bool arrival = motor.goToPositionWaitForArrivalOrTimeout(0, 2000);
        // pos = 0;
        // motor.goToPosition(pos);

        // delay(2000);

        // motor._pid.SetOutputLimits(-speed, speed);
        // arrival = motor.goToPositionWaitForArrivalOrTimeout(400, 2000);
        Serial.println(" goto");
        pos = 1440;
        motor.goToPosition(pos);
        delayUpdateMS(2000);
        // delay(2000);

        // motor.goToPosition(pos);
        // motor.brake();
    }

    // waitForArrivalOrTimeout(pos, 1000);
    delay(2000);
}

void serialEvent()
{
    while (Serial.available())
    {

        sign = (unsigned char)Serial.read();
        if (sign == 'a')
        {
            ready = 1;
        }
    }
}