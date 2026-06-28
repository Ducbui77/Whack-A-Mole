#include <Arduino.h>

#include "pins.h"
#include "config.h"

#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "display.h"

#include "game.h"

#include "taskGame.h"
#include "taskInput.h"

void setup()
{
    Serial.begin(115200);

    ledInit();

    buttonInit();

    buzzerInit();

    displayInit();

    gameInit();

    xTaskCreatePinnedToCore(
        taskInput,
        "TaskInput",
        2048,
        NULL,
        2,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        taskGame,
        "TaskGame",
        4096,
        NULL,
        1,
        NULL,
        1);

    xTaskCreatePinnedToCore(
        soundTask,
        "TaskSound",
        2048,
        NULL,
        1,
        NULL,
        1);
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}