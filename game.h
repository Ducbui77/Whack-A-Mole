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

void gameStart();

void gameReset();

void gameUpdate();

// khi task Input phát hiện nút được nhấn
void gameButtonPressed(uint8_t button);


GameState gameGetState();

int gameGetScore();

int gameGetLife();

int gameGetLevel();

int gameGetRemainTime();

#endif