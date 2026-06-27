#ifndef LED_H
#define LED_H

#include <Arduino.h>

void ledInit();

void ledOn(uint8_t id);

void ledOff(uint8_t id);

void ledToggle(uint8_t id);

void ledAllOff();

/*
 Bật ngẫu nhiên một LED.
 Hàm sẽ tắt toàn bộ LED trước khi bật LED mới.
 Trả về id của LED được bật.
 */
int ledRandom();

/*
 Nhấp nháy LED.
 */
void ledBlink(uint8_t id, uint8_t times, uint16_t delayTime);

#endif