#include <Arduino.h>

#include "taskInput.h"

#include "config.h"

#include "button.h"
#include "game.h"

void taskInput(void *pvParameters)
{
    while (true)
    {
        int button = buttonRead();

        if (button != -1)
        {
            // nhấn nút bất kì để bắt đầu
            GameState state = gameGetState();
            if (state == GAME_WAITING ||state == GAME_OVER)
            {
                gameStart();
            }
            else
            {
                gameButtonPressed(button);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(INPUT_TASK_DELAY));
    }
}