/**
 * MIT License
 *
 * Copyright (c) 2024 Ahmet Burak Irmak (https://youtube.com/Microesque)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * Library GitHub Page:   https://github.com/Microesque/SSD1306
 * Library Documentation: https://github.com/Microesque/SSD1306/wiki
 */

/*----------------------------------------------------------------------------*/
/*---------------------------- Necessary Libraries ---------------------------*/
/*----------------------------------------------------------------------------*/

#include "ssd1306_test.h"

/*----------------------------------------------------------------------------*/
/*----------------------------- Helper Functions -----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Arbitrary delay for the animations to use.
 *
 * @param delay Arbitrary delay value that slows down the animation.
 */
static void h_delay(uint16_t delay) {
    for (volatile uint16_t i = 0; i < delay; i++) {
        for (volatile uint8_t i = 0; i < 255; i++) {
        }
    }
}

/**
 * @brief Template that draws the Microesque logo for the animations to use.
 *
 * @note Clears and updates as well.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
static void h_draw_logo(struct ssd1306_display *display) {
    /* Microesque logo (63x11px) */
    const static uint8_t LOGO_BITMAP_WIDTH = 109;
    const static uint8_t LOGO_BITMAP_HEIGHT = 23;
    const static uint8_t LOGO_BITMAP[] = {
        0x1f, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x1f, 0x07, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x1f, 0x03, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x01, 0x00, 0x00, 0xf0, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x01, 0x00, 0x00, 0xf0,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00,
        0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f,
        0x00, 0x38, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x1f, 0x00, 0x78, 0x00, 0xe0, 0xdf, 0xf7, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x1f, 0x00, 0xf8, 0x00, 0xe0, 0x9f, 0xf3, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0xf8, 0x01, 0xe0, 0x5f, 0x75,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0xf8, 0x03, 0xe0,
        0xdf, 0xf6, 0x0f, 0x8b, 0xe3, 0xf1, 0x70, 0xd8, 0x1d, 0x1f, 0x00, 0xf8,
        0x03, 0xe0, 0xdf, 0x77, 0xf7, 0xf3, 0xdd, 0x6e, 0xbf, 0xdb, 0xed, 0x1e,
        0x00, 0xf8, 0x01, 0xe0, 0xdf, 0x77, 0xf7, 0xfb, 0xdd, 0xe0, 0xb8, 0xdb,
        0x0d, 0x1e, 0x00, 0xf8, 0x00, 0xe0, 0xdf, 0x77, 0xf7, 0xfb, 0xdd, 0xfe,
        0x77, 0xd8, 0xed, 0x1f, 0x00, 0x78, 0x00, 0xe0, 0xdf, 0x77, 0xf7, 0xfb,
        0xdd, 0x7e, 0xf7, 0xdb, 0xed, 0x1f, 0x00, 0x38, 0x00, 0xe0, 0xdf, 0x77,
        0x0f, 0xfb, 0xe3, 0xe1, 0xf8, 0x3b, 0x1e, 0x1e, 0x00, 0x00, 0x00, 0xe0,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0x1f, 0x01, 0x00,
        0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f,
        0x01, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x10, 0x03, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x1f, 0x07, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0x00, 0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f};

    ssd1306_draw_clear(display);
    ssd1306_draw_bitmap(display, 9, 4, LOGO_BITMAP, LOGO_BITMAP_WIDTH,
                        LOGO_BITMAP_HEIGHT, false);
    ssd1306_display_update(display);
}

/*----------------------------------------------------------------------------*/
/*------------------------------ Test Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Tests for:
 *
 * - ssd1306_reinit()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation.
 * Recommended starting value is 6000.
 */
void ssd1306_test_reinit(struct ssd1306_display *display, uint16_t delay) {
    while (1) {
        h_draw_logo(display);
        h_delay(delay);

        ssd1306_reinit(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_display_brightness()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 90.
 */
void ssd1306_test_brightness(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    uint8_t i = 0;
    uint8_t dir = 1;
    while (1) {
        if (dir)
            i++;
        else
            i--;

        if (i == 255 || i == 0)
            dir ^= 1;

        ssd1306_display_brightness(display, i);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_display_enable()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_enable(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_display_enable(display, true);
        h_delay(delay);
        ssd1306_display_enable(display, false);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_display_fully_on()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_fully_on(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_display_fully_on(display, true);
        h_delay(delay);
        ssd1306_display_fully_on(display, false);
        h_delay(delay);
    }
}

/**
 * @brief Tests
 *
 * - ssd1306_display_inverse()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_inverse(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_display_inverse(display, true);
        h_delay(delay);
        ssd1306_display_inverse(display, false);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_display_mirror_v()
 *
 * - ssd1306_display_mirror_h()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_mirrors(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_display_mirror_h(display, true);
        h_delay(delay);
        ssd1306_display_mirror_h(display, false);
        h_delay(delay);

        ssd1306_display_mirror_v(display, true);
        h_delay(delay);
        ssd1306_display_mirror_v(display, false);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_display_scroll_enable()
 *
 * - ssd1306_display_scroll_disable()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 20000.
 */
void ssd1306_test_scroll_enable_disable(struct ssd1306_display *display,
                                        uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_display_scroll_enable(display, false, false, 7);
        h_delay(delay);

        ssd1306_display_scroll_enable(display, true, false, 7);
        h_delay(delay);

        ssd1306_display_scroll_enable(display, false, true, 7);
        h_delay(delay);

        ssd1306_display_scroll_enable(display, true, true, 7);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_clear()
 *
 * - ssd1306_draw_fill()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_draw_clear_fill(struct ssd1306_display *display,
                                  uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_draw_clear(display);
        ssd1306_display_update(display);
        h_delay(delay);

        ssd1306_draw_fill(display);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_invert()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_draw_invert(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        ssd1306_draw_invert(display);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_mirror_h()
 *
 * - ssd1306_draw_mirror_v()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_draw_mirrors(struct ssd1306_display *display,
                               uint16_t delay) {
    h_draw_logo(display);

    while (1) {
        for (uint8_t i = 0; i < 4; i++) {
            if (i < 2) {
                ssd1306_draw_mirror_h(display);
            } else {
                ssd1306_draw_mirror_v(display);
            }
            ssd1306_display_update(display);
            h_delay(delay);
        }
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_shift_right()
 *
 * - ssd1306_draw_shift_left()
 *
 * - ssd1306_draw_shift_up()
 *
 * - ssd1306_draw_shift_down()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 0.
 */
void ssd1306_test_draw_shifts(struct ssd1306_display *display, uint16_t delay) {
    h_draw_logo(display);

    bool enable_rotation = false;
    const uint8_t shift_count = 60;
    while (1) {
        enable_rotation ^= 1;

        ssd1306_draw_clear(display);
        h_draw_logo(display);
        for (uint8_t i = 0; i < 4; i++) {
            if (!enable_rotation) {
                ssd1306_draw_clear(display);
                h_draw_logo(display);
            }

            for (uint8_t j = 0; j < shift_count; j++) {
                if (i == 0) {
                    ssd1306_draw_shift_left(display, enable_rotation);
                    ssd1306_draw_shift_left(display, enable_rotation);
                } else if (i == 1) {
                    ssd1306_draw_shift_right(display, enable_rotation);
                    ssd1306_draw_shift_right(display, enable_rotation);
                } else if (i == 2) {
                    ssd1306_draw_shift_up(display, enable_rotation);
                    ssd1306_draw_shift_up(display, enable_rotation);
                } else if (i == 3) {
                    ssd1306_draw_shift_down(display, enable_rotation);
                    ssd1306_draw_shift_down(display, enable_rotation);
                }
                ssd1306_display_update(display);
                h_delay(delay);
            }
        }
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_line_h()
 *
 * - ssd1306_draw_line_v()
 *
 * - ssd1306_draw_line()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 3000.
 */
void ssd1306_test_draw_lines(struct ssd1306_display *display, uint16_t delay) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    uint8_t middle_x = (uint8_t)(SSD1306_X_MAX >> 1);
    uint8_t middle_y = (uint8_t)(SSD1306_Y_MAX >> 1);
    while (1) {
        ssd1306_draw_clear(display);

        for (uint8_t i = 0; i < 9; i++) {
            if (i == 0) {
                ssd1306_draw_clear(display);
            } else if (i == 1) {
                ssd1306_draw_line_h(display, 0, 0, SSD1306_X_MAX + 1);
            } else if (i == 2) {
                ssd1306_draw_line_v(display, SSD1306_X_MAX, 0,
                                    SSD1306_Y_MAX + 1);
            } else if (i == 3) {
                ssd1306_draw_line_h(display, SSD1306_X_MAX, SSD1306_Y_MAX,
                                    -SSD1306_X_MAX);
            } else if (i == 4) {
                ssd1306_draw_line_v(display, 0, SSD1306_Y_MAX, -SSD1306_Y_MAX);
            } else if (i == 5) {
                ssd1306_draw_line(display, middle_x, middle_y, 0, 0);
            } else if (i == 6) {
                ssd1306_draw_line(display, middle_x, middle_y, SSD1306_X_MAX,
                                  0);
            } else if (i == 7) {
                ssd1306_draw_line(display, middle_x, middle_y, SSD1306_X_MAX,
                                  SSD1306_Y_MAX);
            } else if (i == 8) {
                ssd1306_draw_line(display, middle_x, middle_y, 0,
                                  SSD1306_Y_MAX);
            }
            ssd1306_display_update(display);
            h_delay(delay);
        }
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_triangle()
 *
 * - ssd1306_draw_triangle_fill()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 0.
 */
void ssd1306_test_draw_triangles(struct ssd1306_display *display,
                                 uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    int16_t x0 = SSD1306_X_MAX >> 1;
    int16_t y0 = 0;
    int16_t x1 = SSD1306_X_MAX >> 1;
    int16_t y1 = SSD1306_Y_MAX;
    int16_t x2 = 0;
    int16_t y2 = SSD1306_Y_MAX >> 1;
    bool is_filled = true;
    enum direction dir = POSITIVE;
    while (1) {
        if (dir == POSITIVE)
            x2++;
        else
            x2--;

        if (x2 == 0) {
            dir = POSITIVE;
        } else if (x2 == SSD1306_X_MAX) {
            dir = NEGATIVE;
            is_filled ^= 1;
        }

        ssd1306_draw_clear(display);
        if (is_filled)
            ssd1306_draw_triangle_fill(display, x0, y0, x1, y1, x2, y2);
        else
            ssd1306_draw_triangle(display, x0, y0, x1, y1, x2, y2);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_rect()
 *
 * - ssd1306_draw_rect_fill()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 0.
 */
void ssd1306_test_draw_rects(struct ssd1306_display *display, uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    int16_t x0 = SSD1306_X_MAX >> 1;
    int16_t y0 = 5;
    int16_t width = 0;
    int16_t height = SSD1306_Y_MAX - y0 - y0 + 1;
    int16_t width_max = (SSD1306_X_MAX >> 1) + 1;
    bool is_filled = true;
    enum direction dir = NEGATIVE;
    while (1) {
        if (dir == POSITIVE)
            width++;
        else
            width--;

        if (width == -width_max) {
            dir = POSITIVE;
        } else if (width == width_max) {
            dir = NEGATIVE;
            is_filled ^= 1;
        }

        ssd1306_draw_clear(display);
        if (is_filled)
            ssd1306_draw_rect_fill(display, x0, y0, width, height);
        else
            ssd1306_draw_rect(display, x0, y0, width, height);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_rect_round()
 *
 * - ssd1306_draw_rect_round_fill()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 0.
 */
void ssd1306_test_draw_rect_rounds(struct ssd1306_display *display,
                                   uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    int16_t x0 = SSD1306_X_MAX >> 1;
    int16_t y0 = 5;
    int16_t width = 0;
    int16_t height = SSD1306_Y_MAX - y0 - y0 + 1;
    int16_t width_max = (SSD1306_X_MAX >> 1) + 1;
    int16_t r = 999;
    bool is_filled = true;
    enum direction dir = NEGATIVE;
    while (1) {
        if (dir == POSITIVE)
            width++;
        else
            width--;

        if (width == -width_max) {
            dir = POSITIVE;
            r = -r;
        } else if (width == width_max) {
            dir = NEGATIVE;
            is_filled ^= 1;
            r = -r;
        }

        ssd1306_draw_clear(display);
        if (is_filled)
            ssd1306_draw_rect_round_fill(display, x0, y0, width, height, r);
        else
            ssd1306_draw_rect_round(display, x0, y0, width, height, r);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_draw_arc()
 *
 * - ssd1306_draw_arc_fill()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 2500.
 */
void ssd1306_test_draw_arcs(struct ssd1306_display *display, uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    int16_t x0 = SSD1306_X_MAX >> 1;
    int16_t y0 = SSD1306_Y_MAX >> 1;
    int16_t r = 0;
    int16_t r_max = SSD1306_Y_MAX >> 1;
    uint8_t quadrant = 1;
    bool is_filled = false;
    ssd1306_draw_clear(display);
    while (1) {
        if (r > r_max) {
            r = 0;
            is_filled ^= 1;
            quadrant = 1;
        }
        r += 2;

        ssd1306_draw_clear(display);
        if (is_filled)
            ssd1306_draw_arc_fill(display, x0, y0, r, quadrant);
        else
            ssd1306_draw_arc(display, x0, y0, r, quadrant);
        ssd1306_display_update(display);
        h_delay(delay);

        if (quadrant == 0b1000)
            quadrant = 1;
        else
            quadrant <<= 1;
    }
}

/**
 * @brief Tests for (requires font to be set up, ideally < 10px):
 *
 * - ssd1306_draw_char()
 *
 * - ssd1306_draw_char_custom()
 *
 * - ssd1306_draw_str()
 *
 * - ssd1306_draw_int32()
 *
 * - ssd1306_draw_float()
 *
 * - ssd1306_draw_printf()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_draw_chars(struct ssd1306_display *display, uint16_t delay) {
    uint8_t custom_char1_bitmap[] = {
        0b00000111, 0b11000000, 0b00110000, 0b01100001, 0b10000000, 0b00110011,
        0b11111111, 0b11101101, 0b11111111, 0b11110001, 0b11101111, 0b01100000,
        0b00000000, 0b11000000, 0b00000001, 0b10000000, 0b00000011, 0b00100000,
        0b00100101, 0b00100000, 0b10010010, 0b00111110, 0b00100010, 0b00000000,
        0b10000011, 0b00000110, 0b00000001, 0b11110000, 0b00000000};

    uint8_t custom_char2_bitmap[] = {
        0b00000000, 0b01000000, 0b00000010, 0b10000000, 0b00001010,
        0b00000000, 0b01001000, 0b00000010, 0b01000000, 0b00010011,
        0b11100001, 0b00000000, 0b01101010, 0b00000111, 0b10101000,
        0b00000110, 0b10100000, 0b01111010, 0b10000000, 0b01101010,
        0b00000111, 0b10101000, 0b00000111, 0b10011111, 0b11100000};

    struct ssd1306_custom_char custom_char1 = {
        custom_char1_bitmap, 15, 15, 0, -15, 18};
    struct ssd1306_custom_char custom_char2 = {
        custom_char2_bitmap, 14, 14, 0, -14, 17};

    while (1) {
        for (uint8_t i = 0; i < 6; i++) {
            ssd1306_draw_clear(display);
            ssd1306_set_cursor(display, 0, 15);

            if (i == 0) {
                ssd1306_draw_char(display, 'T');
                ssd1306_draw_char(display, 'e');
                ssd1306_draw_char(display, 's');
                ssd1306_draw_char(display, 't');
                ssd1306_draw_char(display, '1');
                ssd1306_draw_char(display, '2');
                ssd1306_draw_char(display, '3');
            } else if (i == 1) {
                ssd1306_draw_str(display, "Hello\nworld!");
            } else if (i == 2) {
                ssd1306_draw_char_custom(display, &custom_char1);
                ssd1306_draw_char_custom(display, &custom_char2);
                ssd1306_draw_char_custom(display, &custom_char1);
                ssd1306_draw_char_custom(display, &custom_char2);
                ssd1306_draw_char_custom(display, &custom_char1);
                ssd1306_draw_char_custom(display, &custom_char2);
            } else if (i == 3) {
                ssd1306_draw_int32(display, -159);
            } else if (i == 4) {
                ssd1306_draw_float(display, 3.141592f, 6);
            } else if (i == 5) {
                ssd1306_draw_printf(display, "Printf: %d", 255);
            }

            ssd1306_display_update(display);
            h_delay(delay);
        }
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_set_draw_border()
 *
 * - ssd1306_set_draw_border_reset()
 *
 * - ssd1306_get_draw_border()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 0.
 */
void ssd1306_test_border(struct ssd1306_display *display, uint16_t delay) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    uint8_t x_limit;
    uint8_t y_limit;
    uint8_t x_min;
    uint8_t y_min;
    uint8_t x_max;
    uint8_t y_max;

    /* Check limits */
    ssd1306_set_draw_border(display, 255, 255, 255, 255);
    ssd1306_get_draw_border(display, &x_min, &y_min, &x_max, &y_max);
    while (1) {
        if ((x_min == SSD1306_X_MAX) && (y_min == SSD1306_Y_MAX) &&
            (x_max == SSD1306_X_MAX) && (y_max == SSD1306_Y_MAX))
            break;
    }

    /* Check random value */
    ssd1306_set_draw_border(display, 5, 10, 15, 20);
    ssd1306_get_draw_border(display, &x_min, &y_min, &x_max, &y_max);
    while (1) {
        if ((x_min == 5) && (y_min == 10) && (x_max == 15) && (y_max == 20))
            break;
    }

    /* Check reset value */
    ssd1306_set_draw_border_reset(display);
    ssd1306_get_draw_border(display, &x_min, &y_min, &x_max, &y_max);
    while (1) {
        if ((x_min == 0) && (y_min == 0) && (x_max == SSD1306_X_MAX) &&
            (y_max == SSD1306_Y_MAX))
            break;
    }

    /* Animation */
    x_limit = SSD1306_X_MAX >> 1;
    y_limit = SSD1306_Y_MAX >> 1;
    x_min = 0;
    y_min = 0;
    x_max = SSD1306_X_MAX;
    y_max = SSD1306_Y_MAX;
    while (1) {
        if ((x_min == x_limit) && (y_min == y_limit) && (x_max == x_limit) &&
            (y_max == y_limit)) {
            x_min = 0;
            y_min = 0;
            x_max = SSD1306_X_MAX;
            y_max = SSD1306_Y_MAX;
        }

        if (x_min < x_limit)
            x_min++;

        if (y_min < y_limit)
            y_min++;

        if (x_max > x_limit)
            x_max--;

        if (y_max > y_limit)
            y_max--;

        ssd1306_set_draw_border(display, x_min, y_min, x_max, y_max);
        ssd1306_draw_clear(display);
        ssd1306_draw_rect_fill(display, 0, 0, SSD1306_X_MAX, SSD1306_Y_MAX);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_set_buffer_mode()
 *
 * - ssd1306_set_buffer_mode_inverse()
 *
 * - ssd1306_get_buffer_mode()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_buffer_mode(struct ssd1306_display *display, uint16_t delay) {
    /* Tests */
    while (1) {
        ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
        if (ssd1306_get_buffer_mode(display) == SSD1306_BUFFER_MODE_DRAW)
            break;
    }
    while (1) {
        ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_CLEAR);
        if (ssd1306_get_buffer_mode(display) == SSD1306_BUFFER_MODE_CLEAR)
            break;
    }
    ssd1306_set_buffer_mode_inverse(display);
    while (1) {
        if (ssd1306_get_buffer_mode(display) == SSD1306_BUFFER_MODE_DRAW)
            break;
    }

    /* Animation */
    while (1) {
        ssd1306_set_buffer_mode_inverse(display);
        ssd1306_draw_rect_fill(display, 0, 0, SSD1306_X_MAX, SSD1306_Y_MAX_64);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for (requires font to be set up, ideally < 10px):
 *
 * - ssd1306_set_font()
 *
 * - ssd1306_set_font_scale()
 *
 * - ssd1306_set_cursor()
 *
 * - ssd1306_get_font()
 *
 * - ssd1306_get_font_scale()
 *
 * - ssd1306_get_cursor()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_font(struct ssd1306_display *display, uint16_t delay) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    const struct ssd1306_font *original_font = ssd1306_get_font(display);
    int16_t x0, x, y;

    /* Tests */
    ssd1306_set_font(display, NULL);
    while (1) {
        if (ssd1306_get_font(display) == NULL)
            break;
    }

    ssd1306_set_font(display, original_font);
    while (1) {
        if (ssd1306_get_font(display) == original_font)
            break;
    }

    ssd1306_set_font_scale(display, 3);
    while (1) {
        if (ssd1306_get_font_scale(display) == 3)
            break;
    }

    ssd1306_set_font_scale(display, 1);
    while (1) {
        if (ssd1306_get_font_scale(display) == 1)
            break;
    }

    ssd1306_set_cursor(display, 10, 11);
    x0 = ssd1306_get_cursor(display, &x, &y);
    while (1) {
        if ((x0 == 10) && (x == 10) && (y == 11))
            break;
    }

    ssd1306_set_cursor(display, 0, 15);
    x0 = ssd1306_get_cursor(display, &x, &y);
    while (1) {
        if ((x0 == 0) && (x == 0) && (y == 15))
            break;
    }

    /* Animation */
    uint8_t scale = 1;
    while (1) {
        if (scale == 3)
            scale = 1;
        else
            scale++;

        ssd1306_draw_clear(display);
        ssd1306_set_font_scale(display, scale);
        ssd1306_draw_str(display, "PASSED\r");
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Tests for:
 *
 * - ssd1306_get_display_address()
 *
 * - ssd1306_get_display_type()
 *
 * - sd1306_get_buffer()
 *
 * - ssd1306_get_buffer_pixel()
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Recommended
 * starting value is 6000.
 */
void ssd1306_test_get_others(struct ssd1306_display *display, uint16_t delay) {
    uint8_t *buffer = display->data_buffer - 2;

    /* Tests */
    while (1) {
        if ((*buffer >> 1) == ssd1306_get_display_address(display))
            break;
    }

    while (1) {
        if (display->display_type == ssd1306_get_display_type(display))
            break;
    }

    while (1) {
        if (buffer == sd1306_get_buffer(display))
            break;
    }

    ssd1306_draw_clear(display);
    ssd1306_draw_pixel(display, 10, 10);
    ssd1306_draw_pixel(display, 30, 30);
    ssd1306_draw_pixel(display, 120, 20);
    while (1) {
        if (ssd1306_get_buffer_pixel(display, 10, 10) &&
            ssd1306_get_buffer_pixel(display, 30, 30) &&
            ssd1306_get_buffer_pixel(display, 120, 20))
            break;
    }

    while (1) {
        if (!ssd1306_get_buffer_pixel(display, 5, 5) &&
            !ssd1306_get_buffer_pixel(display, 20, 20) &&
            !ssd1306_get_buffer_pixel(display, 110, 20))
            break;
    }

    /* Animation */
    while (1) {
        h_draw_logo(display);
        h_delay(delay);

        ssd1306_draw_clear(display);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}
