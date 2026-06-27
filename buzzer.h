#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

void buzzerInit();

void buzzerOn();

void buzzerOff();

void buzzerBeep(uint16_t duration);

void buzzerStart();

void buzzerHit();

void buzzerMiss();

void buzzerTimeout();

void buzzerGameOver();

#endif