#include "buzzer.h"

#include "pins.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static QueueHandle_t soundQueue;

static void buzzerOn()
{
    digitalWrite(BUZZER_PIN, HIGH);
}

static void buzzerOff()
{
    digitalWrite(BUZZER_PIN, LOW);
}

static void buzzerBeep(uint16_t duration)
{
    buzzerOn();
    delay(duration);
    buzzerOff();
}

static void playStart()
{
    buzzerBeep(80);
    delay(60);
    buzzerBeep(80);
}

static void playHit()
{
    buzzerBeep(40);
}

static void playMiss()
{
    buzzerBeep(250);
}

static void playTimeout()
{
    buzzerBeep(150);
    delay(50);
    buzzerBeep(150);
}

static void playGameOver()
{
    for (int i = 0; i < 3; i++)
    {
        buzzerBeep(250);
        delay(100);
    }
}

static void playTick()
{
    buzzerBeep(30);
}

void buzzerInit()
{
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    soundQueue = xQueueCreate(8, sizeof(SoundEffect));
}

void soundPlay(SoundEffect effect)
{
    xQueueSend(soundQueue, &effect, 0);
}

void soundTask(void *pvParameters)
{
    SoundEffect effect;

    while (true)
    {
        if (xQueueReceive(soundQueue, &effect, portMAX_DELAY) == pdTRUE)
        {
            switch (effect)
            {
            case SND_START:    playStart();    break;
            case SND_HIT:      playHit();      break;
            case SND_MISS:     playMiss();     break;
            case SND_TIMEOUT:  playTimeout();  break;
            case SND_GAMEOVER: playGameOver(); break;
            case SND_TICK:     playTick();     break;
            }
        }
    }
}
