# Whack-A-Mole

Kiến trúc mạch
ESP32

BTN1 -> GPIO13
BTN2 -> GPIO14
BTN3 -> GPIO27
BTN4 -> GPIO26
BTN5 -> GPIO25
BTN6 -> GPIO33

LED1 -> GPIO4
LED2 -> GPIO16
LED3 -> GPIO17
LED4 -> GPIO18
LED5 -> GPIO19
LED6 -> GPIO21

BUZZER -> GPIO23

LCD:
SDA -> GPIO32
SCL -> GPIO22

Tất cả GND nối chung


Luồng Hoạt Động


               +----------------------+
               |     WhackAMole.ino   |
               +----------+-----------+
                          |
                     Khởi tạo Driver
                          |
        +-----------------+------------------+
        |                 |                  |
     TaskInput         TaskGame         (loop rỗng)
        |                 |
        |                 |
 buttonRead()         gameUpdate()
        |                 |
        +--------+--------+
                 |
         Game Engine (game.cpp)
                 |
   +------+------+------+------+
   |             |             |
 LED Driver   LCD Driver   Buzzer Driver


 Cấu trúc file
WhackAMole/
│
├── WhackAMole.ino        // Điểm khởi đầu của chương trình: khởi tạo toàn bộ hệ thống và tạo các FreeRTOS Task.
│
├── pins.h                // Khai báo ánh xạ GPIO của ESP32 (nút bấm, LED, buzzer, LCD).
├── config.h              // Chứa các hằng số cấu hình của trò chơi (thời gian, số mạng, level, chu kỳ Task,...).
│
├── led.h                 // Khai báo API điều khiển LED.
├── led.cpp               // Driver điều khiển 6 LED (bật, tắt, nhấp nháy, chọn LED ngẫu nhiên).
│
├── button.h              // Khai báo API đọc trạng thái nút bấm.
├── button.cpp            // Driver đọc 6 nút bấm, xử lý debounce và phát hiện sự kiện nhấn.
│
├── buzzer.h              // Khai báo API điều khiển buzzer.
├── buzzer.cpp            // Driver phát các hiệu ứng âm thanh (bắt đầu game, đập đúng, đập sai, Game Over,...).
│
├── display.h             // Khai báo API hiển thị LCD.
├── display.cpp           // Driver điều khiển LCD I2C (màn hình chờ, điểm số, level, thời gian, Game Over,...).
│
├── game.h                // Khai báo Game Engine và các trạng thái của trò chơi.
├── game.cpp              // Bộ não của trò chơi: quản lý luật chơi, điểm số, mạng, level, thời gian, sinh chuột và xử lý Game Over.
│
├── taskGame.h            // Khai báo Task Game.
├── taskGame.cpp          // FreeRTOS Task chạy vòng lặp game, cập nhật trạng thái trò chơi theo chu kỳ.
│
├── taskInput.h           // Khai báo Task Input.
└── taskInput.cpp         // FreeRTOS Task đọc nút bấm và gửi sự kiện nhấn tới Game Engine.