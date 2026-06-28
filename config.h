#ifndef CONFIG_H
#define CONFIG_H

/*cài đặt game*/

// số lượng nút và LED

#define NUM_BUTTON         6
#define NUM_LED            6

// thời gian game

#define GAME_TIME_MS       60000

// số mạng

#define PLAYER_LIFE        3

// thời gian LED sáng ban đầu

#define LED_TIME_START     2000

// thời gian LED nhỏ nhất

#define LED_TIME_MIN       700

// giảm 200 ms mỗi level

#define LED_TIME_STEP      200

// cứ 10 điểm thì lên level

#define LEVEL_SCORE        10

// số giây đếm ngược trước khi vào game

#define COUNTDOWN_SEC        3

// từ level này trở lên có 2 mole cùng lúc

#define MOLE_LEVEL_2         3

// số mole tối đa cùng lúc

#define MOLE_MAX             2

/*task period */

#define INPUT_TASK_DELAY     10

#define GAME_TASK_DELAY      20

#define DISPLAY_TASK_DELAY  100

#define SOUND_TASK_DELAY     10

/*button*/

#define DEBOUNCE_TIME        30

/*lcd*/

#define LCD_ADDRESS        0x27

#define LCD_COL             16

#define LCD_ROW              2

#endif