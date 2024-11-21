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

#ifndef SSD1306_DEMO_H
#define SSD1306_DEMO_H

/*----------------------------------------------------------------------------*/
/*---------------------------- Necessary Libraries ---------------------------*/
/*----------------------------------------------------------------------------*/

#include "ssd1306.h"

/*----------------------------------------------------------------------------*/
/*---------------------------- Available Functions ---------------------------*/
/*----------------------------------------------------------------------------*/

void ssd1306_demo_1(struct ssd1306_display *display, uint16_t delay);
void ssd1306_demo_2(struct ssd1306_display *display, uint16_t delay);
void ssd1306_demo_3(struct ssd1306_display *display, uint16_t delay);
void ssd1306_demo_4(struct ssd1306_display *display, uint16_t delay);
void ssd1306_demo_5(struct ssd1306_display *display, uint16_t delay);

#endif
