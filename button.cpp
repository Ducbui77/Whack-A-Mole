#include "button.h"

#include "pins.h"
#include "config.h"

static const uint8_t buttonPin[NUM_BUTTON] =
{
    BTN1_PIN,
    BTN2_PIN,
    BTN3_PIN,
    BTN4_PIN,
    BTN5_PIN,
    BTN6_PIN
};

//Trạng thái đã ổn định sau debounce

static bool buttonState[NUM_BUTTON];

//Giá trị đọc lần gần nhất
static bool lastReading[NUM_BUTTON];

//Thời điểm thay đổi mức điện áp

static unsigned long lastDebounceTime[NUM_BUTTON];

void buttonInit()
{
    for (int i = 0; i < NUM_BUTTON; i++)
    {
        pinMode(buttonPin[i], INPUT_PULLUP);

        buttonState[i] = HIGH;
        lastReading[i] = HIGH;
        lastDebounceTime[i] = 0;
    }
}

int buttonRead()
{
    unsigned long currentTime = millis();

    for (int i = 0; i < NUM_BUTTON; i++)
    {
        bool reading = digitalRead(buttonPin[i]);

        // thay dổi điện áp
        if (reading != lastReading[i])
        {
            lastDebounceTime[i] = currentTime;
            lastReading[i] = reading;
        }

        if ((currentTime - lastDebounceTime[i]) > DEBOUNCE_TIME)
        {
            // trạng thái thay đổi
            if (reading != buttonState[i])
            {
                buttonState[i] = reading;
                //Chỉ trả về khi vừa nhấn xuống
                if (buttonState[i] == LOW)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}