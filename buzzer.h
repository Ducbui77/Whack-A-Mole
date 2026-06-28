#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

enum SoundEffect
{
    SND_START,
    SND_HIT,
    SND_MISS,
    SND_TIMEOUT,
    SND_GAMEOVER,
    SND_TICK
};

void buzzerInit();

// Không chặn: đẩy hiệu ứng vào hàng đợi để soundTask phát.
void soundPlay(SoundEffect effect);

// Task phát âm thanh, cô lập delay() khỏi task game.
void soundTask(void *pvParameters);

#endif
