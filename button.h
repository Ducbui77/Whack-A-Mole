#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

void buttonInit();

// Trả về id nút vừa được nhấn xuống (0..NUM_BUTTON-1), -1 nếu không có.
int buttonRead();

#endif
