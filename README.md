# Whack-A-Mole

Trò chơi "đập chuột chũi" trên ESP32 (6 LED + 6 nút + buzzer + LCD I2C), chạy trên
FreeRTOS với 3 task giao tiếp qua hàng đợi.

## Tính năng

- 6 mole (LED) với độ khó tăng dần theo level (mole sáng nhanh hơn).
- Đếm ngược 3-2-1 trước khi vào game.
- Đa mole: từ level 3 có 2 mole sáng cùng lúc.
- Buzzer bất đồng bộ (task riêng) — không làm trễ vòng game.
- Lưu điểm cao nhất vào flash (NVS), còn sau khi tắt nguồn.
- Hệ thống mạng/điểm/level/đồng hồ hiển thị trên LCD 16x2.

> 📄 Chi tiết kiến trúc, mô hình đa nhiệm và API: [docs/TECHNICAL.md](docs/TECHNICAL.md)

## Kiến trúc mạch ESP32

**Buttons:**
- BTN1 -> GPIO13
- BTN2 -> GPIO14
- BTN3 -> GPIO27
- BTN4 -> GPIO26
- BTN5 -> GPIO25
- BTN6 -> GPIO33

**LEDs:**
- LED1 -> GPIO4
- LED2 -> GPIO16
- LED3 -> GPIO17
- LED4 -> GPIO18
- LED5 -> GPIO19
- LED6 -> GPIO21

**Buzzer:** GPIO23

**LCD:** SDA -> GPIO32 | SCL -> GPIO22

> Tất cả GND nối chung


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


## Cấu trúc file

```
WhackAMole/
│
├── WhackAMole.ino        // Điểm khởi đầu của chương trình
│
├── pins.h                // Khai báo ánh xạ GPIO
├── config.h              // Các hằng số cấu hình
│
├── led.h / led.cpp       // Driver điều khiển 6 LED
├── button.h / button.cpp // Driver đọc 6 nút bấm, debounce
├── buzzer.h / buzzer.cpp // Driver phát hiệu ứng âm thanh
├── display.h / display.cpp // Driver LCD I2C
├── game.h / game.cpp     // Game Engine: luật chơi, điểm, level
├── taskGame.h / taskGame.cpp // FreeRTOS Task vòng lặp game
└── taskInput.h / taskInput.cpp // FreeRTOS Task đọc nút bấm
```