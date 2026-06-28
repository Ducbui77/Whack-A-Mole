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

void ledAllOff()
{
    for (int i = 0; i < NUM_LED; i++)
    {
        digitalWrite(ledPin[i], LOW);
    }
}