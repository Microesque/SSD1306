#include "SSD1306.h"


/*----------------------------------------------------------------------------*/
/*-------------------------- SSD1306 Address Macros --------------------------*/
/*----------------------------------------------------------------------------*/

#define SSD1306_CONTROL_CMD                     0x00
#define SSD1306_CONTROL_DATA                    0x40
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA    0xA3  // Follow by 2-byte setup
#define SSD1306_CMD_SET_MUX_RATIO               0xA8  // Follow by 1-byte setup
#define SSD1306_CMD_SET_MEMORY_ACCESSING_MODE   0x20  // Follow by 1-byte setup
#define SSD1306_CMD_SET_COM_CONFIGURATION       0xDA  // Follow by 1-byte setup
#define SSD1306_CMD_SET_COLUMN_ADDRESS          0x21  // Follow by 2-byte setup
#define SSD1306_CMD_SET_PAGE_ADDRESS            0x22  // Follow by 2-byte setup
#define SSD1306_CMD_SET_DIV_RATIO_AND_FREQ      0xD5  // Follow by 1-byte setup
#define SSD1306_CMD_SET_CHARGE_PUMP             0x8D  // Follow by 1-byte setup
#define SSD1306_CMD_DISPLAY_ON                  0xAF
#define SSD1306_CMD_DISPLAY_OFF                 0xAE
#define SSD1306_CMD_SET_CONTRAST_CONTROL        0x81  // Follow by 1-byte setup
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
#define SSD1306_CMD_SET_SCROLL_H_RIGHT          0x29 // Follow by 5-byte setup
#define SSD1306_CMD_SET_SCROLL_H_LEFT           0x2A // Follow by 5-byte setup


/*----------------------------------------------------------------------------*/
/*------------------------------ Library Macros ------------------------------*/
/*----------------------------------------------------------------------------*/

#define SSD1306_BUFFER_MAX_32 511
#define SSD1306_BUFFER_MAX_64 1023

#define SSD1306_P0_OFFSET 0
#define SSD1306_P1_OFFSET 128
#define SSD1306_P2_OFFSET 256
#define SSD1306_P3_OFFSET 384
#define SSD1306_P4_OFFSET 512
#define SSD1306_P5_OFFSET 640
#define SSD1306_P6_OFFSET 768
#define SSD1306_P7_OFFSET 896
static const uint16_t SSD1306_PAGE_OFFSETS[] = {
    SSD1306_P0_OFFSET, SSD1306_P1_OFFSET, SSD1306_P2_OFFSET, SSD1306_P3_OFFSET,
    SSD1306_P4_OFFSET, SSD1306_P5_OFFSET, SSD1306_P6_OFFSET, SSD1306_P7_OFFSET
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
 * @brief Sends commands to the display.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param data Pointer to an array of bytes to be sent to the display.
 * @param length Number of bytes in the data array to send.
 */
void SSD1306_write_cmd(SSD1306_T* display, uint8_t* data, uint8_t length) {
    display->I2C_start();
    display->I2C_write((uint8_t)(display->I2C_address << 1));
    display->I2C_write(SSD1306_CONTROL_CMD);
    for (uint8_t i = 0; i < length; i++) {
        display->I2C_write(data[i]);
    }
    display->I2C_stop();
}

/**
 * @brief Sends data to the display.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param data Pointer to an array of bytes to be sent to the display.
 * @param length Number of bytes in the data array to send.
 */
void SSD1306_write_data(SSD1306_T* display, uint8_t* data, uint16_t length) {
    display->I2C_start();
    display->I2C_write((uint8_t)(display->I2C_address << 1));
    display->I2C_write(SSD1306_CONTROL_DATA);
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
    cmd_buffer[0] = SSD1306_CMD_SET_VERTICAL_SCROLL_AREA;
    cmd_buffer[1] = 0x00; // Start 0
    cmd_buffer[2] = 0x20; // End 32
    SSD1306_write_cmd(display, cmd_buffer, 3);
    cmd_buffer[0] = SSD1306_CMD_SET_MUX_RATIO;
    cmd_buffer[1] = 0x1F; // Mux of 32 (Mux = 0x1F+1)
    SSD1306_write_cmd(display, cmd_buffer, 2);
    cmd_buffer[0] = SSD1306_CMD_SET_MEMORY_ACCESSING_MODE;
    cmd_buffer[1] = 0x00; // Horizontal addressing mode   
    SSD1306_write_cmd(display, cmd_buffer, 2);
    cmd_buffer[0] = SSD1306_CMD_SET_COM_CONFIGURATION;
    cmd_buffer[1] = 0x02; // Sequential, disable remap  
    SSD1306_write_cmd(display, cmd_buffer, 2);
    cmd_buffer[0] = SSD1306_CMD_SET_COLUMN_ADDRESS;
    cmd_buffer[1] = 0x00; // Start 0
    cmd_buffer[2] = 0x7F; // End 127  
    SSD1306_write_cmd(display, cmd_buffer, 3);
    cmd_buffer[0] = SSD1306_CMD_SET_PAGE_ADDRESS;
    cmd_buffer[1] = 0x00; // Start 0
    cmd_buffer[2] = 0x03; // End 3
    SSD1306_write_cmd(display, cmd_buffer, 3);
    cmd_buffer[0] = SSD1306_CMD_SET_DIV_RATIO_AND_FREQ;
    cmd_buffer[1] = 0xF0; // Div ratio = 1, Freq = max. 
    SSD1306_write_cmd(display, cmd_buffer, 2);
    cmd_buffer[0] = SSD1306_CMD_SET_CHARGE_PUMP;
    cmd_buffer[1] = 0x14; // Enable charge pump  
    SSD1306_write_cmd(display, cmd_buffer, 2);
    
    // Configure defaults.
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
    SSD1306_write_data(display, display->buffer, SSD1306_BUFFER_SIZE_32);
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
    SSD1306_write_cmd(display, cmd, 2);
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
    SSD1306_write_cmd(display, &cmd, 1);
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
    SSD1306_write_cmd(display, &cmd, 1);
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
    SSD1306_write_cmd(display, &cmd, 1);
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
    SSD1306_write_cmd(display, &cmd, 1);
    
    // These commands only effect subsequent data, so update the display.
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
    SSD1306_write_cmd(display, &cmd, 1);
}

/**
 * @brief Starts up a continuous horizontal or diagonal scroll.
 * 
 * Vertical scrolling is not available, and the vertical aspect of the diagonal
 * scroll will always be upwards. These are limitation of the driver chip
 * itself.
 * 
 * Note:
 * 
 * - Any previous scroll setup will be overwritten, and the scroll will start
 * from the original location.
 * 
 * - Automatically updates the screen with the current buffer values
 * (limitation of the driver chip).
 * 
 * - Any updates to the display while there's an ongoing scroll
 * will restart the scrolling.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @param is_lr "true" to scroll left; "false" to scroll right.
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
void SSD1306_display_scroll_enable(SSD1306_T* display, bool is_lr,
                                   bool is_diagonal, uint8_t interval) {
    // Interval can't be bigger than 7
    if (interval > 7) {return;}
    
    // Disable scrolling first (datasheet p44)
    SSD1306_display_scroll_disable(display);
    
    uint8_t cmd[7];
    if (is_lr) {
        cmd[0] = SSD1306_CMD_SET_SCROLL_H_LEFT;
    }
    else {
        cmd[0] = SSD1306_CMD_SET_SCROLL_H_RIGHT;
    }
    cmd[1]= 0x00; // Dummy write (datasheet p28)
    cmd[2]= 0x00; // Start page address 0 (first page)
    cmd[3]= interval;
    cmd[4]= 0x03; // End page address 3 (last page)
    if (is_diagonal) {
        cmd[5]= 0x01; // Set vertical scrolling offset to 1
    }
    else {
        cmd[5]= 0x00; // Set vertical scrolling offset to 0
    }
    cmd[6]= SSD1306_CMD_SCROLL_ENABLE;
    SSD1306_write_cmd(display, cmd, 7);
}

/**
 * @brief Stops an ongoing scroll. You can start a scroll by calling
 * "SSD1306_display_scroll_enable()".
 * 
 * @param display Pointer to an SSD1306_T structure.
 */
void SSD1306_display_scroll_disable(SSD1306_T* display) {
    uint8_t cmd = SSD1306_CMD_SCROLL_DISABLE;
    SSD1306_write_cmd(display, &cmd, 1);
    
    // Update the display (datasheet p36)
    SSD1306_display_update(display);
}










/*----------------------------------------------------------------------------*/
/*------------------------------ Draw Functions ------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Returns the current display buffer mode (draw/clear).
 * 
 * When set to "draw", draw functions will turn the pixels on. When set to
 * "clear", draw functions will turn the pixels off instead.
 * 
 * @param display Pointer to an SSD1306_T structure.
 * @return Current buffer mode.
 */
SSD1306_BufferMode SSD1306_draw_get_mode(SSD1306_T* display) {
    return display->buffer_mode;
}

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
    for (uint16_t i = 0; i < SSD1306_BUFFER_MAX_32; i++) {
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
    for (uint16_t i = 0; i < SSD1306_BUFFER_MAX_32; i++) {
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
    if (x > SSD1306_X_MAX_32) {return;}
    if (y > SSD1306_Y_MAX_32) {return;}
    
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
