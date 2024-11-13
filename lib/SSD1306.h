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
    /*--------------------- Necessary Standard Libraries ---------------------*/
    /*------------------------------------------------------------------------*/

    #include <stdint.h>
    #include <stdbool.h>
    #include <stddef.h>


    /*------------------------------------------------------------------------*/
    /*----------------------------- Enums/Macros -----------------------------*/
    /*------------------------------------------------------------------------*/

    /*
    If the display is in "draw" mode, all of the draw methods will turn those
    pixels on. In "clear" mode, they will turn those pixels off instead.
     */
    typedef enum {
        SSD1306_BUFFER_MODE_CLEAR,
        SSD1306_BUFFER_MODE_DRAW
    } SSD1306_BufferMode;
    
    /*
    Choose according to your display type (128x32 or 128x64).
     */
    typedef enum {
        SSD1306_DISPLAY_TYPE_32,
        SSD1306_DISPLAY_TYPE_64
    } SSD1306_DisplayType;

    /*
    Buffer sizes needed for the respective display type (128x32 or 128x64).
     */
    #define SSD1306_BUFFER_SIZE_32 512 //
    #define SSD1306_BUFFER_SIZE_64 1024

    /*
    Max. coordinates for both display types.
     */
    #define SSD1306_X_MAX  127 //
    #define SSD1306_Y_MAX_32  31 
    #define SSD1306_Y_MAX_64  63
    
    /*
    Masks that can be OR'd to be used in the "SSD1306_draw_arc()" and
    "SSD1306_draw_arc_fill()" functions.
     */
    #define SSD1306_ARC_QUAD1  0x1  // Mask to enable quadrant-1
    #define SSD1306_ARC_QUAD2  0x2  // Mask to enable quadrant-2
    #define SSD1306_ARC_QUAD3  0x4  // Mask to enable quadrant-3
    #define SSD1306_ARC_QUAD4  0x8  // Mask to enable quadrant-4


    /*------------------------------------------------------------------------*/
    /*------------------------------ Structures ------------------------------*/
    /*------------------------------------------------------------------------*/
    
    /*
    Adafruit-GFX glyph structure.
    */
    typedef struct {
        uint16_t bitmap_offset;
        uint8_t width;
        uint8_t height;
        uint8_t x_advance;
        int8_t x_offset;
        int8_t y_offset;
    } GFXglyph;

    /*
    Adafruit-GFX font structure.
    */
    typedef struct {
        uint8_t* bitmap;
        GFXglyph* glyph;
        uint16_t first;
        uint16_t last;
        uint8_t y_advance;
    } GFXfont;
    
    /*
    Custom character structure.
    */
    typedef struct {
        uint8_t* bitmap;
        uint8_t width;
        uint8_t height;
        int8_t x_offset;
        int8_t y_offset;
        uint8_t x_advance;
    } SSD1306_CustomChar;

    /*
    Structure that represents your displays. Initialize with "SSD1306_init()".
     */
    typedef struct {
        void (*I2C_start)(void);
        uint8_t (*I2C_write)(uint8_t);
        void (*I2C_stop)(void);
        const GFXfont* font;
        uint8_t* buffer;
        int16_t cursor_x0;
        int16_t cursor_x;
        int16_t cursor_y;
        SSD1306_DisplayType display_type;
        SSD1306_BufferMode buffer_mode;
        uint8_t I2C_address;
        uint8_t font_scale;
    } SSD1306_T;


    /*------------------------------------------------------------------------*/
    /*----------------------------- Library Setup ----------------------------*/
    /*------------------------------------------------------------------------*/

    /*
    Maximum number of characters that 'SSD1306_draw_printf()' can display at a
    time, including the null termination.
    
    "SSD1306_PRINTF_CHAR_LIMIT * sizeof(char)" bytes of RAM will be reserved
    (total, not per display), but only if you use 'SSD1306_draw_printf()'.
    */
    #define SD1306_PRINTF_CHAR_LIMIT 255

    /*
    Below, you can configure the default settings for the display after calling
    'SSD1306_init()' or 'SSD1306_reinit()'. Square brackets denote valid values.
    
    If you're unsure about what these do, just leave them as they are. All of
    the settings below have an equivalent function to setup/change, all of
    which also come with a comprehensive description. 
    */

    /*
    Brightness level [0-255].
    */
    #define SSD1306_DEFAULT_BRIGHTNESS 127

    /*
    Should the display be fully on [true | false].
    */
    #define SSD1306_DEFAULT_FULLY_ON false

    /*
    Should the display be inverted [true | false].
    */
    #define SSD1306_DEFAULT_INVERSE false

    /*
    Should the display be mirrored horizontally [true | false].
    */
    #define SSD1306_DEFAULT_MIRROR_H false

    /*
    Should the display be mirrored vertically [true | false].
    */
    #define SSD1306_DEFAULT_MIRROR_V false

    /*
    Should the display be enabled [true | false].
    */
    #define SSD1306_DEFAULT_ENABLE true

    /*
    Buffer mode [SSD1306_BUFFER_MODE_CLEAR | SSD1306_BUFFER_MODE_DRAW].
    */
    #define SSD1306_DEFAULT_BUFFER_MODE SSD1306_BUFFER_MODE_DRAW

    /*
    Should the buffer be cleared [true | false].
    */
    #define SSD1306_DEFAULT_CLEAR_BUFFER true

    /*
    Should the buffer be filled [true | false].
    */
    #define SSD1306_DEFAULT_FILL_BUFFER false

    /*
    Default font, 'NULL' means no font [NULL, &<GFXfont>].

    To set up a default font, include the header file of your font above the
    macro, then set the value to the address of the 'GFXfont' variable.

    Ex:
        #include "../fonts/RandomFont.h"
        #define SSD1306_DEFAULT_FONT &RandomFont
    */
    #define SSD1306_DEFAULT_FONT NULL

    /*
    Font scale [0-255].
    */
    #define SSD1306_DEFAULT_FONT_SCALE 1

    /*
    Cursor x-coordinate [-32768...32767].
    */
    #define SSD1306_DEFAULT_CURSOR_X 0

    /*
    Cursor y-coordinate [-32768...32767].
    */
    #define SSD1306_DEFAULT_CURSOR_Y 15


    /*------------------------------------------------------------------------*/
    /*-------------------------- Available Functions -------------------------*/
    /*------------------------------------------------------------------------*/
    
    void ssd1306_init(SSD1306_T* display,
                      uint8_t I2C_address,
                      void (*I2C_start)(void),
                      uint8_t (*I2C_write)(uint8_t),
                      void (*I2C_stop)(void),
                      SSD1306_DisplayType display_type,
                      uint8_t* buffer);
    void ssd1306_reinit(SSD1306_T* display);
    
    void ssd1306_display_update(SSD1306_T* display);
    void ssd1306_display_brightness(SSD1306_T* display, uint8_t brightness);
    void ssd1306_display_enable(SSD1306_T* display, bool is_enabled);
    void ssd1306_display_fully_on(SSD1306_T* display, bool is_enabled);
    void ssd1306_display_inverse(SSD1306_T* display, bool is_enabled);
    void ssd1306_display_mirror_h(SSD1306_T* display, bool is_enabled);
    void ssd1306_display_mirror_v(SSD1306_T* display, bool is_enabled);
    void ssd1306_display_scroll_enable(SSD1306_T* display, bool is_left,
                                       bool is_diagonal, uint8_t interval);
    void ssd1306_display_scroll_disable(SSD1306_T* display);
    
    void ssd1306_draw_clear(SSD1306_T* display);
    void ssd1306_draw_fill(SSD1306_T* display);
    void ssd1306_draw_shift_right(SSD1306_T* display, bool is_rotated);
    void ssd1306_draw_shift_left(SSD1306_T* display, bool is_rotated);
    void ssd1306_draw_shift_up(SSD1306_T* display, bool is_rotated);
    void ssd1306_draw_shift_down(SSD1306_T* display, bool is_rotated);
    void ssd1306_draw_pixel(SSD1306_T* display, int16_t x, int16_t y);
    void ssd1306_draw_line_h(SSD1306_T* display, int16_t x0, int16_t y0,
                             int16_t width);
    void ssd1306_draw_line_v(SSD1306_T* display, int16_t x0, int16_t y0,
                             int16_t height);
    void ssd1306_draw_line(SSD1306_T* display, int16_t x0, int16_t y0,
                           int16_t x1, int16_t y1);
    void ssd1306_draw_triangle(SSD1306_T* display, int16_t x0, int16_t y0,
                               int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void ssd1306_draw_triangle_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                                    int16_t x1, int16_t y1, int16_t x2,
                                    int16_t y2);
    void ssd1306_draw_rect(SSD1306_T* display, int16_t x0, int16_t y0,
                           int16_t width, int16_t height);
    void ssd1306_draw_rect_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                                int16_t width, int16_t height);
    void ssd1306_draw_rect_round(SSD1306_T* display, int16_t x0, int16_t y0,
                                 int16_t width, int16_t height, int16_t r);
    void ssd1306_draw_rect_round_fill(SSD1306_T* display, int16_t x0,
                                      int16_t y0, int16_t width, int16_t height,
                                      int16_t r);
    void ssd1306_draw_arc(SSD1306_T* display, int16_t x0, int16_t y0, int16_t r,
                          uint8_t quadrant);
    void ssd1306_draw_arc_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                               int16_t r, uint8_t quadrant);
    void ssd1306_draw_circle(SSD1306_T* display, int16_t x0, int16_t y0,
                             int16_t r);
    void ssd1306_draw_circle_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                                  int16_t r);
    void ssd1306_draw_bitmap(SSD1306_T* display, int16_t x0, int16_t y0,
                             const uint8_t* bmp, uint16_t width,
                             uint16_t height, bool has_bg);
    void ssd1306_draw_char(SSD1306_T* display, char c);
    void ssd1306_draw_char_custom(SSD1306_T* display,
                                  const SSD1306_CustomChar* c);
    void ssd1306_draw_str(SSD1306_T* display, const char* str);
    void ssd1306_draw_int32(SSD1306_T* display, int32_t num);
    void ssd1306_draw_float(SSD1306_T* display, float num, uint8_t digits);
    void ssd1306_draw_printf(SSD1306_T* display, const char* format, ...);

    void ssd1306_set_buffer_mode(SSD1306_T* display, SSD1306_BufferMode mode);
    void ssd1306_set_font(SSD1306_T* display, const GFXfont* font);
    void ssd1306_set_font_scale(SSD1306_T* display, uint8_t scale);
    void ssd1306_set_cursor(SSD1306_T* display, int16_t x, int16_t y);

    uint8_t ssd1306_get_display_address(SSD1306_T* display);
    SSD1306_DisplayType ssd1306_get_display_type(SSD1306_T* display);
    SSD1306_BufferMode ssd1306_get_buffer_mode(SSD1306_T* display);
    const GFXfont* ssd1306_get_font(SSD1306_T* display);
    uint8_t ssd1306_get_font_scale(SSD1306_T* display);
    int16_t ssd1306_get_cursor(SSD1306_T* display, int16_t* x, int16_t* y);
    uint8_t* ssd1306_get_buffer(SSD1306_T* display);
    uint8_t ssd1306_get_buffer_pixel(SSD1306_T* display, int16_t x, int16_t y);

#endif
