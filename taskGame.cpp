#include <Arduino.h>

#include "taskGame.h"

#include "config.h"
#include "game.h"

void taskGame(void *pvParameters)
{
    while (true)
    {
        gameUpdate();

        vTaskDelay(pdMS_TO_TICKS(GAME_TASK_DELAY));
    }
}