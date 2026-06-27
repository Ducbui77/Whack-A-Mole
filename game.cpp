#include "game.h"

#include "config.h"
#include "led.h"
#include "display.h"
#include "buzzer.h"

static GameState gameState;
static int score;
static int level;
static int life;
static int currentLed;
static unsigned long gameStartTime;
static unsigned long ledStartTime;
static unsigned long ledDuration;

/*PRIVATE FUNCTION*/
static void spawnMole();
static void levelUp();
static void gameOver();

void gameInit()
{
    gameReset();
}

void gameReset()
{
    gameState = GAME_WAITING;
    score = 0;
    level = 1;
    life = PLAYER_LIFE;
    currentLed = -1;
    ledDuration = LED_TIME_START;
    ledAllOff();
    displayShowWelcome();
}

void gameStart()
{
    score = 0;
    level = 1;
    life = PLAYER_LIFE;
    currentLed = -1;
    ledDuration = LED_TIME_START;
    gameStartTime = millis();
    gameState = GAME_PLAYING;

    displayShowGame(
    score,
    level,
    life,
    GAME_TIME_MS / 1000
);

    buzzerStart();
    spawnMole();
}

static void spawnMole()
{
    currentLed = ledRandom();
    ledStartTime = millis();
}

static void levelUp()
{
    level++;
    if (ledDuration > LED_TIME_MIN)
    {
        ledDuration -= LED_TIME_STEP;
        if (ledDuration < LED_TIME_MIN)
        {
            ledDuration = LED_TIME_MIN;
        }
    }
}

static void gameOver()
{
    gameState = GAME_OVER;
    currentLed = -1;
    ledAllOff();
    buzzerGameOver();
    displayShowGameOver(score);
}

GameState gameGetState()
{
    return gameState;
}

int gameGetScore()
{
    return score;
}

int gameGetLife()
{
    return life;
}

int gameGetLevel()
{
    return level;
}

int gameGetRemainTime()
{
    if (gameState != GAME_PLAYING)
        return 0;

    int remain =
        (GAME_TIME_MS - (millis() - gameStartTime)) / 1000;

    if (remain < 0)
        remain = 0;

    return remain;
}

/*game update*/
void gameUpdate()
{
    if (gameState != GAME_PLAYING)
        return;
    // hết giờ game    
    if (millis() - gameStartTime >= GAME_TIME_MS)
    {
        gameOver();
        return;
    }
    // hết thời gian
    if (millis() - ledStartTime >= ledDuration)
    {
        buzzerTimeout();
        life--;
        if(life<=0)
        {
            gameOver();
            return;
        }

        if(millis()-gameStartTime >= GAME_TIME_MS)
        {
            gameOver();
            return;
        }

        spawnMole();
    }
    displayShowGame(
        score,
        level,
        life,
        gameGetRemainTime());
}

//button event
void gameButtonPressed(uint8_t button)
{
    if (gameState != GAME_PLAYING)
        return;
    // đập đúng    
    if (button == currentLed)
    {
        buzzerHit();
        score++;
        // lên level
        if (score % LEVEL_SCORE == 0)
        {
            levelUp();
        }
        spawnMole();
    }
    else
    {
        // đập sai 
        buzzerMiss();
        life--;
        if (life <= 0)
        {
            gameOver();
            return;
        }
    }
}