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

#include "ssd1306.h"
#include <stdarg.h>
#include <stdio.h>

/*----------------------------------------------------------------------------*/
/*-------------------------- SSD1306 Address Macros --------------------------*/
/*----------------------------------------------------------------------------*/

/* clang-format off */
#define SSD1306_CONTROL_CMD                    0x00 /**/
#define SSD1306_CONTROL_DATA                   0x40
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA   0xA3
#define SSD1306_CMD_SET_MUX_RATIO              0xA8
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE 0x20
#define SSD1306_CMD_SET_COM_CONFIGURATION      0xDA
#define SSD1306_CMD_SET_COLUMN_ADDRESS         0x21
#define SSD1306_CMD_SET_PAGE_ADDRESS           0x22
#define SSD1306_CMD_SET_DIV_RATIO_AND_FREQ     0xD5
#define SSD1306_CMD_SET_CHARGE_PUMP            0x8D
#define SSD1306_CMD_SET_CONTRAST_CONTROL       0x81
#define SSD1306_CMD_SET_SCROLL_JUST_RIGHT      0x26
#define SSD1306_CMD_SET_SCROLL_JUST_LEFT       0x27
#define SSD1306_CMD_SET_SCROLL_DIAGONAL_RIGHT  0x29
#define SSD1306_CMD_SET_SCROLL_DIAGONAL_LEFT   0x2A
#define SSD1306_CMD_DISPLAY_ON                 0xAF
#define SSD1306_CMD_DISPLAY_OFF                0xAE
#define SSD1306_CMD_ENTIRE_DISPLAY_ON_ENABLED  0xA5
#define SSD1306_CMD_ENTIRE_DISPLAY_ON_DISABLED 0xA4
#define SSD1306_CMD_INVERSE_ENABLED            0xA7
#define SSD1306_CMD_INVERSE_DISABLED           0xA6
#define SSD1306_CMD_SEGMENT_REMAP_ENABLED      0xA1
#define SSD1306_CMD_SEGMENT_REMAP_DISABLED     0xA0
#define SSD1306_CMD_SCAN_REMAP_ENABLED         0xC8
#define SSD1306_CMD_SCAN_REMAP_DISABLED        0xC0
#define SSD1306_CMD_SCROLL_DISABLE             0x2E
#define SSD1306_CMD_SCROLL_ENABLE              0x2F
/* clang-format on */

/*----------------------------------------------------------------------------*/
/*----------------------- Library Enums/Macros/Globals -----------------------*/
/*----------------------------------------------------------------------------*/

#define SSD1306_BUFFER_SIZE_32 (SSD1306_ARRAY_SIZE_32 - 2)
#define SSD1306_BUFFER_SIZE_64 (SSD1306_ARRAY_SIZE_64 - 2)

enum ssd1303_page_offsets {
    SSD1306_PAGE0_OFFSET = 0,
    SSD1306_PAGE1_OFFSET = 128,
    SSD1306_PAGE2_OFFSET = 256,
    SSD1306_PAGE3_OFFSET = 384,
    SSD1306_PAGE4_OFFSET = 512,
    SSD1306_PAGE5_OFFSET = 640,
    SSD1306_PAGE6_OFFSET = 768,
    SSD1306_PAGE7_OFFSET = 896
};
static const uint16_t SSD1306_PAGE_OFFSETS[] = {
    SSD1306_PAGE0_OFFSET, SSD1306_PAGE1_OFFSET, SSD1306_PAGE2_OFFSET,
    SSD1306_PAGE3_OFFSET, SSD1306_PAGE4_OFFSET, SSD1306_PAGE5_OFFSET,
    SSD1306_PAGE6_OFFSET, SSD1306_PAGE7_OFFSET};

/*----------------------------------------------------------------------------*/
/*----------------------------- Helper Functions -----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Sends the command buffer to the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param length The number of bytes to send. Maximum 8 commands, not checked!
 */
static void h_send_cmd_buffer(struct ssd1306_display *display, uint8_t length) {
    display->cmd_memory[0] = display->i2c_address;
    display->i2c_write(display->cmd_memory, length + 2);
}

/**
 * @brief Sends the data (draw) buffer to the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
static void h_send_data_buffer(struct ssd1306_display *display) {
    uint16_t buffer_size;
    if (display->display_type)
        buffer_size = SSD1306_ARRAY_SIZE_64;
    else
        buffer_size = SSD1306_ARRAY_SIZE_32;

    *(display->data_buffer - 2) = display->i2c_address;
    display->i2c_write(display->data_buffer - 2, buffer_size);
}

/**
 * @brief Checks it the specified coordinates are within drawing border for the
 * specified display.
 *
 * @note After this check, these are guaranteed:
 *
 * - (x > 0) and (y > 0)
 *
 * - (x < SSD1306_X_MAX)
 *
 * - (y < SSD1306_Y_MAX_32) for 128x32 displays
 *
 * - (y < SSD1306_Y_MAX_64) for 128x64 displays
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x x-coordinate to check.
 * @param y y-coordinate to check.
 * @return 'true' if within range; 'false' if out of border.
 */
static bool h_are_coords_in_border(struct ssd1306_display *display, int16_t x,
                                   int16_t y) {
    if ((x < display->border_x_min) || (x > display->border_x_max) ||
        (y < display->border_y_min) || (y > display->border_y_max))
        return false;

    return true;
}

/**
 * @brief Draws a character with the specified values at the current cursor
 * location.
 *
 * @note
 * - Only draws characters with the specified bitmap; doesn't handle special
 * non-printable characters like '\\n'.
 *
 * - Automatically advances the cursor.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param bitmap Pointer to the bitmap array.
 * @param width Width of the character. Should match the bitmap or random parts
 * of the memory may be accessed.
 * @param height Height of the character. Should match the bitmap or random
 * parts of the memory may be accessed.
 * @param x_offset x-offset of the character. Represents the number of pixels
 * (horizontally) the top-left corner of the bitmap is offset from the current
 * cursor coordinates.
 * @param y_offset y-offset of the character. Represents the number of pixels
 * (vertically) the top-left corner of the bitmap is offset from the current
 * cursor coordinates.
 * @param x_advance x-advance of the character. Represents the number of pixels
 * the cursor should advance after drawing.
 */
static void h_draw_char(struct ssd1306_display *display, const uint8_t *bitmap,
                        uint8_t width, uint8_t height, int8_t x_offset,
                        int8_t y_offset, uint8_t x_advance) {
    int16_t x0 = display->cursor_x + x_offset;
    int16_t y0 = display->cursor_y + y_offset;
    uint8_t scale = display->font_scale;
    uint8_t count = 0;
    uint8_t pixels;
    for (uint8_t h = 0; h < height; h++) {
        for (uint8_t w = 0; w < width; w++) {
            if (count == 0) {
                count = 8;
                pixels = *bitmap++;
            }
            count--;

            if (pixels & 0x80) {
                ssd1306_draw_rect_fill(display, x0 + (w * scale),
                                       y0 + (h * scale), scale, scale);
            }
            pixels <<= 1;
        }
    }
    display->cursor_x += (x_advance * scale);
}

/**
 * @brief Returns the reversed version of the byte.
 *
 * @param byte Byte to be reversed.
 * @return Reversed byte.
 */
static uint8_t h_reverse_byte(uint8_t byte) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        result = (uint8_t)(result << 1) | (byte & 1);
        byte >>= 1;
    }
    return result;
}

/*----------------------------------------------------------------------------*/
/*------------------------------ Init Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Initializes the ssd1306_display structure as well as the display.
 *
 * @note
 * - If you've already called this function at least once and want to
 * re-initialize the display, use ssd1306_reinit() instead.
 *
 * - The display will reset to default configurations. These can be modified
 * individually from the header file.
 *
 * - Any ongoing scrolls will be disabled (limitation of the driver chip).
 *
 * - The display will be updated (limitation of the driver chip).
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param i2c_address 7-bit address of the display.
 * @param display_type Type of the display (128x32 or 128x64).
 * @param array Pointer to the array that'll be used as the buffer for the
 * display. Use the macros in the header file to declare an array with the
 * appropriate size according to the display type.
 * @param i2c_write Pointer to the function that writes a stream of data onto
 * the I2C bus.
 *
 * The first byte of the data will always be the I2C write address
 * (i2c_address << 1) of the display. Note that, this isn't the original 7-bit
 * address, but the I2C write command for that address.
 *
 * This is to enable support for various I2C libraries, some of which don't have
 * a sequential write, or address-data separate write functions. If your I2C
 * library functions require the address separately:
 *
 * 1-) Read the first byte and if required, shift it to the right by one to get
 * the 7-bit address.
 *
 * 2-) Advance the data pointer by one, and decrement the length by one, and
 * send the data with your library function.
 */
void ssd1306_init(struct ssd1306_display *display, uint8_t i2c_address,
                  enum ssd1306_display_type display_type, uint8_t *array,
                  void (*i2c_write)(uint8_t *data, uint16_t length)) {
    /*
     * The actual data (draw) buffer starts with a 2 byte offset. The first two
     * bytes are reserved for "I2C address" and "data mode". This way the whole
     * I2C transmission (data_buffer_ptr - 2) can be sent to the write function.
     *
     * The same also applies for the command buffer. The first two bytes are
     * reserved for "I2C address" and "cmd mode". This way the whole I2C
     * transmission (cmd_buffer_ptr - 2) can be sent to the write function.
     * Beware of the max command length (ssd1306_display.cmd_memory[]).
     *
     * NEVER modify the addresses of data_buffer and cmd_buffer!
     */
    array[1] = SSD1306_CONTROL_DATA;
    display->data_buffer = &array[2];

    display->cmd_memory[1] = SSD1306_CONTROL_CMD;
    display->cmd_buffer = &display->cmd_memory[2];

    display->i2c_address = (uint8_t)(i2c_address << 1); /* Write only */
    display->i2c_write = i2c_write;
    display->display_type = display_type;

    /* Rest of the structure is initialized here */
    ssd1306_reinit(display);
}

/**
 * @brief Re-initializes the display.
 *
 * @note
 * - Same as calling ssd1306_init, but without initializing the structure again.
 * Do NOT use this function if you haven't called ssd1306_init() at least once.
 *
 * - The display will reset to default configurations. These can be modified
 * individually through the header file.
 *
 * - Any ongoing scrolls will be disabled (limitation of the driver chip).
 *
 * - The display will be updated (limitation of the driver chip).
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_reinit(struct ssd1306_display *display) {
    /*
     * Some commands are omitted here since their default state is the same as
     * intended, and the library never changes them.
     *
     * The display functions are also called in case the user re-programs or
     * re-inits the display.
     */

    /* Avoid corruption */
    ssd1306_display_scroll_disable(display);

    /* Avoid random flickering */
    ssd1306_display_enable(display, false);

    uint8_t *cmd_buffer = display->cmd_buffer;

    cmd_buffer[0] = SSD1306_CMD_SET_MUX_RATIO;
    if (display->display_type)
        cmd_buffer[1] = 0x3F;
    else
        cmd_buffer[1] = 0x1F;
    h_send_cmd_buffer(display, 2);

    cmd_buffer[0] = SSD1306_CMD_SET_COM_CONFIGURATION;
    if (display->display_type)
        cmd_buffer[1] = 0x12;
    else
        cmd_buffer[1] = 0x02;
    h_send_cmd_buffer(display, 2);

    cmd_buffer[0] = SSD1306_CMD_SET_VERTICAL_SCROLL_AREA;
    cmd_buffer[1] = 0x00;
    if (display->display_type)
        cmd_buffer[2] = 0x40;
    else
        cmd_buffer[2] = 0x20;
    h_send_cmd_buffer(display, 3);

    cmd_buffer[0] = SSD1306_CMD_SET_PAGE_ADDRESS; /* Resets address ptr */
    cmd_buffer[1] = 0x00;
    if (display->display_type)
        cmd_buffer[2] = 0x07;
    else
        cmd_buffer[2] = 0x03;
    h_send_cmd_buffer(display, 3);

    cmd_buffer[0] = SSD1306_CMD_SET_COLUMN_ADDRESS; /* Resets address ptr */
    cmd_buffer[1] = 0x00;
    cmd_buffer[2] = 0x7F;
    h_send_cmd_buffer(display, 3);

    cmd_buffer[0] = SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE;
    cmd_buffer[1] = 0x00;
    h_send_cmd_buffer(display, 2);

    cmd_buffer[0] = SSD1306_CMD_SET_DIV_RATIO_AND_FREQ;
    cmd_buffer[1] = 0xF0;
    h_send_cmd_buffer(display, 2);

    cmd_buffer[0] = SSD1306_CMD_SET_CHARGE_PUMP;
    cmd_buffer[1] = 0x14;
    h_send_cmd_buffer(display, 2);

    ssd1306_display_brightness(display, SSD1306_DEFAULT_BRIGHTNESS);
    ssd1306_display_fully_on(display, SSD1306_DEFAULT_FULLY_ON);
    ssd1306_display_inverse(display, SSD1306_DEFAULT_INVERSE);
    ssd1306_display_mirror_h(display, SSD1306_DEFAULT_MIRROR_H);
    ssd1306_display_mirror_v(display, SSD1306_DEFAULT_MIRROR_V);

#if SSD1306_DEFAULT_CLEAR_BUFFER == true && SSD1306_DEFAULT_FILL_BUFFER == false
    ssd1306_draw_clear(display);
#endif
#if SSD1306_DEFAULT_FILL_BUFFER == true
    ssd1306_draw_fill(display);
#endif
    uint8_t border_y1;
    if (display->display_type)
        border_y1 = SSD1306_DEFAULT_DRAW_BORDER_Y1_64;
    else
        border_y1 = SSD1306_DEFAULT_DRAW_BORDER_Y1_32;
    ssd1306_set_draw_border(display, SSD1306_DEFAULT_DRAW_BORDER_X0,
                            SSD1306_DEFAULT_DRAW_BORDER_Y0,
                            SSD1306_DEFAULT_DRAW_BORDER_X1, border_y1);
    ssd1306_set_buffer_mode(display, SSD1306_DEFAULT_BUFFER_MODE);
    ssd1306_set_font(display, SSD1306_DEFAULT_FONT);
    ssd1306_set_font_scale(display, SSD1306_DEFAULT_FONT_SCALE);
    ssd1306_set_cursor(display, SSD1306_DEFAULT_CURSOR_X,
                       SSD1306_DEFAULT_CURSOR_Y);

    /* Do the enable at the end */
    ssd1306_display_enable(display, SSD1306_DEFAULT_ENABLE);
}

/*----------------------------------------------------------------------------*/
/*----------------------------- Display Functions ----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Updates the display with the current buffer values.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_display_update(struct ssd1306_display *display) {
    h_send_data_buffer(display);
}

/**
 * @brief Sets the brightness level of the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param brightness Brightness level [0-255]. 255 corresponds to the maximum
 * brightness.
 */
void ssd1306_display_brightness(struct ssd1306_display *display,
                                uint8_t brightness) {
    display->cmd_buffer[0] = SSD1306_CMD_SET_CONTRAST_CONTROL;
    display->cmd_buffer[1] = brightness;
    h_send_cmd_buffer(display, 2);
}

/**
 * @brief Enables or disables the display.
 *
 * @note When enabled, the display is operated as normal. When disabled, the
 * screen remains black, regardless of the contents (the contents are not lost).
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_enabled `true` to enable; `false` to disable.
 */
void ssd1306_display_enable(struct ssd1306_display *display, bool is_enabled) {
    if (is_enabled)
        display->cmd_buffer[0] = SSD1306_CMD_DISPLAY_ON;
    else
        display->cmd_buffer[0] = SSD1306_CMD_DISPLAY_OFF;
    h_send_cmd_buffer(display, 1);
}

/**
 * @brief Enables or disables the fully-on feature of the display.
 *
 * @note When enabled, all pixels of the display are turned on, regardless of
 * the contents (the contents are not lost).
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_enabled 'true' to enable; 'false' to disable.
 */
void ssd1306_display_fully_on(struct ssd1306_display *display,
                              bool is_enabled) {
    if (is_enabled)
        display->cmd_buffer[0] = SSD1306_CMD_ENTIRE_DISPLAY_ON_ENABLED;
    else
        display->cmd_buffer[0] = SSD1306_CMD_ENTIRE_DISPLAY_ON_DISABLED;
    h_send_cmd_buffer(display, 1);
}

/**
 * @brief Enables or disables the inverse feature of the display.
 *
 * @note When enabled, the display contents are inverted, meaning pixels that
 * are on will appear off, and vice versa.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_enabled 'true' to enable; 'false' to disable.
 */
void ssd1306_display_inverse(struct ssd1306_display *display, bool is_enabled) {
    if (is_enabled)
        display->cmd_buffer[0] = SSD1306_CMD_INVERSE_ENABLED;
    else
        display->cmd_buffer[0] = SSD1306_CMD_INVERSE_DISABLED;
    h_send_cmd_buffer(display, 1);
}

/**
 * @brief Enables or disables the horizontal mirror feature of the display.
 *
 * @note When enabled, the display contents are shown horizontally mirrored.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_enabled 'true' to enable; 'false' to disable.
 */
void ssd1306_display_mirror_h(struct ssd1306_display *display,
                              bool is_enabled) {
    if (is_enabled)
        display->cmd_buffer[0] = SSD1306_CMD_SEGMENT_REMAP_ENABLED;
    else
        display->cmd_buffer[0] = SSD1306_CMD_SEGMENT_REMAP_DISABLED;
    h_send_cmd_buffer(display, 1);

    /* Only effects subsequent data */
    ssd1306_display_update(display);
}

/**
 * @brief Enables or disables the vertical mirror feature of the display.
 *
 * @note When enabled, the display contents are shown vertically mirrored.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_enabled 'true' to enable; 'false' to disable.
 */
void ssd1306_display_mirror_v(struct ssd1306_display *display,
                              bool is_enabled) {
    if (is_enabled)
        display->cmd_buffer[0] = SSD1306_CMD_SCAN_REMAP_ENABLED;
    else
        display->cmd_buffer[0] = SSD1306_CMD_SCAN_REMAP_DISABLED;
    h_send_cmd_buffer(display, 1);
}

/**
 * @brief Starts a continuous horizontal or diagonal scroll.
 *
 * @note
 * Limitations of the driver chip:
 *
 * - The display will be updated, and any subsequent updates will cause the
 * contents of the display to be corrupted. To prevent corruption, call
 * ssd1306_display_scroll_disable() before updating.
 *
 * - Vertical scrolling is not supported.
 *
 * - Diagonal scrolling will not work on 128x64 displays.
 *
 * - The vertical aspect of the diagonal scroll will always be upwards.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_left 'true' to scroll left; 'false' to scroll right.
 * @param is_diagonal 'true' to scroll diagonally; 'false' to scroll
 * horizontally.
 * @param interval Interval between each scroll. Values higher than 7 will loop
 * down the list:
 *
 *  - 0 -> 5 frames
 *
 *  - 1 -> 64 frames
 *
 *  - 2 -> 128 frames
 *
 *  - 3 -> 256 frames
 *
 *  - 4 -> 3 frames
 *
 *  - 5 -> 4 frames
 *
 *  - 6 -> 25 frames
 *
 *  - 7 -> 2 frames
 */
void ssd1306_display_scroll_enable(struct ssd1306_display *display,
                                   bool is_left, bool is_diagonal,
                                   uint8_t interval) {
    /* Data-sheet p46 */
    ssd1306_display_scroll_disable(display);

    uint8_t *cmd_buffer = display->cmd_buffer;
    uint8_t cmd_length;

    /* Common command values */
    cmd_buffer[1] = 0x00;
    cmd_buffer[2] = 0x00;
    cmd_buffer[3] = interval;
    if (display->display_type)
        cmd_buffer[4] = 0x07;
    else
        cmd_buffer[4] = 0x03;

    /* Horizontal and diagonal scroll commands are separate */
    if (is_diagonal) {
        if (is_left)
            cmd_buffer[0] = SSD1306_CMD_SET_SCROLL_DIAGONAL_LEFT;
        else
            cmd_buffer[0] = SSD1306_CMD_SET_SCROLL_DIAGONAL_RIGHT;
        cmd_buffer[5] = 0x01;
        cmd_buffer[6] = SSD1306_CMD_SCROLL_ENABLE;
        cmd_length = 7;
    } else {
        if (is_left)
            cmd_buffer[0] = SSD1306_CMD_SET_SCROLL_JUST_LEFT;
        else
            cmd_buffer[0] = SSD1306_CMD_SET_SCROLL_JUST_RIGHT;
        cmd_buffer[5] = 0x00;
        cmd_buffer[6] = 0xFF;
        cmd_buffer[7] = SSD1306_CMD_SCROLL_ENABLE;
        cmd_length = 8;
    }
    h_send_cmd_buffer(display, cmd_length);
}

/**
 * @brief Stops an ongoing scroll.
 *
 * @note
 * - The display will be updated (limitation of the driver chip).
 *
 * - To start a scroll, call ssd1306_display_scroll_enable().
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_display_scroll_disable(struct ssd1306_display *display) {
    display->cmd_buffer[0] = SSD1306_CMD_SCROLL_DISABLE;
    h_send_cmd_buffer(display, 1);

    /* Data-sheet p46 */
    ssd1306_display_update(display);
}

/*----------------------------------------------------------------------------*/
/*------------------------------ Draw Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Clears the entire buffer (all pixels off).
 *
 * @note
 * - Ignores buffer mode (draw/clear).
 *
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_draw_clear(struct ssd1306_display *display) {
    uint16_t buffer_size;
    if (display->display_type)
        buffer_size = SSD1306_BUFFER_SIZE_64;
    else
        buffer_size = SSD1306_BUFFER_SIZE_32;
    for (uint16_t i = 0; i < buffer_size; i++) {
        display->data_buffer[i] = 0x00;
    }
}

/**
 * @brief Fills the entire buffer (all pixels on).
 *
 * @note
 * - Ignores buffer mode (draw/clear).
 *
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_draw_fill(struct ssd1306_display *display) {
    uint16_t buffer_size;
    if (display->display_type)
        buffer_size = SSD1306_BUFFER_SIZE_64;
    else
        buffer_size = SSD1306_BUFFER_SIZE_32;
    for (uint16_t i = 0; i < buffer_size; i++) {
        display->data_buffer[i] = 0xFF;
    }
}

/**
 * @brief Inverts the entire buffer (all pixels flipped).
 *
 * @note
 * - Ignores buffer mode (draw/clear).
 *
 * - Ignores draw border.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_draw_invert(struct ssd1306_display *display) {
    uint16_t buffer_size;
    if (display->display_type)
        buffer_size = SSD1306_BUFFER_SIZE_64;
    else
        buffer_size = SSD1306_BUFFER_SIZE_32;
    for (uint16_t i = 0; i < buffer_size; i++) {
        display->data_buffer[i] = ~display->data_buffer[i];
    }
}

/**
 * @brief Horizontally mirrors the entire buffer.
 *
 * @note
 * - Ignores buffer mode (draw/clear).
 *
 * - Ignores draw border.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_draw_mirror_h(struct ssd1306_display *display) {
    uint8_t page_last;
    if (display->display_type)
        page_last = 7;
    else
        page_last = 3;

    uint8_t middle = (SSD1306_X_MAX + 1) >> 1;
    uint8_t temp;
    uint8_t *byte_first_ptr;
    uint8_t *byte_last_ptr;
    for (uint8_t page = 0; page <= page_last; page++) {
        byte_first_ptr = &display->data_buffer[SSD1306_PAGE_OFFSETS[page]];
        byte_last_ptr = byte_first_ptr + SSD1306_X_MAX;

        for (uint8_t i = 0; i < middle; i++) {
            temp = *byte_first_ptr;
            *byte_first_ptr = *byte_last_ptr;
            *byte_last_ptr = temp;
            byte_first_ptr++;
            byte_last_ptr--;
        }
    }
}

/**
 * @brief Vertically mirrors the entire buffer.
 *
 * @note
 * - Ignores buffer mode (draw/clear).
 *
 * - Ignores draw border.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_draw_mirror_v(struct ssd1306_display *display) {
    uint8_t page_last;
    uint8_t swap_counter;
    if (display->display_type) {
        page_last = 7;
        swap_counter = 4;
    } else {
        page_last = 3;
        swap_counter = 2;
    }

    uint8_t temp;
    uint8_t *byte_first_ptr;
    uint8_t *byte_last_ptr;
    for (uint8_t i = 0; i <= SSD1306_X_MAX; i++) {
        byte_first_ptr = &display->data_buffer[i];
        byte_last_ptr = byte_first_ptr + SSD1306_PAGE_OFFSETS[page_last];

        for (uint8_t i = 0; i < swap_counter; i++) {
            temp = h_reverse_byte(*byte_first_ptr);
            *byte_first_ptr = h_reverse_byte(*byte_last_ptr);
            *byte_last_ptr = temp;

            byte_first_ptr += SSD1306_PAGE1_OFFSET;
            byte_last_ptr -= SSD1306_PAGE1_OFFSET;
        }
    }
}

/**
 * @brief Shifts the buffer contents to the right by one pixel.
 *
 * @note
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_rotated `true` to enable rotation; `false` to just shift. When
 * rotation is enabled, pixels that shift off-screen reappear on the opposite
 * side. When rotation is disabled, pixels that shift off-screen are clipped,
 * and the value for new pixels entering the screen is determined by the buffer
 * mode.
 */
void ssd1306_draw_shift_right(struct ssd1306_display *display,
                              bool is_rotated) {
    uint8_t page_last;
    if (display->display_type)
        page_last = 7;
    else
        page_last = 3;

    uint8_t temp;
    uint8_t *byte_ptr;
    for (uint8_t page = 0; page <= page_last; page++) {
        byte_ptr = &display->data_buffer[SSD1306_PAGE_OFFSETS[page]];
        byte_ptr += SSD1306_X_MAX;
        temp = *byte_ptr;

        for (uint8_t i = 0; i < SSD1306_X_MAX; i++) {
            *byte_ptr = *(byte_ptr - 1);
            byte_ptr--;
        }

        if (is_rotated)
            *byte_ptr = temp;
        else if (display->buffer_mode)
            *byte_ptr = 0x00;
        else
            *byte_ptr = 0xFF;
    }
}

/**
 * @brief Shifts the buffer contents to the left by one pixel.
 *
 * @note
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_rotated `true` to enable rotation; `false` to just shift. When
 * rotation is enabled, pixels that shift off-screen reappear on the opposite
 * side. When rotation is disabled, pixels that shift off-screen are clipped,
 * and the value for new pixels entering the screen is determined by the buffer
 * mode.
 */
void ssd1306_draw_shift_left(struct ssd1306_display *display, bool is_rotated) {
    uint8_t page_last;
    if (display->display_type)
        page_last = 7;
    else
        page_last = 3;

    uint8_t temp;
    uint8_t *byte_ptr;
    for (uint8_t page = 0; page <= page_last; page++) {
        byte_ptr = &display->data_buffer[SSD1306_PAGE_OFFSETS[page]];
        temp = *byte_ptr;

        for (uint8_t i = 0; i < SSD1306_X_MAX; i++) {
            *byte_ptr = *(byte_ptr + 1);
            byte_ptr++;
        }

        if (is_rotated)
            *byte_ptr = temp;
        else if (display->buffer_mode)
            *byte_ptr = 0x00;
        else
            *byte_ptr = 0xFF;
    }
}

/**
 * @brief Shifts the buffer contents upward by one pixel.
 *
 * @note
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_rotated `true` to enable rotation; `false` to just shift. When
 * rotation is enabled, pixels that shift off-screen reappear on the opposite
 * side. When rotation is disabled, pixels that shift off-screen are clipped,
 * and the value for new pixels entering the screen is determined by the buffer
 * mode.
 */
void ssd1306_draw_shift_up(struct ssd1306_display *display, bool is_rotated) {
    uint8_t page_last;
    if (display->display_type)
        page_last = 7;
    else
        page_last = 3;

    uint8_t very_top_bit;
    if (!is_rotated) {
        if (display->buffer_mode)
            very_top_bit = 0x00;
        else
            very_top_bit = 0x80;
    }

    uint8_t *byte_ptr;
    uint8_t *byte_next_ptr;
    uint8_t top_bit;
    for (uint8_t i = 0; i <= SSD1306_X_MAX; i++) {
        byte_ptr = &display->data_buffer[i];

        if (is_rotated) {
            if (*byte_ptr & 1)
                very_top_bit = 0x80;
            else
                very_top_bit = 0x00;
        }

        for (uint8_t page = 0; page < page_last; page++) {
            byte_next_ptr = byte_ptr + SSD1306_PAGE1_OFFSET;
            if (*byte_next_ptr & 1)
                top_bit = 0x80;
            else
                top_bit = 0x00;
            *byte_ptr = (uint8_t)(*byte_ptr >> 1) + top_bit;
            byte_ptr = byte_next_ptr;
        }
        *byte_ptr = (uint8_t)(*byte_ptr >> 1) + very_top_bit;
    }
}

/**
 * @brief Shifts the buffer contents downward by one pixel.
 *
 * @note
 * - Ignores draw border.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param is_rotated `true` to enable rotation; `false` to just shift. When
 * rotation is enabled, pixels that shift off-screen reappear on the opposite
 * side. When rotation is disabled, pixels that shift off-screen are clipped,
 * and the value for new pixels entering the screen is determined by the buffer
 * mode.
 */
void ssd1306_draw_shift_down(struct ssd1306_display *display, bool is_rotated) {
    uint8_t page_last;
    if (display->display_type)
        page_last = 7;
    else
        page_last = 3;

    uint8_t very_bottom_bit;
    if (!is_rotated) {
        if (display->buffer_mode)
            very_bottom_bit = 0;
        else
            very_bottom_bit = 1;
    }

    uint8_t *byte_ptr;
    uint8_t *byte_next_ptr;
    uint8_t bottom_bit;
    for (uint8_t i = 0; i <= SSD1306_X_MAX; i++) {
        byte_ptr = &display->data_buffer[i] + SSD1306_PAGE_OFFSETS[page_last];

        if (is_rotated) {
            if (*byte_ptr & 0x80)
                very_bottom_bit = 1;
            else
                very_bottom_bit = 0;
        }

        for (uint8_t page = 0; page < page_last; page++) {
            byte_next_ptr = byte_ptr - SSD1306_PAGE1_OFFSET;
            if (*byte_next_ptr & 0x80)
                bottom_bit = 1;
            else
                bottom_bit = 0;
            *byte_ptr = (uint8_t)(*byte_ptr << 1) + bottom_bit;
            byte_ptr = byte_next_ptr;
        }
        *byte_ptr = (uint8_t)(*byte_ptr << 1) + very_bottom_bit;
    }
}

/**
 * @brief Draws a pixel at the specified coordinates.
 *
 * @note
 * - Clears the pixel instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x x-coordinate of the pixel. Any value out of bounds will be clipped.
 * @param y y-coordinate of the pixel. Any value out of bounds will be clipped.
 */
void ssd1306_draw_pixel(struct ssd1306_display *display, int16_t x, int16_t y) {
    if (!h_are_coords_in_border(display, x, y))
        return;

    /* x > 0 and y > 0 after above check */
    uint16_t index = SSD1306_PAGE_OFFSETS[y >> 3] + (uint16_t)x;
    uint8_t mask = (uint8_t)(1 << (y & 7));
    if (display->buffer_mode)
        display->data_buffer[index] |= mask;
    else
        display->data_buffer[index] &= ~mask;
}

/**
 * @brief Draws a horizontal line starting from the specified coordinates and
 * extending to the specified length.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the line. A positive value extends the line to the
 * right, while a negative value extends it to the left.
 */
void ssd1306_draw_line_h(struct ssd1306_display *display, int16_t x0,
                         int16_t y0, int16_t width) {
    int16_t xi;
    if (width < 0) {
        width = -width;
        xi = -1;
    } else {
        xi = 1;
    }

    for (; width > 0; width--) {
        ssd1306_draw_pixel(display, x0, y0);
        x0 += xi;
    }
}

/**
 * @brief Draws a vertical line starting from the specified coordinates and
 * extending to the specified length.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param height Height of the line. A positive value extends the line downward,
 * while a negative value extends it upward.
 */
void ssd1306_draw_line_v(struct ssd1306_display *display, int16_t x0,
                         int16_t y0, int16_t height) {
    int16_t yi;
    if (height < 0) {
        height = -height;
        yi = -1;
    } else {
        yi = 1;
    }

    for (; height > 0; height--) {
        ssd1306_draw_pixel(display, x0, y0);
        y0 += yi;
    }
}

/**
 * @brief Draws a a line between the specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the first point.
 * @param y0 y-coordinate of the first point.
 * @param x1 x-coordinate of the second point.
 * @param y1 y-coordinate of the second point.
 */
void ssd1306_draw_line(struct ssd1306_display *display, int16_t x0, int16_t y0,
                       int16_t x1, int16_t y1) {
    int16_t dx, dy, D, yi, temp;
    uint8_t is_swapped;

    /* Swap coordinates if slope > 1 (compensated when drawing) */
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    if (dy > dx) {
        temp = x0;
        x0 = y0;
        y0 = temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        is_swapped = true;
    } else {
        is_swapped = false;
    }

    /* Make sure x0 < x1 */
    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }

    /* Draw the line */
    dx = x1 - x0;
    dy = y1 - y0;
    D = -((uint16_t)dx >> 1); /* In case not optimized, dx is always > 0 */
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    } else {
        yi = 1;
    }
    for (; x0 <= x1; x0++) {
        if (is_swapped)
            ssd1306_draw_pixel(display, y0, x0);
        else
            ssd1306_draw_pixel(display, x0, y0);
        D += dy;
        if (D > 0) {
            D -= dx;
            y0 += yi;
        }
    }
}

/**
 * @brief Draws a triangle between the specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the first point.
 * @param y0 y-coordinate of the first point.
 * @param x1 x-coordinate of the second point.
 * @param y1 y-coordinate of the second point.
 * @param x2 x-coordinate of the third point.
 * @param y2 y-coordinate of the third point.
 */
void ssd1306_draw_triangle(struct ssd1306_display *display, int16_t x0,
                           int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                           int16_t y2) {
    ssd1306_draw_line(display, x0, y0, x1, y1);
    ssd1306_draw_line(display, x1, y1, x2, y2);
    ssd1306_draw_line(display, x2, y2, x0, y0);
}

/**
 * @brief Draws a filled triangle between the specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the first point.
 * @param y0 y-coordinate of the first point.
 * @param x1 x-coordinate of the second point.
 * @param y1 y-coordinate of the second point.
 * @param x2 x-coordinate of the third point.
 * @param y2 y-coordinate of the third point.
 */
void ssd1306_draw_triangle_fill(struct ssd1306_display *display, int16_t x0,
                                int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                                int16_t y2) {
    int16_t dx01, dy01, dx02, dy02, dx12, dy12;
    int16_t y, xa, xb, dxa, dxb, width;
    int16_t temp;

    /* Sort the coordinates by y position */
    if (y0 > y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    if (y1 > y2) {
        temp = y1;
        y1 = y2;
        y2 = temp;
        temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y0 > y1) {
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }

    /*
     * Find the left most and right most coordinates
     * If all coordinates are on the same line, return to avoid /0
     */
    if (y0 == y2) {
        if (x0 < x1) {
            xa = x0;
            xb = x1;
        } else {
            xa = x1;
            xb = x0;
        }

        if (x2 < xa)
            xa = x2;
        if (x2 > xb)
            xb = x2;

        ssd1306_draw_line_h(display, xa, y0, xb - xa + 1);
        return;
    }

    /* Initialize the deltas */
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;

    /*
     * Draw the upper triangle (flat bottom)
     * (if y0 == y1, loop is skipped so no /0)
     * (if y1 == y2, draw the y1 line as well)
     */
    if (y1 == y2)
        y1++;
    dxa = 0;
    dxb = 0;
    for (y = y0; y < y1; y++) {
        xa = x0 + (dxa / dy01);
        xb = x0 + (dxb / dy02);
        dxa += dx01;
        dxb += dx02;

        width = xb - xa;
        if (width < 0)
            width--;
        else
            width++;
        ssd1306_draw_line_h(display, xa, y, width);
    }

    /*
     * Draw the lower triangle (flat top)
     * If y1 == y2, line is already drawn above, so return
     */
    if (y1 == y2)
        return;
    dxa = 0;
    for (; y <= y2; y++) {
        xa = x1 + (dxa / dy12);
        xb = x0 + (dxb / dy02);
        dxa += dx12;
        dxb += dx02;

        width = xb - xa;
        if (width < 0)
            width--;
        else
            width++;
        ssd1306_draw_line_h(display, xa, y, width);
    }
}

/**
 * @brief Draws a rectangle starting from the specified coordinates and
 * extending to the specified lengths.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * downward, while a negative value extends it upward.
 */
void ssd1306_draw_rect(struct ssd1306_display *display, int16_t x0, int16_t y0,
                       int16_t width, int16_t height) {
    if (width == 0 || height == 0)
        return;

    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }
    ssd1306_draw_line_h(display, x0, y0, width);
    ssd1306_draw_line_h(display, x0, y0 + height - 1, width);
    ssd1306_draw_line_v(display, x0, y0, height);
    ssd1306_draw_line_v(display, x0 + width - 1, y0, height);
}

/**
 * @brief Draws a filled rectangle starting from the specified coordinates and
 * extending to the specified lengths.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * downward, while a negative value extends it upward.
 */
void ssd1306_draw_rect_fill(struct ssd1306_display *display, int16_t x0,
                            int16_t y0, int16_t width, int16_t height) {
    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }

    while (height > 0) {
        height--;
        ssd1306_draw_line_h(display, x0, y0 + height, width);
    }
}

/**
 * @brief Draws a rectangle with round corners starting from the specified
 * coordinates and extending to the specified lengths.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * downward, while a negative value extends it upward.
 * @param r Radius of the corner arcs. If the given radius is too large, it will
 * be limited to the maximum value possible. Passing zero or a negative value
 * will result in a normal rectangle.
 */
void ssd1306_draw_rect_round(struct ssd1306_display *display, int16_t x0,
                             int16_t y0, int16_t width, int16_t height,
                             int16_t r) {
    if (width == 0 || height == 0)
        return;

    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }

    int16_t r_max;
    if (width < height)
        r_max = width >> 1; /* Width is always positive */
    else
        r_max = height >> 1; /* Height is always positive */

    if (r < 0)
        r = 0;
    else if (r > r_max)
        r = r_max;

    int16_t width_h = width - r - r;
    int16_t height_v = height - r - r;
    ssd1306_draw_arc(display, x0 + width - r - 1, y0 + r, r, 0x01);
    ssd1306_draw_arc(display, x0 + r, y0 + r, r, 0x2);
    ssd1306_draw_arc(display, x0 + r, y0 + height - r - 1, r, 0x4);
    ssd1306_draw_arc(display, x0 + width - r - 1, y0 + height - r - 1, r, 0x8);

    ssd1306_draw_line_h(display, x0 + r, y0, width_h);
    ssd1306_draw_line_h(display, x0 + r, y0 + height - 1, width_h);
    ssd1306_draw_line_v(display, x0, y0 + r, height_v);
    ssd1306_draw_line_v(display, x0 + width - 1, y0 + r, height_v);
}

/**
 * @brief Draws a filled rectangle with round corners starting from the
 * specified coordinates and extending to the specified lengths.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * downward, while a negative value extends it upward.
 * @param r Radius of the corner arcs. If the given radius is too large, it will
 * be limited to the maximum value possible. Passing zero or a negative value
 * will result in a normal rectangle.
 */
void ssd1306_draw_rect_round_fill(struct ssd1306_display *display, int16_t x0,
                                  int16_t y0, int16_t width, int16_t height,
                                  int16_t r) {
    if (width == 0 || height == 0)
        return;

    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }

    int16_t r_max;
    if (width < height)
        r_max = width >> 1; /* Width is always positive */
    else
        r_max = height >> 1; /* Height is always positive */

    if (r < 0)
        r = 0;
    else if (r > r_max)
        r = r_max;

    int16_t width_h = width - r - r;
    int16_t height_v = height - r - r;
    ssd1306_draw_arc_fill(display, x0 + width - r - 1, y0 + r, r, 0x1);
    ssd1306_draw_arc_fill(display, x0 + r, y0 + r, r, 0x2);
    ssd1306_draw_arc_fill(display, x0 + r, y0 + height - r - 1, r, 0x4);
    ssd1306_draw_arc_fill(display, x0 + width - r - 1, y0 + height - r - 1, r,
                          0x8);

    for (int16_t i = 0; i <= r; i++) {
        ssd1306_draw_line_h(display, x0 + r, y0 + i, width_h);
    }
    for (int16_t i = 0; i <= r; i++) {
        ssd1306_draw_line_h(display, x0 + r, y0 + height - 1 - i, width_h);
    }
    for (int16_t i = 0; i < height_v; i++) {
        ssd1306_draw_line_h(display, x0, y0 + r + i, width);
    }
}

/**
 * @brief Draws quadrant arcs with the specified radius, centered at the
 * specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the arc center.
 * @param y0 y-coordinate of the arc center.
 * @param r Radius of the arc. Negative values are ignored.
 * @param quadrants A 4-bit value where each bit represents a quadrant. Only the
 * right most 4-bits are checked. The most significant bit (MSB) represents
 * quadrant 4, while the least significant bit (LSB) represents quadrant 1. Set
 * the corresponding bits to enable drawing for those quadrants. For example,
 * '0b0101' enables drawing for quadrants 3 and 1.
 */
void ssd1306_draw_arc(struct ssd1306_display *display, int16_t x0, int16_t y0,
                      int16_t r, uint8_t quadrants) {
    if (r < 0)
        return;

    if (quadrants & 0b1100)
        ssd1306_draw_pixel(display, x0, y0 + r);
    if (quadrants & 0b0011)
        ssd1306_draw_pixel(display, x0, y0 - r);
    if (quadrants & 0b1001)
        ssd1306_draw_pixel(display, x0 + r, y0);
    if (quadrants & 0b0110)
        ssd1306_draw_pixel(display, x0 - r, y0);

    int16_t f_middle, delta_e, delta_se, x, y;

    /* Initialize the middle point and delta values, start from (0, r) */
    f_middle = 1 - r; /* Simplified from "5/4-r" */
    delta_e = 3;
    delta_se = -(r + r) + 5;
    x = 0;
    y = r;

    /* Iterate from the top of the circle to the x=y line */
    while (x < y) {
        if (f_middle < 0) {
            f_middle += delta_e;
            delta_se += 2;
        } else {
            f_middle += delta_se;
            delta_se += 4;
            y--;
        }
        delta_e += 2;
        x++;

        /* Draw using 8-way symmetry */
        if (quadrants & 0b0001) {
            ssd1306_draw_pixel(display, (x0 + x), (y0 - y));
            ssd1306_draw_pixel(display, (x0 + y), (y0 - x));
        }
        if (quadrants & 0b0010) {
            ssd1306_draw_pixel(display, (x0 - x), (y0 - y));
            ssd1306_draw_pixel(display, (x0 - y), (y0 - x));
        }
        if (quadrants & 0b0100) {
            ssd1306_draw_pixel(display, (x0 - x), (y0 + y));
            ssd1306_draw_pixel(display, (x0 - y), (y0 + x));
        }
        if (quadrants & 0b1000) {
            ssd1306_draw_pixel(display, (x0 + x), (y0 + y));
            ssd1306_draw_pixel(display, (x0 + y), (y0 + x));
        }
    }
}

/**
 * @brief Draws filled quadrant arcs with the specified radius, centered at the
 * specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the arc center.
 * @param y0 y-coordinate of the arc center.
 * @param r Radius of the arc. Negative values are ignored.
 * @param quadrants A 4-bit value where each bit represents a quadrant. Only the
 * right most 4-bits are checked. The most significant bit (MSB) represents
 * quadrant 4, while the least significant bit (LSB) represents quadrant 1. Set
 * the corresponding bits to enable drawing for those quadrants. For example,
 * '0b0101' enables drawing for quadrants 3 and 1.
 */
void ssd1306_draw_arc_fill(struct ssd1306_display *display, int16_t x0,
                           int16_t y0, int16_t r, uint8_t quadrants) {
    if (r < 0)
        return;

    if (quadrants & 0b1100)
        ssd1306_draw_line_v(display, x0, y0, r + 1);
    if (quadrants & 0b0011)
        ssd1306_draw_line_v(display, x0, y0, -r - 1);
    if (quadrants & 0b1001)
        ssd1306_draw_line_h(display, x0, y0, r + 1);
    if (quadrants & 0b0110)
        ssd1306_draw_line_h(display, x0, y0, -r - 1);

    int16_t f_middle, delta_e, delta_se, x, y;
    int16_t diff_1, diff_2;

    /* Initialize the middle point and delta values, start from (0, r) */
    f_middle = 1 - r; /* Simplified from "5/4-r" */
    delta_e = 3;
    delta_se = -(r + r) + 5;
    x = 0;
    y = r;

    /* Iterate from the top of the circle to the x=y line */
    while (x < y) {
        if (f_middle < 0) {
            f_middle += delta_e;
            delta_se += 2;
        } else {
            f_middle += delta_se;
            delta_se += 4;
            y--;
        }
        delta_e += 2;
        x++;

        /* Draw using 8-way symmetry */
        diff_1 = y - x + 1;
        diff_2 = -y + x - 1;
        if (quadrants & 0b0001) {
            ssd1306_draw_line_v(display, (x0 + x), (y0 - y), diff_1);
            ssd1306_draw_line_h(display, (x0 + y), (y0 - x), diff_2);
        }
        if (quadrants & 0b0010) {
            ssd1306_draw_line_v(display, (x0 - x), (y0 - y), diff_1);
            ssd1306_draw_line_h(display, (x0 - y), (y0 - x), diff_1);
        }
        if (quadrants & 0b0100) {
            ssd1306_draw_line_v(display, (x0 - x), (y0 + y), diff_2);
            ssd1306_draw_line_h(display, (x0 - y), (y0 + x), diff_1);
        }
        if (quadrants & 0b1000) {
            ssd1306_draw_line_v(display, (x0 + x), (y0 + y), diff_2);
            ssd1306_draw_line_h(display, (x0 + y), (y0 + x), diff_2);
        }
    }
}

/**
 * @brief Draws a circle with the specified radius, centered at the specified
 * coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the circle center.
 * @param y0 y-coordinate of the circle center.
 * @param r Radius of the arc. Negative values are ignored.
 */
void ssd1306_draw_circle(struct ssd1306_display *display, int16_t x0,
                         int16_t y0, int16_t r) {
    ssd1306_draw_arc(display, x0, y0, r, 0b1111);
}

/**
 * @brief Draws a filled circle with the specified radius, centered at the
 * specified coordinates.
 *
 * @note
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the circle center.
 * @param y0 y-coordinate of the circle center.
 * @param r Radius of the arc. Negative values are ignored.
 */
void ssd1306_draw_circle_fill(struct ssd1306_display *display, int16_t x0,
                              int16_t y0, int16_t r) {
    ssd1306_draw_arc_fill(display, x0, y0, r, 0b1111);
}

/**
 * @brief Draws a bitmap image starting from the specified coordinates and
 * extending down-right.
 *
 * @note
 * - XBM is the only supported bitmap format. You can convert your images to XBM
 * by using a free tool such as GIMP or by using a website (search for "Image
 * to XBM converter").
 *
 * - Most XBM converters use a brightness threshold of 50% when converting
 * images to binary. This doesn't always result in the best-looking image.
 * There are websites that allow you to set the threshold manually, such as
 * https://javl.github.io/image2cpp/. Don't forget to tick "Invert image colors"
 * and "Swap bits in byte" to match the XBM format.
 *
 * - Since XBM images are inverted by default, this function draws the inverse
 * of the images to match the original. Setting the buffer mode to clear will
 * result in drawing the inverse of the ORIGINAL image.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x0 x-coordinate of the top left pixel of the image.
 * @param y0 y-coordinate of the top left pixel of the image.
 * @param bitmap Pointer to the bitmap array.
 * @param width Width of the image in pixels. The value MUST match the bitmap
 * width, or the drawing may get corrupted and random parts of the memory may
 * be accessed. For example, for an image with a resolution of 60x40, the
 * width value should be '60'.
 * @param height Height of the image in pixels. The value MUST match the bitmap
 * height, or the drawing may get corrupted and random parts of the memory may
 * be accessed. For example, for an image with a resolution of 60x40, the
 * height value should be '40'.
 * @param has_bg 'true' to overwrite the contents in the background; 'false' to
 * draw transparent.
 */
void ssd1306_draw_bitmap(struct ssd1306_display *display, int16_t x0,
                         int16_t y0, const uint8_t *bitmap, uint16_t width,
                         uint16_t height, bool has_bg) {
    uint8_t pixels;
    for (int16_t h = 0; h < height; h++) {
        for (int16_t w = 0; w < width; w++) {
            if (!((uint8_t)w & 7)) {
                pixels = *bitmap++;
            }

            if (!(pixels & 1)) {
                ssd1306_draw_pixel(display, x0 + w, y0 + h);
            } else if (has_bg) {
                ssd1306_set_buffer_mode_inverse(display);
                ssd1306_draw_pixel(display, x0 + w, y0 + h);
                ssd1306_set_buffer_mode_inverse(display);
            }
            pixels >>= 1;
        }
    }
}

/**
 * @brief Draws a character at the current cursor coordinates.
 *
 * @note
 * - Nothing will be drawn if the display isn't setup with a font. Fonts can be
 * assigned by calling ssd1306_set_font().
 *
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - If the current font doesn't support the character, it'll be drawn as a '?'.
 *
 * - '\\n' and '\\r' are the only special non-printable characters supported.
 *
 * - Automatically advances the cursor.
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param c Character to be drawn.
 */
void ssd1306_draw_char(struct ssd1306_display *display, char c) {
    if (display->font == NULL)
        return;

    switch (c) {
    case '\n':
        display->cursor_y += (display->font->y_advance * display->font_scale);
    case '\r':
        display->cursor_x = display->cursor_x0;
        return;
    }

    if (c < display->font->first || c > display->font->last)
        c = '?';

    struct ssd1306_glyph *glyph;
    glyph = &display->font->glyph[c - display->font->first];
    h_draw_char(display, &display->font->bitmap[glyph->bitmap_offset],
                glyph->width, glyph->height, glyph->x_offset, glyph->y_offset,
                glyph->x_advance);
}

/**
 * @brief Draws a custom character at the current cursor coordinates.
 *
 * @note
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - If the current font doesn't support the character, it'll be drawn as a '?'.
 *
 * - '\\n' and '\\r' are the only special non-printable characters supported.
 *
 * - Automatically advances the cursor.
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param c Character to be drawn.
 */
void ssd1306_draw_char_custom(struct ssd1306_display *display,
                              const struct ssd1306_custom_char *c) {
    h_draw_char(display, c->bitmap, c->width, c->height, c->x_offset,
                c->y_offset, c->x_advance);
}

/**
 * @brief Draws a string at the current cursor coordinates.
 *
 * @note
 * - Expects the string to be null terminated!
 *
 * - Nothing will be drawn if the display isn't setup with a font. Fonts can be
 * assigned by calling ssd1306_set_font().
 *
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - If the current font doesn't support the character, it'll be drawn as a '?'.
 *
 * - '\\n' and '\\r' are the only special non-printable characters supported.
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param str String to be drawn.
 */
void ssd1306_draw_str(struct ssd1306_display *display, const char *str) {
    while (*str != '\0') {
        ssd1306_draw_char(display, *str++);
    }
}

/**
 * @brief Draws a 32-bit variable at the current cursor coordinates.
 *
 * @note
 * - Meant to be a lower memory alternative for ssd1306_draw_printf().
 *
 * - Nothing will be drawn if the display isn't setup with a font. Fonts can be
 * assigned by calling ssd1306_set_font().
 *
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param num 32-bit variable to be drawn.
 */
void ssd1306_draw_int32(struct ssd1306_display *display, int32_t num) {
    if (num == 0) {
        ssd1306_draw_char(display, '0');
        return;
    }

    if (num < 0) {
        ssd1306_draw_char(display, '-');
        num = -num;
    }

    uint8_t digits[10]; /* int32 max 10 digits */
    uint8_t i = 0;
    while (num > 0) {
        digits[i] = num % 10;
        num /= 10;
        i++;
    }
    while (i > 0) {
        i--;
        ssd1306_draw_char(display, ('0' + digits[i]));
    }
}

/**
 * @brief Draws a float point number at the current cursor coordinates.
 *
 * Notes:
 *
 * - Meant to be a lower memory alternative for ssd1306_draw_printf().
 *
 * - The integer part of the number must fit within a signed 32-bit range, or
 * the number will overflow! No such limit for the fractional part or the number
 * of digits shown.
 *
 * - Nothing will be drawn if the display isn't setup with a font. Fonts can be
 * assigned by calling ssd1306_set_font().
 *
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param num Float point number to be drawn.
 * @param digits Number of digits after the decimal point to be drawn.
 */
void ssd1306_draw_float(struct ssd1306_display *display, float num,
                        uint8_t digits) {
    if (num < 0.0f) {
        ssd1306_draw_char(display, '-');
        num = -num;
    }

    int32_t num_int = (int32_t)num;
    ssd1306_draw_int32(display, num_int);
    ssd1306_draw_char(display, '.');

    uint8_t d;
    num -= num_int;
    for (; digits > 0; digits--) {
        num *= 10;
        d = (uint8_t)num;
        num -= d;
        ssd1306_draw_char(display, ('0' + d));
    }
}

/**
 * @brief Draws a formatted string at the current cursor coordinates.
 *
 * @note
 * - The SD1306_PRINTF_CHAR_LIMIT macro in the header file determines how many
 * character can be printed at a time.
 *
 * - For lower memory alternatives, consider using ssd1306_draw_str(),
 * ssd1306_draw_int32(), and ssd1306_draw_float() instead.
 *
 * - Nothing will be drawn if the display isn't setup with a font. Fonts can be
 * assigned by calling ssd1306_set_font().
 *
 * - The cursor coordinates can be set by calling ssd1306_set_cursor().
 *
 * - Font characters can be scaled by calling ssd1306_set_font_scale().
 *
 * - If the current font doesn't support the character, it'll be drawn as a '?'.
 *
 * - '\\n' and '\\r' are the only special non-printable characters supported.
 *
 * - Clears the pixels instead if the buffer is in clear mode.
 *
 * - Drawing outside the border is allowed, but pixels that are out of bounds
 * will be clipped.
 *
 * - Draw functions don't update the display. Don't forget to call the
 * ssd1306_display_update() to push the buffer onto the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param format Format string.
 * @param ... Arguments for the format string.
 */
void ssd1306_draw_printf(struct ssd1306_display *display, const char *format,
                         ...) {
    char str[SSD1306_PRINTF_CHAR_LIMIT];
    va_list args;
    va_start(args, format);
    vsnprintf(str, sizeof(str), format, args);
    va_end(args);
    ssd1306_draw_str(display, str);
}

/*----------------------------------------------------------------------------*/
/*----------------------------- Setter Functions -----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Sets the drawable border of the display.
 *
 * @note
 * - Any attempts to draw pixels outside of the specified ranges will be
 * ignored.
 *
 * - Ranges exceeding the respective display type's resolution will be
 * automatically limited to the display's maximum resolution.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x_min Minimum x-coordinate before clipping.
 * @param y_min Minimum y-coordinate before clipping.
 * @param x_max Maximum x-coordinate before clipping.
 * @param y_max Maximum y-coordinate before clipping.
 */
void ssd1306_set_draw_border(struct ssd1306_display *display, uint8_t x_min,
                             uint8_t y_min, uint8_t x_max, uint8_t y_max) {
    /* Below checks are required to prevent writing to random memory! */
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    if (x_min > SSD1306_X_MAX)
        x_min = SSD1306_X_MAX;

    if (x_max > SSD1306_X_MAX)
        x_max = SSD1306_X_MAX;

    if (y_min > SSD1306_Y_MAX)
        y_min = SSD1306_Y_MAX;

    if (y_max > SSD1306_Y_MAX)
        y_max = SSD1306_Y_MAX;

    display->border_x_min = x_min;
    display->border_y_min = y_min;
    display->border_x_max = x_max;
    display->border_y_max = y_max;
}

/**
 * @brief Sets the drawable border to the respective display type's full range.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_set_draw_border_reset(struct ssd1306_display *display) {
    uint8_t SSD1306_Y_MAX;
    if (display->display_type)
        SSD1306_Y_MAX = SSD1306_Y_MAX_64;
    else
        SSD1306_Y_MAX = SSD1306_Y_MAX_32;

    display->border_x_min = 0;
    display->border_y_min = 0;
    display->border_x_max = SSD1306_X_MAX;
    display->border_y_max = SSD1306_Y_MAX;
}

/**
 * @brief Changes the buffer mode of the display (draw/clear).
 *
 * @note In draw mode, draw functions will turn the pixels on. In clear mode,
 * draw functions will turn the pixels off instead.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param mode Buffer mode to be set.
 */
void ssd1306_set_buffer_mode(struct ssd1306_display *display,
                             enum ssd1306_buffer_mode mode) {
    display->buffer_mode = mode;
}

/**
 * @brief Inverts the buffer mode of the display (draw->clear | clear->draw).
 *
 * @note In draw mode, draw functions will turn the pixels on. In clear mode,
 * draw functions will turn the pixels off instead.
 *
 * @param display Pointer to the ssd1306_display structure.
 */
void ssd1306_set_buffer_mode_inverse(struct ssd1306_display *display) {
    display->buffer_mode ^= 1;
}

/**
 * @brief Assigns a font to the display.
 *
 * @note
 * - The specified font will be used for the subsequent character drawings.
 *
 * - The only supported font format is "GFXfont" from the "Adafruit-GFX-Library"
 * on GitHub (requires minor conversions).
 *
 * - GitHub link: https://github.com/adafruit/Adafruit-GFX-Library/
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param font Font to be assigned to the display. Pass NULL for no font.
 */
void ssd1306_set_font(struct ssd1306_display *display,
                      const struct ssd1306_font *font) {
    display->font = font;
}

/**
 * @brief Configures the font scaling for the display.
 *
 * @note
 * - The specified scaling will be used for the subsequent character drawings.
 *
 * - Scaling is linear (new size = original size x scale).
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param scale The scaling factor. Can't be negative.
 */
void ssd1306_set_font_scale(struct ssd1306_display *display, uint8_t scale) {
    display->font_scale = scale;
}

/**
 * @brief Places the cursor at the specified coordinates.
 *
 * @note The cursor location represents the starting point for the subsequent
 * character to be drawn. This point is typically the bottom-left corner of the
 * character; but this isn't exact, as the font can define offsets in any
 * direction.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x x-coordinate for the cursor.
 * @param y y-coordinate for the cursor.
 */
void ssd1306_set_cursor(struct ssd1306_display *display, int16_t x, int16_t y) {
    display->cursor_x0 = x;
    display->cursor_x = x;
    display->cursor_y = y;
}

/*----------------------------------------------------------------------------*/
/*----------------------------- Getters Functions ----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Returns the assigned 7-bit I2C address of the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return The assigned 7-bit I2C address of the display. If ssd1306_init()
 * hasn't been called for the specified structure, the return value will be
 * undefined.
 */
uint8_t ssd1306_get_display_address(struct ssd1306_display *display) {
    return *(display->data_buffer - 2) >> 1;
}

/**
 * @brief Returns the assigned display type of the display (128x32 or 128x64).
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return The assigned display type of the display (128x32 or 128x64). If
 * ssd1306_init() hasn't been called for the specified structure, the return
 * value will be undefined.
 */
enum ssd1306_display_type
ssd1306_get_display_type(struct ssd1306_display *display) {
    return display->display_type;
}

/**
 * @brief Returns the current drawable border of the display.
 *
 * @note The drawable border can be set by calling ssd1306_set_draw_border().
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x_min Pointer where the minimum x-coordinate before clipping will be
 * placed.
 * @param y_min Pointer where the minimum y-coordinate before clipping will be
 * placed.
 * @param x_max Pointer where the maximum x-coordinate before clipping will be
 * placed.
 * @param y_max Pointer where the maximum y-coordinate before clipping will be
 * placed.
 */
void ssd1306_get_draw_border(struct ssd1306_display *display, uint8_t *x_min,
                             uint8_t *y_min, uint8_t *x_max, uint8_t *y_max) {
    *x_min = display->border_x_min;
    *y_min = display->border_y_min;
    *x_max = display->border_x_max;
    *y_max = display->border_y_max;
}

/**
 * @brief Returns the current buffer mode of the display (draw/clear).
 *
 * @note The buffer mode can be set by calling ssd1306_set_buffer_mode().
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return The current buffer mode of the display (draw/clear).
 */
enum ssd1306_buffer_mode
ssd1306_get_buffer_mode(struct ssd1306_display *display) {
    return display->buffer_mode;
}

/**
 * @brief Returns the current font assigned to the display.
 *
 * @note Fonts can be assigned by calling ssd1306_set_font().
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return The current font assigned to the display. Returns NULL if no font is
 * assigned.
 */
const struct ssd1306_font *ssd1306_get_font(struct ssd1306_display *display) {
    return display->font;
}

/**
 * @brief Returns the current font scale of the display.
 *
 * @note The font scale can be set by calling ssd1306_set_font_scale().
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return The current font scale of the display.
 */
uint8_t ssd1306_get_font_scale(struct ssd1306_display *display) {
    return display->font_scale;
}

/**
 * @brief Returns the coordinates of the current cursor location.
 *
 * @note The cursor location can be set by calling ssd1306_set_cursor().
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x Pointer where the current x-coordinate will be placed.
 * @param y Pointer where the current y-coordinate will be placed.
 * @return The x-coordinate where the cursor would move after a carriage return.
 */
int16_t ssd1306_get_cursor(struct ssd1306_display *display, int16_t *x,
                           int16_t *y) {
    *x = display->cursor_x;
    *y = display->cursor_y;
    return display->cursor_x0;
}

/**
 * @brief Returns a pointer to the display buffer.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @return Pointer to the display buffer. If ssd1306_init() hasn't been called
 * for the specified structure, the return value will be undefined.
 */
uint8_t *sd1306_get_buffer(struct ssd1306_display *display) {
    return display->data_buffer - 2;
}

/**
 * @brief Returns the value of the specified pixel in the buffer.
 *
 * @note The pixel value is from the buffer, not the display.
 *
 * @param display Pointer to the ssd1306_display structure.
 * @param x x-coordinate of the pixel.
 * @param y y-coordinate of the pixel.
 * @return The value of the specified pixel ('0' or '1'). Coordinates that are
 * out of bounds will return '0' as well.
 */
uint8_t ssd1306_get_buffer_pixel(struct ssd1306_display *display, int16_t x,
                                 int16_t y) {
    if (!h_are_coords_in_border(display, x, y))
        return 0;

    /* x > 0 and y > 0 after above check */
    uint16_t index = SSD1306_PAGE_OFFSETS[y >> 3] + (uint16_t)x;
    uint8_t mask = (uint8_t)(1 << (y & 7));
    if (display->data_buffer[index] & mask)
        return 1;

    return 0;
}
