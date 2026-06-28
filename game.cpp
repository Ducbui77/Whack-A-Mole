#include "game.h"

#include "config.h"
#include "led.h"
#include "display.h"
#include "buzzer.h"

#include <Preferences.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static GameState gameState;
static int score;
static int level;
static int life;
static int highScore;

// Trạng thái từng mole: bật/tắt và thời điểm xuất hiện (để tính hết giờ riêng).
static bool moleOn[NUM_LED];
static unsigned long moleStart[NUM_LED];
static unsigned long ledDuration;

static unsigned long gameStartTime;

static unsigned long countdownStartTime;
static int lastCountdownSec;

// Hàng đợi nút bấm: taskInput ghi vào, gameUpdate (taskGame) đọc ra,
// nhờ vậy toàn bộ trạng thái game chỉ do một task chạm tới.
static QueueHandle_t buttonQueue;

static Preferences prefs;

/*PRIVATE FUNCTION*/
static void startCountdown();
static void gameStart();
static void handleButton(uint8_t button);
static int  targetMoleCount();
static int  activeMoleCount();
static void spawnMole();
static void ensureMoles();
static void clearMoles();
static void levelUp();
static void gameOver();
static int  gameRemainTime();

void gameInit()
{
    buttonQueue = xQueueCreate(8, sizeof(uint8_t));

    prefs.begin("wam", false);
    highScore = prefs.getInt("hi", 0);

    gameReset();
}

void gameReset()
{
    gameState = GAME_WAITING;
    score = 0;
    level = 1;
    life = PLAYER_LIFE;
    ledDuration = LED_TIME_START;
    clearMoles();
    displayShowWelcome(highScore);
}

void gamePostButton(uint8_t button)
{
    xQueueSend(buttonQueue, &button, 0);
}

static void startCountdown()
{
    gameState = GAME_COUNTDOWN;
    countdownStartTime = millis();
    lastCountdownSec = -1;
    clearMoles();
}

static void gameStart()
{
    score = 0;
    level = 1;
    life = PLAYER_LIFE;
    ledDuration = LED_TIME_START;
    gameStartTime = millis();
    gameState = GAME_PLAYING;

    displayClear();
    displayShowGame(
        score,
        level,
        life,
        GAME_TIME_MS / 1000);

    soundPlay(SND_START);

    clearMoles();
    ensureMoles();
}

static int targetMoleCount()
{
    return (level >= MOLE_LEVEL_2) ? MOLE_MAX : 1;
}

static int activeMoleCount()
{
    int count = 0;
    for (int i = 0; i < NUM_LED; i++)
    {
        if (moleOn[i])
            count++;
    }
    return count;
}

static void spawnMole()
{
    int id;
    do
    {
        id = random(NUM_LED);
    } while (moleOn[id]);

    moleOn[id] = true;
    moleStart[id] = millis();
    ledOn(id);
}

static void ensureMoles()
{
    int target = targetMoleCount();
    while (activeMoleCount() < target)
    {
        spawnMole();
    }
}

static void clearMoles()
{
    for (int i = 0; i < NUM_LED; i++)
    {
        moleOn[i] = false;
    }
    ledAllOff();
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
    clearMoles();

    if (score > highScore)
    {
        highScore = score;
        prefs.putInt("hi", highScore);
    }

    soundPlay(SND_GAMEOVER);
    displayShowGameOver(score, highScore);
}

static int gameRemainTime()
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
    if (gameState == GAME_PLAYING)
    {
        if (moleOn[button])
        {
            soundPlay(SND_HIT);
            ledOff(button);
            moleOn[button] = false;
            score++;
            if (score % LEVEL_SCORE == 0)
            {
                levelUp();
            }
            ensureMoles();
        }
        else
        {
            soundPlay(SND_MISS);
            life--;
            if (life <= 0)
            {
                gameOver();
            }
        }
    }
    else if (gameState == GAME_WAITING || gameState == GAME_OVER)
    {
        // Nhấn nút bất kì để bắt đầu ván mới
        startCountdown();
        // Bỏ các nút còn tồn trong hàng đợi để không bị tính ngay vào ván mới
        xQueueReset(buttonQueue);
    }
    // GAME_COUNTDOWN: bỏ qua, chờ đếm xong
}

void gameUpdate()
{
    uint8_t button;
    while (xQueueReceive(buttonQueue, &button, 0) == pdTRUE)
    {
        handleButton(button);
    }

    if (gameState == GAME_COUNTDOWN)
    {
        int sec = COUNTDOWN_SEC - (int)((millis() - countdownStartTime) / 1000);
        if (sec <= 0)
        {
            gameStart();
        }
        else if (sec != lastCountdownSec)
        {
            displayShowCountdown(sec);
            soundPlay(SND_TICK);
            lastCountdownSec = sec;
        }
        return;
    }

    if (gameState != GAME_PLAYING)
        return;

    // hết giờ game
    if (millis() - gameStartTime >= GAME_TIME_MS)
    {
        gameOver();
        return;
    }

    // mole nào hết thời gian -> trừ mạng
    for (int i = 0; i < NUM_LED; i++)
    {
        if (moleOn[i] && (millis() - moleStart[i] >= ledDuration))
        {
            soundPlay(SND_TIMEOUT);
            ledOff(i);
            moleOn[i] = false;
            life--;
            if (life <= 0)
            {
                gameOver();
                return;
            }
        }
    }
    ensureMoles();

    displayShowGame(
        score,
        level,
        life,
        gameRemainTime());
}
