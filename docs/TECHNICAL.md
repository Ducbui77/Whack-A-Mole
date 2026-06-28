# Tài liệu kỹ thuật — Whack-A-Mole (ESP32)

Trò chơi "đập chuột chũi" chạy trên ESP32: 6 LED là các "mole", 6 nút bấm tương ứng,
phản hồi bằng buzzer và màn hình LCD I2C. Toàn bộ logic chạy trên FreeRTOS với
3 task giao tiếp qua hàng đợi (queue).

---

## 1. Phần cứng

### Sơ đồ chân (GPIO)

| Khối | Thành phần | GPIO |
|------|-----------|------|
| Nút bấm | BTN1..BTN6 | 13, 14, 27, 26, 25, 33 |
| LED | LED1..LED6 | 4, 16, 17, 18, 19, 21 |
| Buzzer | BUZZER | 23 |
| LCD I2C | SDA / SCL | 32 / 22 |

- **Nút bấm**: nối theo kiểu kéo lên nội bộ (`INPUT_PULLUP`); nhấn = mức `LOW`.
- **LED**: tích cực mức cao (`HIGH` = sáng), có điện trở hạn dòng.
- **Buzzer**: buzzer chủ động (active), bật/tắt bằng mức logic.
- **LCD**: 16x2, địa chỉ I2C `0x27`.
- Tất cả GND nối chung.

> Ánh xạ chân khai báo tập trung trong [`pins.h`](../pins.h).

---

## 2. Kiến trúc phần mềm

Mã nguồn chia 3 tầng rõ ràng:

```
                ┌───────────────────────────┐
                │      WhackAMole.ino        │  setup(): init + tạo task
                └──────────────┬────────────┘
                               │
        ┌──────────────┬───────┴───────┬──────────────┐
        │              │               │              │
   ┌────────┐    ┌──────────┐    ┌───────────┐        │   Tầng TASK
   │taskInput│    │ taskGame │    │ soundTask │        │  (FreeRTOS)
   └────┬───┘    └────┬─────┘    └─────┬─────┘        │
        │ buttonQueue  │                │ soundQueue   │
        └──────────────▼                ▼              │
                  ┌──────────────────────────┐         │   Tầng LOGIC
                  │      game.cpp (engine)    │─────────┘
                  └─────┬──────┬──────┬───────┘
                        │      │      │
                   ┌────▼─┐ ┌──▼───┐ ┌▼──────┐               Tầng DRIVER
                   │ led  │ │display│ │buzzer │  + button
                   └──────┘ └──────┘ └───────┘
```

| Tầng | File | Trách nhiệm |
|------|------|-------------|
| Entry | `WhackAMole.ino` | Khởi tạo driver, tạo 3 task, pin vào core 1 |
| Task | `taskInput`, `taskGame`, `soundTask` | Vòng lặp đa nhiệm |
| Logic | `game.cpp` / `game.h` | Máy trạng thái, luật chơi, điểm, level, high score |
| Driver | `led`, `button`, `buzzer`, `display` | Trừu tượng hóa phần cứng |
| Cấu hình | `config.h`, `pins.h` | Hằng số game & ánh xạ chân |

---

## 3. Mô hình đa nhiệm (FreeRTOS)

Ba task, tất cả pin vào **core 1** (core 0 dành cho WiFi/BT stack của ESP32):

| Task | Ưu tiên | Chu kỳ | Stack | Vai trò |
|------|---------|--------|-------|---------|
| `taskInput` | 2 (cao) | `INPUT_TASK_DELAY` = 10 ms | 2048 | Quét nút, debounce, đẩy nút vào `buttonQueue` |
| `taskGame` | 1 | `GAME_TASK_DELAY` = 20 ms | 4096 | Đọc `buttonQueue`, chạy `gameUpdate()` |
| `soundTask` | 1 | chờ sự kiện (block) | 2048 | Đọc `soundQueue`, phát buzzer |

### Vì sao dùng queue?

Trước đây `taskInput` gọi thẳng hàm logic, còn `taskGame` cũng sửa cùng các biến
(`score`, `life`, mole...) → **race condition** vì không có khóa.

Giải pháp: **dồn toàn bộ trạng thái về một mình `taskGame`**.

- `taskInput` chỉ làm một việc: `gamePostButton(id)` → `xQueueSend(buttonQueue, ...)`.
- `gameUpdate()` (trong `taskGame`) đầu mỗi vòng sẽ rút sạch queue và xử lý tuần tự.
- Nhờ vậy không cần `mutex`/`volatile`, trạng thái luôn nhất quán.

Tương tự, `soundQueue` cô lập các `delay()` của buzzer sang `soundTask`, để
đồng hồ game trong `taskGame` không bị "đứng" khi đang phát âm thanh.
`soundTask` dùng `xQueueReceive(..., portMAX_DELAY)` nên ngủ hoàn toàn khi im lặng,
không tốn CPU.

```
taskInput ──(uint8_t id)──► [buttonQueue] ──► taskGame.gameUpdate()
                                                     │
                                                     └──(SoundEffect)──► [soundQueue] ──► soundTask
```

---

## 4. Máy trạng thái game

```
        ┌──────────┐  nhấn nút bất kì   ┌──────────────┐
        │ WAITING  │ ─────────────────► │  COUNTDOWN   │
        │(welcome) │                    │  3 → 2 → 1   │
        └──────────┘                    └──────┬───────┘
              ▲                                │ đếm xong
              │                                ▼
              │                         ┌──────────────┐
              │   nhấn nút bất kì       │   PLAYING    │
        ┌──────────┐ ◄──────────────── │ (đang chơi)  │
        │GAME_OVER │                    └──────┬───────┘
        └──────────┘ ◄─────────────────────────┘
                       hết giờ / hết mạng
```

| Trạng thái | Mô tả | Thoát khi |
|-----------|-------|-----------|
| `GAME_WAITING` | Màn welcome + high score | Nhấn nút bất kì → COUNTDOWN |
| `GAME_COUNTDOWN` | Đếm ngược `COUNTDOWN_SEC` giây, có tiếng tick | Hết đếm → PLAYING |
| `GAME_PLAYING` | Vòng chơi chính | Hết giờ hoặc `life <= 0` → GAME_OVER |
| `GAME_OVER` | Hiện điểm + high score | Nhấn nút bất kì → COUNTDOWN |

### Luật chơi (trong `gameUpdate` / `handleButton`)

- **Đập trúng** (nút == mole đang sáng): `score++`, tắt mole đó, sinh mole mới.
  Cứ mỗi `LEVEL_SCORE` (10) điểm thì lên level.
- **Đập sai** (nút không có mole): `life--`.
- **Mole hết giờ** (sáng quá `ledDuration`): `life--`, mole biến mất, sinh mole mới.
- **Lên level**: giảm `ledDuration` đi `LED_TIME_STEP` (mole xuất hiện nhanh hơn),
  chặn dưới ở `LED_TIME_MIN`.
- **Đa mole**: từ `MOLE_LEVEL_2` (level 3) trở lên có tối đa `MOLE_MAX` (2) mole
  cùng lúc; mỗi mole có bộ đếm thời gian riêng (`moleStart[i]`).

---

## 5. Chi tiết các module

### `game` — engine
| Hàm | Mô tả |
|-----|-------|
| `gameInit()` | Tạo `buttonQueue`, mở NVS, nạp high score, gọi `gameReset()` |
| `gameReset()` | Đưa về `WAITING`, hiện welcome |
| `gameUpdate()` | Gọi định kỳ từ `taskGame`: xử lý input + cập nhật trạng thái |
| `gamePostButton(id)` | Đẩy nút vào queue (gọi từ `taskInput`) |

Trạng thái nội bộ (đều là `static`, chỉ `taskGame` chạm tới): `score`, `level`,
`life`, `highScore`, `moleOn[]`, `moleStart[]`, `ledDuration`, `gameStartTime`...

### `button` — đọc nút + debounce
`buttonRead()` quét 6 nút, áp dụng debounce `DEBOUNCE_TIME` (30 ms) theo thuật toán
"chờ tín hiệu ổn định", chỉ trả về id **tại thời điểm nhấn xuống** (cạnh xuống),
trả `-1` nếu không có nút nào.

### `led` — driver LED
`ledInit()` (kèm `randomSeed`), `ledOn(id)`, `ledOff(id)`, `ledAllOff()`.
Mọi hàm kiểm tra biên `id >= NUM_LED`.

### `buzzer` — âm thanh bất đồng bộ
`buzzerInit()` tạo `soundQueue`. `soundPlay(effect)` đẩy hiệu ứng vào queue (không chặn).
`soundTask()` phát âm thanh. Các hiệu ứng: `SND_START`, `SND_HIT`, `SND_MISS`,
`SND_TIMEOUT`, `SND_GAMEOVER`, `SND_TICK`.

### `display` — LCD I2C
Vẽ có **tối ưu**: ở màn chơi, mỗi trường (điểm/level/mạng/thời gian) chỉ ghi lại LCD
khi giá trị đổi (so với `oldScore`, `oldLevel`...) → tránh nhấp nháy.
`displayShowWelcome(hi)`, `displayShowCountdown(sec)`, `displayShowGame(...)`,
`displayShowGameOver(score, hi)`, `displayClear()`.

---

## 6. Cấu hình (`config.h`)

| Hằng số | Giá trị | Ý nghĩa |
|---------|---------|---------|
| `NUM_BUTTON` / `NUM_LED` | 6 | Số nút / LED |
| `GAME_TIME_MS` | 60000 | Thời lượng một ván (ms) |
| `PLAYER_LIFE` | 3 | Số mạng |
| `LED_TIME_START` | 2000 | Thời gian mole sáng ban đầu (ms) |
| `LED_TIME_MIN` | 700 | Giới hạn dưới khi lên level |
| `LED_TIME_STEP` | 200 | Giảm mỗi level (ms) |
| `LEVEL_SCORE` | 10 | Số điểm để lên 1 level |
| `COUNTDOWN_SEC` | 3 | Giây đếm ngược trước khi chơi |
| `MOLE_LEVEL_2` | 3 | Level bắt đầu có 2 mole |
| `MOLE_MAX` | 2 | Số mole tối đa cùng lúc |
| `DEBOUNCE_TIME` | 30 | Thời gian debounce nút (ms) |
| `*_TASK_DELAY` | 10/20/100 | Chu kỳ task (ms) |
| `LCD_ADDRESS/COL/ROW` | 0x27/16/2 | Cấu hình LCD |

---

## 7. Lưu high score (NVS)

Dùng thư viện `Preferences` của ESP32 (lưu vào flash NVS, không mất khi tắt nguồn):

- Namespace: `"wam"`, key: `"hi"`.
- Nạp ở `gameInit()`; ghi ở `gameOver()` khi `score > highScore`.

---

## 8. Build & nạp

**Yêu cầu:**
- Arduino core cho ESP32 (kèm sẵn FreeRTOS và `Preferences`).
- Thư viện `LiquidCrystal_I2C`.

**Bằng Arduino IDE:** mở `WhackAMole.ino`, chọn board ESP32 phù hợp, nạp.

**Bằng `arduino-cli`:**
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload  --fqbn esp32:esp32:esp32 -p <PORT> .
```

---

## 9. Hướng phát triển

- **Hiệu ứng LED**: dùng PWM (`ledc`) cho fade thay vì bật/tắt.
- **Nhiều nốt nhạc**: thay buzzer chủ động bằng `tone()` để có giai điệu.
- **Mở rộng đa mole**: tăng `MOLE_MAX`, hoặc tăng dần theo level.
- **Kết nối**: gửi điểm qua WiFi/BLE, làm bảng xếp hạng online.
- **Chọn độ khó**: giữ nút ở màn welcome để chọn Easy/Hard (đổi `LED_TIME_START`).
