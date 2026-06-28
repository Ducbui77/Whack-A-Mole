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
            gamePostButton((uint8_t)button);
        }

        vTaskDelay(pdMS_TO_TICKS(INPUT_TASK_DELAY));
    }
}
