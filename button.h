#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
void buttonInit();

int buttonRead();

bool buttonIsPressed(uint8_t id);

#endif