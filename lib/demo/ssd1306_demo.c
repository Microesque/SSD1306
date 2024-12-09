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

#include "ssd1306_demo.h"

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

/*----------------------------------------------------------------------------*/
/*------------------------------ Demo Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief DVD animation of the Microesque logo.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Start with
 * 0, if the animation is too fast, increase the value.
 */
void ssd1306_demo_1(struct ssd1306_display *display, uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    /* Microesque logo (63x11px) */
    const static uint8_t BITMAP_WIDTH = 63;
    const static uint8_t BITMAP_HEIGHT = 11;
    const static uint8_t BITMAP[] = {
        0x03, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x01, 0xb8, 0xfb,
        0xff, 0xff, 0xff, 0xff, 0x7f, 0x30, 0x30, 0xd9, 0xff, 0xff, 0xff,
        0xff, 0x7f, 0x70, 0xb0, 0xfa, 0x31, 0xe6, 0x1c, 0xb3, 0x4d, 0xf0,
        0xb0, 0xdb, 0xde, 0x5b, 0xeb, 0xad, 0x35, 0xf0, 0xb0, 0xdb, 0xde,
        0x5b, 0x98, 0xad, 0x05, 0x70, 0xb0, 0xdb, 0xde, 0x5b, 0x7f, 0xa3,
        0x75, 0x30, 0xb0, 0xdb, 0xd1, 0xe7, 0x88, 0x6f, 0x0e, 0x01, 0xf8,
        0xff, 0xff, 0xff, 0xff, 0xef, 0x7f, 0x03, 0xfc, 0xff, 0xff, 0xff,
        0xff, 0xff, 0x7f, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    enum direction x_dir = POSITIVE;
    enum direction y_dir = POSITIVE;
    int8_t bitmap_x0 = 0;
    int8_t bitmap_y0 = 0;

    ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
    while (1) {
        if (x_dir == POSITIVE)
            bitmap_x0++;
        else
            bitmap_x0--;

        if (y_dir == POSITIVE)
            bitmap_y0++;
        else
            bitmap_y0--;

        if (bitmap_x0 == 0 || ((bitmap_x0 + BITMAP_WIDTH) == SSD1306_X_MAX))
            x_dir ^= 1;

        if ((bitmap_y0 == 0) || ((bitmap_y0 + BITMAP_HEIGHT) == SSD1306_Y_MAX))
            y_dir ^= 1;

        ssd1306_draw_clear(display);
        ssd1306_draw_bitmap(display, bitmap_x0, bitmap_y0, BITMAP, BITMAP_WIDTH,
                            BITMAP_HEIGHT, false);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Animation of lines sprouting from each corner.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Start with
 * 0, if the animation is too fast, increase the value.
 */
void ssd1306_demo_2(struct ssd1306_display *display, uint16_t delay) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
    const uint8_t LINE_GAP = 7;
    uint8_t i;
    while (1) {
        /* Top left*/
        ssd1306_draw_clear(display);
        for (i = 0; i <= SSD1306_Y_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, 0, 0, SSD1306_X_MAX, i);
            ssd1306_display_update(display);
            h_delay(delay);
        }
        for (i = LINE_GAP; i <= SSD1306_X_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, 0, 0, SSD1306_X_MAX - i, SSD1306_Y_MAX);
            ssd1306_display_update(display);
            h_delay(delay);
        }

        /* Top right */
        ssd1306_draw_clear(display);
        for (i = 0; i <= SSD1306_Y_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, SSD1306_X_MAX, 0, 0, i);
            ssd1306_display_update(display);
            h_delay(delay);
        }
        for (i = LINE_GAP; i <= SSD1306_X_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, SSD1306_X_MAX, 0, i, SSD1306_Y_MAX);
            ssd1306_display_update(display);
            h_delay(delay);
        }

        /* Bottom right */
        ssd1306_draw_clear(display);
        for (i = 0; i <= SSD1306_Y_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, SSD1306_X_MAX, SSD1306_Y_MAX, 0,
                              SSD1306_Y_MAX - i);
            ssd1306_display_update(display);
            h_delay(delay);
        }
        for (i = LINE_GAP; i <= SSD1306_X_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, SSD1306_X_MAX, SSD1306_Y_MAX, i, 0);
            ssd1306_display_update(display);
            h_delay(delay);
        }

        /* Bottom left */
        ssd1306_draw_clear(display);
        for (i = 0; i <= SSD1306_Y_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, 0, SSD1306_Y_MAX, SSD1306_X_MAX,
                              SSD1306_Y_MAX - i);
            ssd1306_display_update(display);
            h_delay(delay);
        }
        for (i = LINE_GAP; i <= SSD1306_X_MAX; i += LINE_GAP) {
            ssd1306_draw_line(display, 0, SSD1306_Y_MAX, SSD1306_X_MAX - i, 0);
            ssd1306_display_update(display);
            h_delay(delay);
        }
    }
}

/**
 * @brief Animation of fireworks going off.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Start with
 * 0, if the animation is too fast, increase the value.
 */
void ssd1306_demo_3(struct ssd1306_display *display, uint16_t delay) {
    struct firework {
        uint8_t cooldown; /* Initial cooldown before firework goes off */
        uint8_t x;        /* y coordinate of the firework*/
        uint8_t y;        /* x coordinate of the firework*/
        uint8_t step;     /* Set to '0' on init */
        bool is_second;   /* Set to false on init */
    };

    struct firework fireworks[] = {
        {0, 17, 6, 0, false},    {10, 72, 22, 0, false},
        {18, 50, 0, 0, false},   {21, 100, 35, 0, false},
        {24, 10, 55, 0, false},  {30, 120, 5, 0, false},
        {32, 115, 62, 0, false}, {35, 10, 25, 0, false},
        {40, 55, 58, 0, false},  {45, 90, 8, 0, false},
        {50, 40, 28, 0, false},  {55, 122, 20, 0, false}};
    static const uint8_t FIREWORK_COUNT = 12;
    static const uint8_t FIREWORK_RADIUS = 14;
    static const uint8_t FIREWORK_COOLDOWN = 30;

    ssd1306_draw_clear(display);
    while (1) {
        for (uint8_t j = 0; j < FIREWORK_COUNT; j++) {
            struct firework *f = &fireworks[j];
            if (f->cooldown != 0) {
                f->cooldown--;
                continue;
            }

            if (f->is_second)
                ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_CLEAR);
            else
                ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
            ssd1306_draw_circle(display, f->x, f->y, f->step);

            if (f->step == FIREWORK_RADIUS) {
                if (f->is_second)
                    f->cooldown = FIREWORK_COOLDOWN;
                f->step = 0;
                f->is_second ^= 1;
            } else {
                f->step++;
            }
        }
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Animation of a triangle with independently moving corners.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Start with
 * 0, if the animation is too fast, increase the value.
 */
void ssd1306_demo_4(struct ssd1306_display *display, uint16_t delay) {
    enum direction {
        NEGATIVE,
        POSITIVE
    };

    struct point {
        uint8_t x;
        uint8_t y;
        enum direction x_dir;
        enum direction y_dir;
    };

    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    struct point points[] = {{60, 2, POSITIVE, POSITIVE},
                             {30, 30, POSITIVE, NEGATIVE},
                             {80, 20, NEGATIVE, POSITIVE}};

    ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
    while (1) {
        for (uint8_t i = 0; i < 3; i++) {
            struct point *p = &points[i];
            if (p->x_dir == POSITIVE)
                p->x++;
            else
                p->x--;

            if (p->y_dir == POSITIVE)
                p->y++;
            else
                p->y--;

            if ((p->x == 0) || (p->x == SSD1306_X_MAX))
                p->x_dir ^= 1;

            if ((p->y) == 0 || (p->y == SSD1306_Y_MAX))
                p->y_dir ^= 1;
        }
        ssd1306_draw_clear(display);
        ssd1306_draw_triangle_fill(display, points[0].x, points[0].y,
                                   points[1].x, points[1].y, points[2].x,
                                   points[2].y);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}

/**
 * @brief Animation of a spiral forming.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param delay Arbitrary delay value that slows down the animation. Start with
 * 0, if the animation is too fast, increase the value.
 */
void ssd1306_demo_5(struct ssd1306_display *display, uint16_t delay) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    int16_t x0 = SSD1306_X_MAX >> 1;
    int16_t y0 = SSD1306_Y_MAX >> 1;
    int16_t r_max = SSD1306_X_MAX >> 1;
    int16_t r = 0;
    uint8_t quadrant = 1;
    ssd1306_set_buffer_mode(display, SSD1306_BUFFER_MODE_DRAW);
    while (1) {
        if (r == r_max) {
            r = 1;
            quadrant = 1;
            ssd1306_set_buffer_mode_inverse(display);
        } else {
            r++;
        }

        if (quadrant == 0x10)
            quadrant = 1;
        else
            quadrant <<= 1;

        ssd1306_draw_arc(display, x0, y0, r, quadrant);
        ssd1306_display_update(display);
        h_delay(delay);
    }
}
