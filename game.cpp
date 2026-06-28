#include "game.h"

#include "config.h"
#include "led.h"
#include "display.h"
#include "buzzer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static GameState gameState;
static int score;
static int level;
static int life;
static int currentLed;
static unsigned long gameStartTime;
static unsigned long ledStartTime;
static unsigned long ledDuration;

// Hàng đợi nút bấm: taskInput ghi vào, gameUpdate (taskGame) đọc ra,
// nhờ vậy toàn bộ trạng thái game chỉ do một task chạm tới.
static QueueHandle_t buttonQueue;

/*PRIVATE FUNCTION*/
static void gameStart();
static void handleButton(uint8_t button);
static void spawnMole();
static void levelUp();
static void gameOver();

void gameInit()
{
    buttonQueue = xQueueCreate(8, sizeof(uint8_t));
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

void gamePostButton(uint8_t button)
{
    xQueueSend(buttonQueue, &button, 0);
}

static void gameStart()
{
    score = 0;
    level = 1;
    life = PLAYER_LIFE;
    currentLed = -1;
    ledDuration = LED_TIME_START;
    gameStartTime = millis();
    gameState = GAME_PLAYING;

    displayClear();
    displayShowGame(
        score,
        level,
        life,
        GAME_TIME_MS / 1000);

    buzzerStart();
    spawnMole();
}

static void spawnMole()
{
    int id;
    do
    {
        id = ledRandom();
    } while (id == currentLed && NUM_LED > 1);

    currentLed = id;
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

static void handleButton(uint8_t button)
{
    // Nhấn nút bất kì để bắt đầu ván mới
    if (gameState != GAME_PLAYING)
    {
        gameStart();
        // Bỏ các nút còn tồn trong hàng đợi để không bị tính ngay vào ván mới
        xQueueReset(buttonQueue);
        return;
    }

    if (button == currentLed)
    {
        buzzerHit();
        score++;
        if (score % LEVEL_SCORE == 0)
        {
            levelUp();
        }
        spawnMole();
    }
    else
    {
        buzzerMiss();
        life--;
        if (life <= 0)
        {
            gameOver();
        }
    }
}

void gameUpdate()
{
    uint8_t button;
    while (xQueueReceive(buttonQueue, &button, 0) == pdTRUE)
    {
        handleButton(button);
    }

    if (gameState != GAME_PLAYING)
        return;

    // hết giờ game
    if (millis() - gameStartTime >= GAME_TIME_MS)
    {
        gameOver();
        return;
    }

    // mole hết thời gian -> trừ mạng
    if (millis() - ledStartTime >= ledDuration)
    {
        buzzerTimeout();
        life--;
        if (life <= 0)
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
