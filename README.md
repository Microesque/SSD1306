# SSD1306 - Microesque
Platform independent C library for monochrome OLEDs based on SSD1306 driver.

Setup guide includes `STM32 (HAL Library)` and `PIC16/18 (MCC Library)` examples.

---

### Features:
- Library with only one `.c/.h` pair.
- Library with only static memory allocations.
- Requires minimal setup with a single callback function.
- Supports unlimited number of displays.
- Provides all hardware commands.
- Includes graphical functions (circle/line/rectangle/etc.).
- Includes extra buffer functions (shift/rotate/mirror/etc.).
- Includes character drawing (printf and lower memory alternatives).
- Includes custom characters and image drawing.

---

### Limitations:
- Only supports communication via I2C.
- Only supports 128x64 and 128x32 displays.
- No included fonts; supports [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) font format.

---

[Full Setup Guide & Documentation](https://github.com/Microesque/SSD1306/wiki)

---

<div style="text-align: left;">
<img src="https://raw.githubusercontent.com/wiki/Microesque/SSD1306/images/Home/Intro.gif" alt="img" width="750">
</div>