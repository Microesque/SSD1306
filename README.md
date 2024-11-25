# SSD1306 - Microesque
Platform independent C library for monochrome OLEDs based on SSD1306 driver.

---

### Features:
- Library with only one `.c/.h` pair.
- Requires minimal setup with a single callback function.
- Uses static only memory allocation.
- Supports unlimited number of displays.
- Provides all hardware commands.
- Provides graphical functions (circle/line/rectangle/etc.).
- Provides extra buffer functions (shift/rotate/mirror/etc.).
- Provides character drawing (printf and lower memory alternatives).
- Provides custom characters and image drawing.

---

### Limitations:
- Only supports communication via I2C.
- Only supports 128x64 and 128x32 displays.
- No included fonts; supports [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) font format.

---

### Library Setup:
- Download or clone the repository.
- Navigate to the `lib` folder.
- Copy the `ssd1306.c` and `ssd1306.h` into your project.
- Update your include paths (if necessary).
- Include the `ssd1306.h` in your source file.
- Set up your display by following the [Display Setup]().
- Use the provided [library functions]() to test your display, or **optionally** use the [demo]() or [test]() animations.

---

[Full Setup Guide & Documentation](https://github.com/Microesque/SSD1306/wiki)

---
