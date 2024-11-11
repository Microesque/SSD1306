#include "SSD1306.h"


/*----------------------------------------------------------------------------*/
/*-------------------------- SSD1306 Address Macros --------------------------*/
/*----------------------------------------------------------------------------*/

#define SSD1306_CONTROL_CMD                     0x00
#define SSD1306_CONTROL_DATA                    0x40
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA    0xA3  // Follow by 2-byte setup
#define SSD1306_CMD_SET_MUX_RATIO               0xA8  // Follow by 1-byte setup
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE  0x20  // Follow by 1-byte setup
#define SSD1306_CMD_SET_COM_CONFIGURATION       0xDA  // Follow by 1-byte setup
#define SSD1306_CMD_SET_PAGE_ADDRESS            0x22  // Follow by 2-byte setup
#define SSD1306_CMD_SET_DIV_RATIO_AND_FREQ      0xD5  // Follow by 1-byte setup
#define SSD1306_CMD_SET_CHARGE_PUMP             0x8D  // Follow by 1-byte setup
#define SSD1306_CMD_SET_CONTRAST_CONTROL        0x81  // Follow by 1-byte setup
#define SSD1306_CMD_SET_SCROLL_JUST_RIGHT       0x26  // Follow by 6-byte setup
#define SSD1306_CMD_SET_SCROLL_JUST_LEFT        0x27  // Follow by 6-byte setup
#define SSD1306_CMD_SET_SCROLL_DIAGONAL_RIGHT   0x29  // Follow by 5-byte setup
#define SSD1306_CMD_SET_SCROLL_DIAGONAL_LEFT    0x2A  // Follow by 5-byte setup
#define SSD1306_CMD_DISPLAY_ON                  0xAF
#define SSD1306_CMD_DISPLAY_OFF                 0xAE
#define SSD1306_CMD_ENTIRE_DISPLAY_ON_ENABLED   0xA5
#define SSD1306_CMD_ENTIRE_DISPLAY_ON_DISABLED  0xA4
#define SSD1306_CMD_INVERSE_ENABLED             0xA7
#define SSD1306_CMD_INVERSE_DISABLED            0xA6
#define SSD1306_CMD_SEGMENT_REMAP_ENABLED       0xA1
#define SSD1306_CMD_SEGMENT_REMAP_DISABLED      0xA0
#define SSD1306_CMD_SCAN_REMAP_ENABLED          0xC8
#define SSD1306_CMD_SCAN_REMAP_DISABLED         0xC0
#define SSD1306_CMD_SCROLL_DISABLE              0x2E
#define SSD1306_CMD_SCROLL_ENABLE               0x2F


/*----------------------------------------------------------------------------*/
/*--------------------------- Library Enums/Macros ---------------------------*/
/*----------------------------------------------------------------------------*/

typedef enum {
    SSD1306_WRITE_CMD,
    SSD1306_WRITE_DATA
} SSD1306_WRITE_MODE;

#define SSD1306_PAGE0_OFFSET 0
#define SSD1306_PAGE1_OFFSET 128
#define SSD1306_PAGE2_OFFSET 256
#define SSD1306_PAGE3_OFFSET 384
#define SSD1306_PAGE4_OFFSET 512
#define SSD1306_PAGE5_OFFSET 640
#define SSD1306_PAGE6_OFFSET 768
#define SSD1306_PAGE7_OFFSET 896
static const uint16_t SSD1306_PAGE_OFFSETS[] = {
    SSD1306_PAGE0_OFFSET, SSD1306_PAGE1_OFFSET,
    SSD1306_PAGE2_OFFSET, SSD1306_PAGE3_OFFSET,
    SSD1306_PAGE4_OFFSET, SSD1306_PAGE5_OFFSET,
    SSD1306_PAGE6_OFFSET, SSD1306_PAGE7_OFFSET
};










/*----------------------------------------------------------------------------*/
/*-------------------------- Communication Functions -------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Initializes an SSD1306_T structure as well as its physical display.
 * 
 * Defaults to:
 * 
 *  - Display is in draw mode.
 * 
 *  - Both the display and the buffer are cleared.
 * 
 *  - Display on.
 * 
 *  - Display brightness: 127.
 * 
 *  - Vertical mirroring disabled.
 * 
 *  - Horizontal mirroring disabled.
 * 
 *  - Inverse mode disabled.
 * 
 *  - Fully on mode disabled.
 * 
 *  - Scrolling disabled.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param buffer Pointer to an array to be used as the buffer for that
 * display. Use the macros in the header file to declare an array according
 * to your display type.
 * @param display_type Type of your display (128x32 or 128x64).
 * @param I2C_address 7-bit address of your display.
 * @param I2C_start Pointer to your function to initiate an i2c start
 * condition.
 * @param I2C_write Pointer to your function to send an 8-bit data on the i2c
 * bus.
 * @param I2C_stop Pointer to your function to initiate a i2c stop condition.
 */
void SSD1306_init(SSD1306_T* display,
                  uint8_t* buffer,
                  SSD1306_DisplayType display_type,
                  uint8_t I2C_address,
                  void (*I2C_start)(void),
                  uint8_t (*I2C_write)(uint8_t),
                  void (*I2C_stop)(void)){
    display->buffer = buffer;
    display->display_type = display_type;
    display->I2C_address = I2C_address;
    display->I2C_start = I2C_start;
    display->I2C_write = I2C_write;
    display->I2C_stop = I2C_stop;
    SSD1306_reinit(display);
}

/**
 * @brief Sends commands or data to the display.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param write_mode Command vs data mode.
 * @param data Pointer to an array of bytes to be sent to the display.
 * @param length Number of bytes in the data array to send.
 */
static void SSD1306_write(SSD1306_T* display, SSD1306_WRITE_MODE write_mode,
                          uint8_t* data, uint16_t length) {
    display->I2C_start();
    display->I2C_write((uint8_t)(display->I2C_address << 1));
    if (write_mode) {
        display->I2C_write(SSD1306_CONTROL_DATA);
    }
    else {
        display->I2C_write(SSD1306_CONTROL_CMD);
    }
    for (uint16_t i = 0; i < length; i++) {
        display->I2C_write(data[i]);
    }
    display->I2C_stop();
}










/*----------------------------------------------------------------------------*/
/*----------------------------- Display Functions ----------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Re-initializes the display. Basically the same as calling
 * "SSD1306_init()", but without initializing the structure or having to pass
 * the other parameters.
 * 
 * Defaults to:
 * 
 *  - Display is in draw mode.
 * 
 *  - Both the display and the buffer are cleared.
 * 
 *  - Display on.
 * 
 *  - Display brightness: 127.
 * 
 *  - Vertical mirroring disabled.
 * 
 *  - Horizontal mirroring disabled.
 * 
 *  - Inverse mode disabled.
 * 
 *  - Fully on mode disabled.
 * 
 *  - Scrolling disabled.
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_reinit(SSD1306_T* display) {
    /*
     Initialize the display (from datasheet p28-32, p34-46, p64). Some commands
     are ommited here due to their reset state being the same as intended.
     */
    uint8_t cmd_buffer[3];

    // For the commands below, default values are correct for 128x64 displays
    if (display->display_type == SSD1306_DISPLAY_TYPE_32) {
        // Change page address
        cmd_buffer[0] = SSD1306_CMD_SET_PAGE_ADDRESS;
        cmd_buffer[1] = 0x00;
        cmd_buffer[2] = 0x03;
        SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 3);

        // Change mux ratio (default is correct for 128x64)
        cmd_buffer[0] = SSD1306_CMD_SET_MUX_RATIO;
        cmd_buffer[1] = 0x1F;
        SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 2);

        // Change COM configuration (default is correct for 128x64)
        cmd_buffer[0] = SSD1306_CMD_SET_COM_CONFIGURATION;
        cmd_buffer[1] = 0x02;
        SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 2);

        // Change vertical scroll area for (default is correct for 128x64)
        cmd_buffer[0] = SSD1306_CMD_SET_VERTICAL_SCROLL_AREA;
        cmd_buffer[1] = 0x00;
        cmd_buffer[2] = 0x20;
        SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 3);
    }

    // Horizontal addressing mode
    cmd_buffer[0] = SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE;
    cmd_buffer[1] = 0x00;
    SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 2);

    // Div ratio min, freq max.
    cmd_buffer[0] = SSD1306_CMD_SET_DIV_RATIO_AND_FREQ;
    cmd_buffer[1] = 0xF0;
    SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 2);

    // Enable charge pump 
    cmd_buffer[0] = SSD1306_CMD_SET_CHARGE_PUMP;
    cmd_buffer[1] = 0x14; 
    SSD1306_write(display, SSD1306_WRITE_CMD, cmd_buffer, 2);
    
    // Configure defaults for reinits
    SSD1306_display_brightness(display, 127);
    SSD1306_display_fully_on(display, false);
    SSD1306_display_inverse(display, false);
    SSD1306_display_mirror_h(display, false);
    SSD1306_display_mirror_v(display, false);
    SSD1306_display_scroll_disable(display);
    SSD1306_draw_set_mode(display, SSD1306_MODE_DRAW);
    SSD1306_draw_clear(display);
    SSD1306_display_update(display);
    SSD1306_display_enable(display, true);
}

/**
 * @brief Updates the display with the current buffer values.
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_display_update(SSD1306_T* display) {
    uint16_t buffer_size;
    if (display->display_type) {
        buffer_size = SSD1306_BUFFER_SIZE_64;
    }
    else {
        buffer_size = SSD1306_BUFFER_SIZE_32;
    }
    SSD1306_write(display, SSD1306_WRITE_DATA, display->buffer, buffer_size);
}

/**
 * @brief Sets the brightness level of the display.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param brightness Brightness level [0-255]. 255 corresponds to the maximum
 * brightness.
 */
void SSD1306_display_brightness(SSD1306_T* display, uint8_t brightness) {
    uint8_t cmd[] = {SSD1306_CMD_SET_CONTRAST_CONTROL, brightness};
    SSD1306_write(display, SSD1306_WRITE_CMD, cmd, 2);
}

/**
 * @brief When enabled, turns the display on. When disabled, the screen will
 * remain black, regardless of its contents (contents are not lost).
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_enabled "true" to enable; "false" to disable.
 */
void SSD1306_display_enable(SSD1306_T* display, bool is_enabled) {
    uint8_t cmd;
    if (is_enabled) {
        cmd = SSD1306_CMD_DISPLAY_ON;
    }
    else {
        cmd = SSD1306_CMD_DISPLAY_OFF;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
}

/**
 * @brief When enabled, all of the pixels of the display will turn on regardless
 * of its contents (contents are not lost).
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_enabled "true" to enable; "false" to disable.
 */
void SSD1306_display_fully_on(SSD1306_T* display, bool is_enabled) {
    uint8_t cmd;
    if (is_enabled) {
        cmd = SSD1306_CMD_ENTIRE_DISPLAY_ON_ENABLED;
    }
    else {
        cmd = SSD1306_CMD_ENTIRE_DISPLAY_ON_DISABLED;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
}

/**
 * @brief When enabled, the contents of the display will be shown inverted
 * (pixels that are supposed to be on will be off and vice versa).
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_enabled "true" to enable; "false" to disable.
 */
void SSD1306_display_inverse(SSD1306_T* display, bool is_enabled) {
    uint8_t cmd;
    if (is_enabled) {
        cmd = SSD1306_CMD_INVERSE_ENABLED;
    }
    else {
        cmd = SSD1306_CMD_INVERSE_DISABLED;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
}

/**
 * @brief When enabled, the contents of the display will be shown horizontally
 * mirrored.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_enabled "true" to enable; "false" to disable.
 */
void SSD1306_display_mirror_h(SSD1306_T* display, bool is_enabled) {
    uint8_t cmd;
    if (is_enabled) {
        cmd = SSD1306_CMD_SEGMENT_REMAP_ENABLED;
    }
    else {
        cmd = SSD1306_CMD_SEGMENT_REMAP_DISABLED;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
    
    // Only effect subsequent data, so update the display.
    SSD1306_display_update(display);
}

/**
 * @brief When enabled, the contents of the display will be shown vertically
 * mirrored.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_enabled "true" to enable; "false" to disable.
 */
void SSD1306_display_mirror_v(SSD1306_T* display, bool is_enabled) {
    uint8_t cmd;
    if (is_enabled) {
        cmd = SSD1306_CMD_SCAN_REMAP_ENABLED;
    }
    else {
        cmd = SSD1306_CMD_SCAN_REMAP_DISABLED;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
}

/**
 * @brief Starts up a continuous horizontal or diagonal scroll.
 * 
 * Below are all due to the limitations of the driver chip itself:
 * 
 * - Vertical scrolling by itself is not available.
 * 
 * - Diagonal scroll won't work with "128x64" displays.
 * 
 * - Vertical aspect of the diagonal scroll will always be upwards.
 * 
 * - Any previous scroll setup will be overwritten, and the scroll will start
 * from the original location.
 * 
 * - Automatically updates the screen with the current buffer values.
 * 
 * - Any updates to the display while there's an ongoing scroll will restart the
 * scrolling.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_left "true" to scroll left; "false" to scroll right.
 * @param is_diagonal "true" to scroll diagonally; "false" to scroll
 * horizontally.
 * @param interval Interval between each scroll. Must be within [0-7].
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
void SSD1306_display_scroll_enable(SSD1306_T* display, bool is_left,
                                   bool is_diagonal, uint8_t interval) {
    // Interval can't be bigger than 7
    if (interval > 7) {return;}
    
    // Disable scrolling first (datasheet p44)
    SSD1306_display_scroll_disable(display);
    
    uint8_t cmd[8];
    uint8_t cmd_length;

    // Assign the common command values
    cmd[1]= 0x00;
    cmd[2]= 0x00;
    cmd[3]= interval;
    if (display->display_type) {
        cmd[4]= 0x07;
    }
    else {
        cmd[4]= 0x03;
    }

    // Choose the right command and send
    if (is_diagonal) {
        if (is_left) {
            cmd[0] = SSD1306_CMD_SET_SCROLL_DIAGONAL_LEFT;
        }
        else {
            cmd[0] = SSD1306_CMD_SET_SCROLL_DIAGONAL_RIGHT;
        }
        cmd[5] = 0x01;
        cmd[6] = SSD1306_CMD_SCROLL_ENABLE;
        cmd_length = 7;
    }
    else {
        if (is_left) {
            cmd[0] = SSD1306_CMD_SET_SCROLL_JUST_LEFT;
        }
        else {
            cmd[0] = SSD1306_CMD_SET_SCROLL_JUST_RIGHT;
        }
        cmd[5] = 0x00;
        cmd[6] = 0xFF;
        cmd[7] = SSD1306_CMD_SCROLL_ENABLE;
        cmd_length = 8;
    }
    SSD1306_write(display, SSD1306_WRITE_CMD, cmd, cmd_length);
}

/**
 * @brief Stops an ongoing scroll. You can start a scroll by calling
 * "SSD1306_display_scroll_enable()".
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_display_scroll_disable(SSD1306_T* display) {
    uint8_t cmd = SSD1306_CMD_SCROLL_DISABLE;
    SSD1306_write(display, SSD1306_WRITE_CMD, &cmd, 1);
    
    // Update the display (datasheet p36)
    SSD1306_display_update(display);
}










/*----------------------------------------------------------------------------*/
/*------------------------------ Draw Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Changes the display buffer mode (draw/clear).
 * 
 * When set to "draw", draw functions will turn the pixels on. When set to
 * "clear", draw functions will turn the pixels off instead.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param mode Buffer mode to be set.
 */
void SSD1306_draw_set_mode(SSD1306_T* display, SSD1306_BufferMode mode) {
    display->buffer_mode = mode;
}

/**
 * @brief Clears the entire display (all pixels off).
 * 
 * Note:
 * 
 * - Ignores buffer mode (draw/clear).
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_draw_clear(SSD1306_T* display) {
    uint16_t buffer_size;
    if (display->display_type) {
        buffer_size = SSD1306_BUFFER_SIZE_64;
    }
    else {
        buffer_size = SSD1306_BUFFER_SIZE_32;
    }
    for (uint16_t i = 0; i < buffer_size; i++) {
        display->buffer[i] = 0x00;
    }
}

/**
 * @brief Fills the entire display (all pixels lit up).
 * 
 * Note:
 * 
 * - Ignores buffer mode (draw/clear).
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_draw_fill(SSD1306_T* display) {
    uint16_t buffer_size;
    if (display->display_type) {
        buffer_size = SSD1306_BUFFER_SIZE_64;
    }
    else {
        buffer_size = SSD1306_BUFFER_SIZE_32;
    }
    for (uint16_t i = 0; i < buffer_size; i++) {
        display->buffer[i] = 0xFF;
    }
}

/**
 * @brief Draws a pixel at the specified location.
 * 
 * Note:
 * 
 * - Clears the pixel instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x x coordinate of the pixel. Any value out of bounds will be clipped.
 * @param y y coordinate of the pixel. Any value out of bounds will be clipped.
 */
void SSD1306_draw_pixel(SSD1306_T* display, int16_t x, int16_t y) {    
    // Clip the coordinates that are out of bounds
    if (x < 0) {return;}
    if (y < 0) {return;}
    if (x > SSD1306_X_MAX) {return;}
    if (display->display_type) {
        if (y > SSD1306_Y_MAX_64) {return;}
    }
    else {
        if (y > SSD1306_Y_MAX_32) {return;}
    }
    
    uint16_t index;
    uint8_t mask;

    // Find the buffer index and its mask according to the given coordinates
    index = SSD1306_PAGE_OFFSETS[y >> 3] + (uint16_t)x;
    mask = (uint8_t)(1 << (y & 7));
    
    // Change the pixel according to the current buffer mode
    if (display->buffer_mode) {
        display->buffer[index] = display->buffer[index] | mask;
    }
    else {
        display->buffer[index] = display->buffer[index] & ~mask;
    }
}

/**
 * @brief Draws a horizontal line starting from the specified coordinates and
 * extending to the specified length.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the line. A positive value extends the line to the
 * right, while a negative value extends it to the left.
 */
void SSD1306_draw_line_h(SSD1306_T* display, int16_t x0, int16_t y0,
                         int16_t width) {
    int16_t xi;
    
    // Change direction if width is negative
    if (width < 0) {
        width = -width;
        xi = -1;
    }
    else {
        xi = 1;
    }
    
    // Draw the line
    for (; width > 0; width--) {
        SSD1306_draw_pixel(display, x0, y0);
        x0 += xi;
    }
}

/**
 * @brief Draws a vertical line starting from the specified coordinates and
 * extending to the specified length.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param height Height of the line. A positive value extends the line upward, 
 * while a negative value extends it downward.
 */
void SSD1306_draw_line_v(SSD1306_T* display, int16_t x0, int16_t y0,
                         int16_t height) {
    int16_t yi;
    
    // Change direction if height is negative
    if (height < 0) {
        height = -height;
        yi = -1;
    }
    else {
        yi = 1;
    }
    
    // Draw the line
    for (; height > 0; height--) {
        SSD1306_draw_pixel(display, x0, y0);
        y0 += yi;
    }
}

/**
 * @brief Draws a straight line between the specified start and end coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param x1 x-coordinate of the ending point.
 * @param y1 y-coordinate of the ending point.
 */
void SSD1306_draw_line(SSD1306_T* display, int16_t x0, int16_t y0, int16_t x1,
                       int16_t y1) {
    // Handle the straight line cases seperately
    if (x0 == x1) {
        SSD1306_draw_pixel(display, x1, y1);
        SSD1306_draw_line_v(display, x0, y0, y1 - y0);
        return;
    }
    if (y0 == y1) {
        SSD1306_draw_pixel(display, x1, y1);
        SSD1306_draw_line_h(display, x0, y0, x1 - x0);
        return;
    }
    
    int16_t dx, dy, D, yi, temp;
    uint8_t is_swapped;
    
    // Swap coordinates if slope > 1 (compensated when drawing)
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dy;
    }
    if (dy > dx) {
        temp = x0;
        x0 = y0;
        y0 = temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        is_swapped = true;
    }
    else {
        is_swapped = false;
    }
    
    // Make sure x0 < x1
    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    
    // Draw the line
    dx = x1 - x0;
    dy = y1 - y0;
    D = -((uint16_t)dx >> 1); // -dx/2 (in case not optimized, dx is always > 0)
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    else {
        yi = 1;
    }
    for (; x0 <= x1; x0++) {
        if (is_swapped) {
            SSD1306_draw_pixel(display, y0, x0);
        }
        else {
            SSD1306_draw_pixel(display, x0, y0);
        }
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
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of point-0.
 * @param y0 y-coordinate of point-0.
 * @param x1 x-coordinate of point-1.
 * @param y1 y-coordinate of point-1.
 * @param x2 x-coordinate of point-2.
 * @param y2 y-coordinate of point-2.
 */
void SSD1306_draw_triangle(SSD1306_T* display, int16_t x0, int16_t y0,
                           int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    SSD1306_draw_line(display, x0, y0, x1, y1);
    SSD1306_draw_line(display, x1, y1, x2, y2);
    SSD1306_draw_line(display, x2, y2, x0, y0);
}

/**
 * @brief Draws a filled triangle between the specified coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of point-0.
 * @param y0 y-coordinate of point-0.
 * @param x1 x-coordinate of point-1.
 * @param y1 y-coordinate of point-1.
 * @param x2 x-coordinate of point-2.
 * @param y2 y-coordinate of point-2.
 */
void SSD1306_draw_triangle_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                               int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    int16_t dx01, dy01, dx02, dy02, dx12, dy12;
    int16_t y, xa, xb, dxa, dxb, width;
    int16_t temp;
    
    // Sort the coordinates by y position
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
    
    // If all coordinates are on the same line (return to avoid /0)
    if (y0 == y2) {
        // Find the left most and right most coordinates
        if (x0 < x1) {
            xa = x0;
            xb = x1;
        }
        else {
            xa = x1;
            xb = x0;
        }
        if (x2 < xa) {
            xa = x2;
        }
        if (x2 > xb) {
            xb = x2;
        }
        
        // Draw the horizontal line
        SSD1306_draw_line_h(display, xa, y0, xb - xa + 1);
        return;
    }
    
    // Initialize the delta variables
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    
    // Draw the upper triangle (flat bottom)
    // (if y0 == y1, loop is skipped so no /0)
    // (if y1 == y2, draw the y1 line as well)
    if (y1 == y2) {
        y1++;
    }
    dxa = 0;
    dxb = 0;
    for (y = y0; y < y1; y++) {
        // Interpolate x-coordinates for the current scanline     
        xa = x0 + (dxa / dy01);
        xb = x0 + (dxb / dy02);
        dxa += dx01;
        dxb += dx02;
        
        // Draw the horizontal line for the current scanline
        width = xb - xa;
        if (width < 0) {
            width--;
        }
        else {
            width++;
        }
        SSD1306_draw_line_h(display, xa, y, width);
    }
    
    // Draw the lower triangle (flat top)
    // (if y1 == y2, line is already drawn above, so return)
    if (y1 == y2) {
        return;
    }
    dxa = 0;
    for (; y <= y2; y++) {
        // Interpolate x-coordinates for the current scanline     
        xa = x1 + (dxa / dy12);
        xb = x0 + (dxb / dy02);
        dxa += dx12;
        dxb += dx02;
        
        // Draw the horizontal line for the current scanline
        width = xb - xa;
        if (width < 0) {
            width--;
        }
        else {
            width++;
        }
        SSD1306_draw_line_h(display, xa, y, width);
    }
}

/**
 * @brief Draws a rectangle starting from the specified coordinates and
 * extending to the specified lengths.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * upward, while a negative value extends it downward.
 */
void SSD1306_draw_rect(SSD1306_T* display, int16_t x0, int16_t y0,
                       int16_t width, int16_t height) {
    // Width and height can't be 0
    if (width == 0 || height == 0) {return;}
    
    // Shift the rectangle if width or height is negative
    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }
    
    // Draw the rectangle
    SSD1306_draw_line_h(display, x0, y0, width);
    SSD1306_draw_line_h(display, x0, y0 + height - 1, width);
    SSD1306_draw_line_v(display, x0, y0, height);
    SSD1306_draw_line_v(display, x0 + width - 1, y0, height);
}

/**
 * @brief Draws a filled rectangle starting from the specified coordinates and
 * extending to the specified lengths.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the starting point.
 * @param y0 y-coordinate of the starting point.
 * @param width Width of the rectangle. A positive value extends the rectangle
 * to the right, while a negative value extends it to the left.
 * @param height Height of the rectangle. A positive value extends the rectangle
 * upward, while a negative value extends it downward.
 */
void SSD1306_draw_rect_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                            int16_t width, int16_t height) {
    // Width and height can't be 0
    if (width == 0 || height == 0) {return;}
    
    // Shift the rectangle if width or height is negative
    if (width < 0) {
        width = -width;
        x0 -= (width - 1);
    }
    if (height < 0) {
        height = -height;
        y0 -= (height - 1);
    }
    
    // Fill the rectangle
    while (height > 0) {
        height--;
        SSD1306_draw_line_h(display, x0, y0 + height, width);
    }
}

/**
 * @brief Draws quadrant arcs with the specified radius, centered at the
 * specified coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the arc center.
 * @param y0 y-coordinate of the arc center.
 * @param r Radius of the arc in pixels. Negative values are ignored.
 * @param quadrant A 4-bit value where each bit represents a quadrant. Values
 * bigger than 4-bits are ignored. The most significant bit (MSB) represents
 * quadrant-4, while the least significant bit (LSB) represents quadrant-1. Set
 * the corresponding bits to enable drawing for those quadrants. For example,
 * "0b0110" enables drawing for quadrants 2 and 3.
 */
void SSD1306_draw_arc(SSD1306_T* display, int16_t x0, int16_t y0, int16_t r,
                      uint8_t quadrant) {
    // Radius can't be negative, skip invalid quadrants
    if (r < 0) {return;}
    if (quadrant > 0b1111) {return;}
    
    // Draw the 4 way corner pixels
    if (quadrant & 0b1100) {
        SSD1306_draw_pixel(display, x0, y0 + r);
    }
    if (quadrant & 0b0011) {
        SSD1306_draw_pixel(display, x0, y0 - r);
    }
    if (quadrant & 0b1001) {
        SSD1306_draw_pixel(display, x0 + r, y0);
    }
    if (quadrant & 0b0110) {
        SSD1306_draw_pixel(display, x0 - r, y0);
    }

    int16_t f_middle, delta_e, delta_se, x, y;
    int16_t diff_1, diff_2;
    
    // Initialize the middle point and delta values, start from (0, r)
    f_middle = 1 - r;  // Simplified from "5/4-r"
    delta_e = 3;
    delta_se = -(r + r) + 5;
    x = 0;
    y = r;
    
    // Iterate from the top of the circle to the x=y line
    while (x < y) {
        if (f_middle < 0) {
            f_middle += delta_e;
            delta_se += 2;
        }
        else {
            f_middle += delta_se;
            delta_se += 4;
            y--;
        }
        delta_e += 2;
        x++;
        
        // Draw the arcs using 8-way symmetry
        diff_1 =  y - x + 1;
        diff_2 = -y + x - 1;
        if (quadrant & 0b0001) {
            SSD1306_draw_pixel(display, (x0 + x), (y0 - y));
            SSD1306_draw_pixel(display, (x0 + y), (y0 - x));
        }
        if (quadrant & 0b0010) {
            SSD1306_draw_pixel(display, (x0 - x), (y0 - y));
            SSD1306_draw_pixel(display, (x0 - y), (y0 - x));
        }
        if (quadrant & 0b0100) {
            SSD1306_draw_pixel(display, (x0 - x), (y0 + y));
            SSD1306_draw_pixel(display, (x0 - y), (y0 + x));
        }
        if (quadrant & 0b1000) {
            SSD1306_draw_pixel(display, (x0 + x), (y0 + y));
            SSD1306_draw_pixel(display, (x0 + y), (y0 + x));
        }
    }
}

/**
 * @brief Draws filled quadrant arcs with the specified radius, centered at the
 * specified coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the arc center.
 * @param y0 y-coordinate of the arc center.
 * @param r Radius of the arc in pixels. Negative values are ignored.
 * @param quadrant A 4-bit value where each bit represents a quadrant. Values
 * bigger than 4-bits are ignored. The most significant bit (MSB) represents
 * quadrant-4, while the least significant bit (LSB) represents quadrant-1. Set
 * the corresponding bits to enable drawing for those quadrants. For example,
 * "0b0110" enables drawing for quadrants 2 and 3.
 */
void SSD1306_draw_arc_fill(SSD1306_T* display, int16_t x0, int16_t y0, int16_t r,
                           uint8_t quadrant) {
    // Radius can't be negative, skip invalid quadrants
    if (r < 0) {return;}
    if (quadrant > 0b1111) {return;}
    
    // Draw the 4 way corner lines
    if (quadrant & 0b1100) {
        SSD1306_draw_line_v(display, x0, y0, r + 1);
    }
    if (quadrant & 0b0011) {
        SSD1306_draw_line_v(display, x0, y0, -r - 1);
    }
    if (quadrant & 0b1001) {
        SSD1306_draw_line_h(display, x0, y0, r + 1);
    }
    if (quadrant & 0b0110) {
        SSD1306_draw_line_h(display, x0, y0, -r - 1);
    }

    int16_t f_middle, delta_e, delta_se, x, y;
    int16_t diff_1, diff_2;
    
    // Initialize the middle point and delta values, start from (0, r)
    f_middle = 1 - r;  // Simplified from "5/4-r"
    delta_e = 3;
    delta_se = -(r + r) + 5;
    x = 0;
    y = r;
    
    // Iterate from the top of the circle to the x=y line
    while (x < y) {
        if (f_middle < 0) {
            f_middle += delta_e;
            delta_se += 2;
        }
        else {
            f_middle += delta_se;
            delta_se += 4;
            y--;
        }
        delta_e += 2;
        x++;
        
        // Fill the arcs using 8-way symmetry
        diff_1 =  y - x + 1;
        diff_2 = -y + x - 1;
        if (quadrant & 0b0001) {
            SSD1306_draw_line_v(display, (x0 + x), (y0 - y), diff_1);
            SSD1306_draw_line_h(display, (x0 + y), (y0 - x), diff_2);
        }
        if (quadrant & 0b0010) {
            SSD1306_draw_line_v(display, (x0 - x), (y0 - y), diff_1);
            SSD1306_draw_line_h(display, (x0 - y), (y0 - x), diff_1);
        }
        if (quadrant & 0b0100) {
            SSD1306_draw_line_v(display, (x0 - x), (y0 + y), diff_2);
            SSD1306_draw_line_h(display, (x0 - y), (y0 + x), diff_1);
        }
        if (quadrant & 0b1000) {
            SSD1306_draw_line_v(display, (x0 + x), (y0 + y), diff_2);
            SSD1306_draw_line_h(display, (x0 + y), (y0 + x), diff_2);
        }
    }
}

/**
 * @brief Draws a circle with the specified radius, centered at the specified
 * coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the circle center.
 * @param y0 y-coordinate of the circle center.
 * @param r Radius of the arc in pixels. Negative values are ignored.
 */
void SSD1306_draw_circle(SSD1306_T* display, int16_t x0, int16_t y0,
                         int16_t r) {
    SSD1306_draw_arc(display, x0, y0, r, 0b1111);
}


/**
 * @brief Draws a filled circle with the specified radius, centered at the
 * specified coordinates.
 * 
 * Note:
 * 
 * - Clears the pixels instead if the display is in "clear" mode.
 * 
 * - You can draw off-screen, but everything that's out of bounds will be
 * clipped.
 * 
 * - Draw functions don't update the screen. Don't forget to call the
 * "SSD1306_display_update()" to push the buffer onto the screen.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param x0 x-coordinate of the circle center.
 * @param y0 y-coordinate of the circle center.
 * @param r Radius of the arc in pixels. Negative values are ignored.
 */
void SSD1306_draw_circle_fill(SSD1306_T* display, int16_t x0, int16_t y0,
                         int16_t r) {
    SSD1306_draw_arc_fill(display, x0, y0, r, 0b1111);
}
