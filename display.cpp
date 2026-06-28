#include "display.h"

#include "config.h"
#include "pins.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(
    LCD_ADDRESS,
    LCD_COL,
    LCD_ROW
);
//lưu giá trị cũ
static int oldScore = -1;
static int oldLevel = -1;
static int oldLife = -1;
static int oldTime = -1;

void displayInit()
{
    Wire.begin(
        LCD_SDA_PIN,
        LCD_SCL_PIN
    );

    lcd.init();

    lcd.backlight();

    lcd.clear();
}

void displayClear()
{
    lcd.clear();

    oldScore = -1;
    oldLevel = -1;
    oldLife = -1;
    oldTime = -1;
}

void displayShowWelcome(int highScore)
{
    displayClear();

    lcd.setCursor(0,0);
    lcd.print(" WHACK A MOLE ");

    lcd.setCursor(0,1);
    lcd.print("Hi:");
    lcd.print(highScore);
    lcd.print(" Press Key");
}

void displayShowCountdown(uint8_t sec)
{
    displayClear();

    lcd.setCursor(0,0);
    lcd.print("Starting...");

    lcd.setCursor(7,1);
    lcd.print(sec);
}

void displayShowGame(
    int score,
    int level,
    int life,
    int remainTime)
{

    if(score != oldScore)
    {
        lcd.setCursor(0,0);

        lcd.print("S:");

        if(score < 10)
            lcd.print(' ');

        if(score < 100)
            lcd.print(' ');

        lcd.print(score);

        oldScore = score;
    }

    if(level != oldLevel)
    {
        lcd.setCursor(9,0);

        lcd.print("Lv:");

        lcd.print(level);

        lcd.print(' ');

        oldLevel = level;
    }

    if(life != oldLife)
    {
        lcd.setCursor(0,1);

        lcd.print("HP:");

        lcd.print(life);

        lcd.print(' ');

        oldLife = life;
    }

    if(remainTime != oldTime)
    {
        lcd.setCursor(8,1);

        lcd.print("T:");

        if(remainTime < 10)
            lcd.print(' ');

        lcd.print(remainTime);

        lcd.print(" ");

        oldTime = remainTime;
    }
}

void displayShowGameOver(int score, int highScore)
{
    displayClear();

    lcd.setCursor(0,0);
    lcd.print("   GAME OVER");

    lcd.setCursor(0,1);
    lcd.print("Score:");
    lcd.print(score);

    lcd.print(" Hi:");
    lcd.print(highScore);
}