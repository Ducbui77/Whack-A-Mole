#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

enum GameState
{
    WAITING,
    PLAYING,
    GAME_OVER
};

extern GameState gameState;

extern volatile int score;
extern volatile int life;
extern volatile int level;

extern volatile int currentLED;

extern volatile bool ledHit;

extern volatile uint32_t gameStartTime;

extern volatile uint32_t ledStartTime;

extern volatile uint32_t ledDuration;

#endif