#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void displayInit();

// Màn hình chờ
void displayShowWelcome();

// Màn hình đếm ngược
void displayShowCountdown(uint8_t sec);

// Màn hình lúc chơi
void displayShowGame(
    int score,
    int level,
    int life,
    int remainTime);

// Màn hình game over
void displayShowGameOver(int score);

void displayClear();

#endif