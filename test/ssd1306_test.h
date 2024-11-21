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

#ifndef SSD1306_TEST_H
#define SSD1306_TEST_H

/*----------------------------------------------------------------------------*/
/*---------------------------- Necessary Libraries ---------------------------*/
/*----------------------------------------------------------------------------*/

#include "ssd1306.h"

/*----------------------------------------------------------------------------*/
/*---------------------------- Available Functions ---------------------------*/
/*----------------------------------------------------------------------------*/

void ssd1306_test_reinit(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_brightness(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_enable(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_fully_on(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_inverse(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_mirrors(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_scroll_enable_disable(struct ssd1306_display *display,
                                        uint16_t delay);

void ssd1306_test_draw_clear_fill(struct ssd1306_display *display,
                                  uint16_t delay);
void ssd1306_test_draw_shifts(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_draw_lines(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_draw_triangles(struct ssd1306_display *display,
                                 uint16_t delay);
void ssd1306_test_draw_rects(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_draw_rect_rounds(struct ssd1306_display *display,
                                   uint16_t delay);
void ssd1306_test_draw_arcs(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_draw_chars(struct ssd1306_display *display, uint16_t delay);

void ssd1306_test_border(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_buffer_mode(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_font(struct ssd1306_display *display, uint16_t delay);
void ssd1306_test_get_others(struct ssd1306_display *display, uint16_t delay);

/*---------- Omitted tests ----------*/
/* ssd1306_init()           -> tested by everything */
/* ssd1306_display_update() -> tested by everything */
/* ssd1306_draw_pixel()     -> tested by everything */
/* ssd1306_draw_circle()    -> tested by ssd1306_test_draw_arcs() */
/* ssd1306_draw_bitmap()    -> tested by display functions */
/* ssd1306_set_cursor()     -> tested by ssd1306_test_draw_chars */

#endif
