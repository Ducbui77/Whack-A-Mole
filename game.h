#ifndef GAME_H
#define GAME_H

#include <Arduino.h>

enum GameState
{
    GAME_WAITING,
    GAME_COUNTDOWN,
    GAME_PLAYING,
    GAME_OVER
};

void gameInit();

void gameReset();

void gameUpdate();

// taskInput gửi nút vừa nhấn vào hàng đợi; gameUpdate sẽ xử lý
void gamePostButton(uint8_t button);

#endif