#include "buzzer.h"

#include "pins.h"

void buzzerInit()
{
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
}

void buzzerOn()
{
    digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff()
{
    digitalWrite(BUZZER_PIN, LOW);
}

void buzzerBeep(uint16_t duration)
{
    buzzerOn();

    delay(duration);

    buzzerOff();
}

void buzzerStart()
{
    buzzerBeep(80);

    delay(60);

    buzzerBeep(80);
}

void buzzerHit()
{
    buzzerBeep(40);
}

void buzzerMiss()
{
    buzzerBeep(250);
}

void buzzerTimeout()
{
    buzzerBeep(150);

    delay(50);

    buzzerBeep(150);
}


void buzzerGameOver()
{
    for(int i=0;i<3;i++)
    {
        buzzerBeep(250);

        delay(100);
    }
}