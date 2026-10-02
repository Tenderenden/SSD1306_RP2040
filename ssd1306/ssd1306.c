#include "ssd1306.h"
#include <hardware/i2c.h>
#include <pico/platform/compiler.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

uint8_t ssd1306_config_cmds[] = {
        SSD1306_SET_DISPLAY_OFF_CMD,
        SSD1306_SET_DISPLAY_CLK_DIVIDE_FREQ_CMD,
        SSD1306_CLK_FREQ_VAL,
        SSD1306_SET_MULTIPLEX_RATIO_CMD,
        SSD1306_MUX_RATIO_32,
        SSD1306_SET_DISPLAY_OFFSET_CMD,
        SSD1306_DISP_OFFSET_VAL,
        SSD1306_SET_DISPLAY_START_LINE_CMD,
        SSD1306_SET_SEGMENT_REMAP_NORMAL_CMD,
        SSD1306_SET_COM_SCAN_DIR_NORMAL_CMD,
        SSD1306_SET_COM_HW_CONFIG_CMD,
        SSD1306_HW_CONFIG_VAL,
        SSD1306_SET_CONTRAST_CMD,
        SSD1306_CONTRAST_VAL,
        SSD1306_DISPLAY_ON_RAM_CMD,
        SSD1306_SET_NORMAL_DISPLAY_CMD,
        SSD1306_CHARGE_PUMP_SETTING_CMD,
        SSD1306_CHARGE_PUMP_VAL,
        SSD1306_SET_DISPLAY_ON_CMD,
        SSD1306_DISPLAY_ON_FULL_CMD,
        SSD1306_DISPLAY_ON_RAM_CMD
    };


void ssd1306_init(ssd1306_t* scrn) {
    printf("Initializing OLED\n");

    //Initialize buffer for full screen
    
    scrn->buffer.length = SSD1306_WIDTH * SSD1306_HEIGHT / 8;
    memset(scrn->buffer.buffer + 1, 0, scrn->buffer.length);
    //Add control byte to the first place in buffer
    scrn->buffer.buffer[0] = 0x40;

    i2c_init(scrn->config.i2c_port, scrn->config.baud);
    gpio_set_function(scrn->config.sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scrn->config.sck_pin, GPIO_FUNC_I2C);
    gpio_pull_up(scrn->config.sda_pin);
    gpio_pull_up(scrn->config.sck_pin);

    for(uint8_t i=0; i<count_of(ssd1306_config_cmds);i++){
        ssd1306_send_one_cmd(scrn, ssd1306_config_cmds[i]);
    }

    //Set horizontal mode for full screen
    ssd1306_set_mem_addr_mode(scrn, ADDR_MODE_HORIZONTAL, 0, SSD1306_WIDTH-1, 0, (SSD1306_HEIGHT/8)-1);
}

void ssd1306_send_one_cmd(ssd1306_t* scrn, uint8_t cmd){
    uint8_t cmd_buffer[] = {0x80, cmd};
    ssd1306_write_cmds(&(scrn->config), cmd_buffer, 2, false);
}

void ssd1306_send_data(ssd1306_t* scrn, ssd1306_buffer_t *buffer)
{
    ssd1306_write_cmds(&(scrn->config), buffer->buffer, (buffer->length)+1, false);
}

void ssd1306_draw(ssd1306_t* scrn)
{
    ssd1306_send_data(scrn, &scrn->buffer);
}

void ssd1306_fill_buff(ssd1306_t* scrn, uint8_t val)
{
    memset(scrn->buffer.buffer+1, val, scrn->buffer.length);
}

void ssd1306_set_mem_addr_mode(ssd1306_t* scrn, ssd1306_addr_mode_t mode, uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop)
{
    uint8_t cmds[] = {0x00, 0x0, 0x0, 0x0};
    
    // Set horizontal adressing mode
    cmds[1] = SSD1306_ADDRSET_MEMORY_ADDRESSING_MODE_CMD;
    cmds[2] = mode;
    ssd1306_write_cmds(&(scrn->config), cmds, 3, false);
    
    // Set column address
    cmds[1] = SSD1306_SET_COLUMN_ADDRESS_CMD;
    cmds[2] = col_start; //0 - 127
    cmds[3] = col_stop; //0 - 127
    ssd1306_write_cmds(&(scrn->config), cmds, 4, false);
    
    // Set page address
    cmds[1] = SSD1306_SET_PAGE_ADDRESS_CMD;
    cmds[2] = page_start; //0 - 7
    cmds[3] = page_stop; //0 - 7
    ssd1306_write_cmds(&(scrn->config), cmds, 4, false);
}

void ssd1306_set_page_mode(ssd1306_t* scrn, uint8_t page_start_address, uint8_t lower_column, uint8_t higher_column)
{
    uint8_t cmds[] = {0x00, page_start_address, lower_column, higher_column};
    ssd1306_write_cmds(&(scrn->config), cmds, 4, false);
    //Set page start address 0xB0 - 0xB7 max is B3, cause of smaller screen
    //Set lower start column 0x00 - 0x0F
    //Set upper start column 0x10 - 0x1F
}

void ssd1306_write_cmds(ssd1306_cfg_t* cfg, uint8_t *cmd_buffer, size_t buff_len, bool nostop)
{
    i2c_write_blocking(cfg->i2c_port, cfg->address, cmd_buffer, buff_len, nostop);
}

void ssd1306_set_pixel(ssd1306_t* scrn, uint8_t x, uint8_t y, bool pixel_on)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    uint16_t index = 1 + (page * SSD1306_WIDTH) + x;

    if (pixel_on) {
        scrn->buffer.buffer[index] |= (1 << bit);
    } else {
        scrn->buffer.buffer[index] &= ~(1 << bit);
    }
    // printf("Col: %d \nPage: %d\nBit: %d\n", column, page, bit);
}

bool ssd1306_get_pixel(ssd1306_t* scrn, uint8_t x, uint8_t y)
{
    bool ret_val;
    uint8_t page = y / 8;
    uint16_t index = 1 + (page * SSD1306_WIDTH) + x;
    ret_val = (bool)((scrn->buffer.buffer[index]) & (1 << (y % 8)));
    return ret_val;
}

void ssd1306_line(ssd1306_t *scrn, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    /* source: https://zingl.github.io/bresenham.html */
    int dx = abs((int)x1-(int)x0);
    int sx = (x0<x1) ? 1: -1;
    int dy = -abs((int)y1-(int)y0);
    int sy = (y0<y1) ? 1: -1;
    int err = dx + dy;
    int e2;

    while (true) {
        ssd1306_set_pixel(scrn, x0, y0, true);
        if(x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

void ssd1306_h_line(ssd1306_t *scrn, uint8_t x, uint8_t y, uint8_t length)
{
    for(uint8_t i=x; i<(x+length); i++)
    {
        ssd1306_set_pixel(scrn, i, y, true);
    }
}

void ssd1306_v_line(ssd1306_t *scrn, uint8_t x, uint8_t y, uint8_t length)
{
    for(uint8_t i=y; i<(y+length); i++)
    {
        ssd1306_set_pixel(scrn, x, i, true);
    }
}

void test_segments(uint8_t col_start, uint8_t col_count, uint8_t page_start, uint8_t page_count)
{
    ssd1306_t test_screen;
    ssd1306_buffer_t test_buffer;
    test_buffer.buffer[0] = 0;
    test_buffer.length = 1;

    //Check column mapping
    for(uint8_t i=col_start; i<col_start+col_count;i++)
    {
        for(uint8_t x=0; x<8; x++)
        {
            printf("column: %d, page: %d, value: %d\n", i, page_start, x);
            ssd1306_set_mem_addr_mode(&test_screen, ADDR_MODE_HORIZONTAL, i, i+1, page_start, page_start+1);
            test_buffer.buffer[0] = (1 << x);
            ssd1306_send_data(&test_screen, &test_buffer);
            sleep_ms(400);
        }
    }
    
    //Check page mapping
}
