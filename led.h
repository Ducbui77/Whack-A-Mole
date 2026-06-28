#ifndef LED_H
#define LED_H

#include <Arduino.h>

void ledInit();

void ledOn(uint8_t id);

void ledOff(uint8_t id);

void ledAllOff();

#endif
