#include "led.h"

#include "pins.h"
#include "config.h"

static const uint8_t ledPin[NUM_LED] =
{
    LED1_PIN,
    LED2_PIN,
    LED3_PIN,
    LED4_PIN,
    LED5_PIN,
    LED6_PIN
};

void ledInit()
{
    for (int i = 0; i < NUM_LED; i++)
    {
        pinMode(ledPin[i], OUTPUT);
        digitalWrite(ledPin[i], LOW);
    }

    // Seed cho random
    randomSeed(micros());
}

void ledOn(uint8_t id)
{
    if (id >= NUM_LED)
        return;

    digitalWrite(ledPin[id], HIGH);
}

void ledOff(uint8_t id)
{
    if (id >= NUM_LED)
        return;

    digitalWrite(ledPin[id], LOW);
}

void ledToggle(uint8_t id)
{
    if (id >= NUM_LED)
        return;

    digitalWrite(ledPin[id], !digitalRead(ledPin[id]));
}

void ledAllOff()
{
    for (int i = 0; i < NUM_LED; i++)
    {
        digitalWrite(ledPin[i], LOW);
    }
}

int ledRandom()
{
    ledAllOff();

    int id = random(NUM_LED);

    digitalWrite(ledPin[id], HIGH);

    return id;
}

void ledBlink(uint8_t id, uint8_t times, uint16_t delayTime)
{
    if (id >= NUM_LED)
        return;

    for (int i = 0; i < times; i++)
    {
        digitalWrite(ledPin[id], HIGH);
        delay(delayTime);

        digitalWrite(ledPin[id], LOW);
        delay(delayTime);
    }
}