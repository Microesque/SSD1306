/*
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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef SSD1306_H
    #define	SSD1306_H

    /*------------------------------------------------------------------------*/
    /*-------------------------- Necessary Libraries -------------------------*/
    /*------------------------------------------------------------------------*/

    #include <stdint.h>
    #include <stdbool.h>

    
    /*------------------------------------------------------------------------*/
    /*-------------------------- Useful Enums/Macros -------------------------*/
    /*------------------------------------------------------------------------*/

    /*
     * If the display is in "draw" mode, all of the draw methods will turn those
     * pixels on. In "clear" mode, they will turn those pixels off instead.
     */
    typedef enum {
        SSD1306_MODE_CLEAR,
        SSD1306_MODE_DRAW
    } SSD1306_BufferMode;
    
    /*
     * Choose according to your display type (128x32 or 128x64).
     */
    typedef enum {
        SSD1306_DISPLAY_TYPE_32,
        SSD1306_DISPLAY_TYPE_64
    } SSD1306_DisplayType;

    /*
     * Buffer sizes needed for the respective display type (128x32 or 128x64).
     */
    #define SSD1306_BUFFER_SIZE_32 512 //
    #define SSD1306_BUFFER_SIZE_64 1024

    /*
     * Max. coordinates for both display types.
     */
    #define SSD1306_X_MAX  127 //
    #define SSD1306_Y_MAX_32  31 
    #define SSD1306_Y_MAX_64  63 

    
    /*------------------------------------------------------------------------*/
    /*------------------------------ Structures ------------------------------*/
    /*------------------------------------------------------------------------*/

    /*
     * Structure that represents your displays. Initialize with
     * "SSD1306_init()".
     */
    typedef struct {
        uint8_t* buffer;
        SSD1306_DisplayType display_type;
        uint8_t I2C_address;
        void (*I2C_start)(void);
        uint8_t (*I2C_write)(uint8_t);
        void (*I2C_stop)(void);
        SSD1306_BufferMode buffer_mode;
    } SSD1306_T;
    
    /*------------------------------------------------------------------------*/
    /*-------------------------- Available Functions -------------------------*/
    /*------------------------------------------------------------------------*/
    
    void SSD1306_init(SSD1306_T* display,
                      uint8_t* buffer,
                      SSD1306_DisplayType display_type,
                      uint8_t I2C_address,
                      void (*I2C_start)(void),
                      uint8_t (*I2C_write)(uint8_t),
                      void (*I2C_stop)(void));
    void SSD1306_reinit(SSD1306_T* display);
    
    void SSD1306_display_update(SSD1306_T* display);
    void SSD1306_display_brightness(SSD1306_T* display, uint8_t brightness);
    void SSD1306_display_enable(SSD1306_T* display, bool is_enabled);
    void SSD1306_display_fully_on(SSD1306_T* display, bool is_enabled);
    void SSD1306_display_inverse(SSD1306_T* display, bool is_enabled);
    void SSD1306_display_mirror_h(SSD1306_T* display, bool is_enabled);
    void SSD1306_display_mirror_v(SSD1306_T* display, bool is_enabled);
    void SSD1306_display_scroll_enable(SSD1306_T* display, bool is_left,
                                       bool is_diagonal, uint8_t interval);
    void SSD1306_display_scroll_disable(SSD1306_T* display);
    
    void SSD1306_draw_set_mode(SSD1306_T* display, SSD1306_BufferMode mode);
    void SSD1306_draw_clear(SSD1306_T* display);
    void SSD1306_draw_fill(SSD1306_T* display);
    void SSD1306_draw_pixel(SSD1306_T* display, int16_t x, int16_t y);
    void SSD1306_draw_line_h(SSD1306_T* display, int16_t x0, int16_t y0,
                             int16_t width);
    void SSD1306_draw_line_v(SSD1306_T* display, int16_t x0, int16_t y0,
                             int16_t height);
    void SSD1306_draw_line(SSD1306_T* display, int16_t x0, int16_t y0,
                           int16_t x1, int16_t y1);
    void SSD1306_draw_triangle(SSD1306_T* display, int16_t x0, int16_t y0,
                               int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void SSD1306_draw_triangle_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                                    int16_t x1, int16_t y1, int16_t x2,
                                    int16_t y2);
    void SSD1306_draw_rect(SSD1306_T* display, int16_t x0, int16_t y0,
                           int16_t width, int16_t height);
    void SSD1306_draw_rect_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                                int16_t width, int16_t height);
    void SSD1306_draw_arc(SSD1306_T* display, int16_t x0, int16_t y0, int16_t r,
                          uint8_t quadrant);
    void SSD1306_draw_arc_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                               int16_t r, uint8_t quadrant);

#endif
