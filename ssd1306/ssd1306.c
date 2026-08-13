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

ssd1306_buffer_t ssd1306_main_buffer = {
    .length = SSD1306_WIDTH * 4,
};
ssd1306_cfg_t ssd1306_config = {
    .address = SSD1306_ADDR,
    .i2c_port = i2c0
};


void ssd1306_init() {
    printf("Initializing OLED\n");
    uint8_t config_cmds[]={
        SSD1306_SET_DISPLAY_OFF_CMD,
        SSD1306_SET_DISPLAY_CLK_DIVIDE_FREQ_CMD,
        ((8<<4)|0),
        SSD1306_SET_MULTIPLEX_RATIO_CMD,
        SSD1306_MUX_RATIO_32,
        SSD1306_SET_DISPLAY_OFFSET_CMD,
        0x0U,
        SSD1306_SET_DISPLAY_START_LINE_CMD,
        SSD1306_SET_SEGMENT_REMAP_NORMAL_CMD,
        SSD1306_SET_COM_SCAN_DIR_NORMAL_CMD,
        SSD1306_SET_COM_HW_CONFIG_CMD,
        0x02U,
        SSD1306_SET_CONTRAST_CMD,
        0x7F,
        SSD1306_DISPLAY_ON_RAM_CMD,
        SSD1306_SET_NORMAL_DISPLAY_CMD,
        SSD1306_CHARGE_PUMP_SETTING_CMD,
        0x14U,
        SSD1306_SET_DISPLAY_ON_CMD,
        SSD1306_DISPLAY_ON_FULL_CMD,
        SSD1306_DISPLAY_ON_RAM_CMD
    };

    //Initialize buffer for full screen
    ssd1306_main_buffer.length = (SSD1306_WIDTH * SSD1306_HEIGHT) / 8;
    memset(ssd1306_main_buffer.buffer, 0, ssd1306_main_buffer.length);

    i2c_init(ssd1306_config.i2c_port, 400 * 1000);
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCK_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCK_PIN);

    for(uint8_t i=0; i<count_of(config_cmds);i++){
        ssd1306_send_one_cmd(config_cmds[i]);
    }

    //Set horizontal mode for full screen
    ssd1306_set_horizontal_mode(0, SSD1306_WIDTH-1, 0, (SSD1306_HEIGHT/8)-1);
}

void ssd1306_send_one_cmd(uint8_t cmd){
    uint8_t cmd_buffer[] = {0x80, cmd};
    ssd1306_write_cmds(cmd_buffer, 2, false);
}

void ssd1306_send_data(ssd1306_buffer_t *buffer)
{
    uint8_t *data_buffer = malloc(buffer->length + 1);
    data_buffer[0] = 0x40;
    memcpy(data_buffer+1, buffer->buffer, buffer->length);
    ssd1306_write_cmds(data_buffer, (buffer->length)+1, false);
    free(data_buffer);
}

void ssd1306_draw(void)
{
    ssd1306_send_data(&ssd1306_main_buffer);
}

void ssd1306_fill_buff(uint8_t val)
{
    memset(ssd1306_main_buffer.buffer, val, ssd1306_main_buffer.length);
}

void ssd1306_set_horizontal_mode(uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop)
{
    uint8_t cmds[] = {0x00, 0x0, 0x0, 0x0};
    
    // Set horizontal adressing mode
    cmds[1] = 0x20;
    cmds[2] = 0x00;
    ssd1306_write_cmds(cmds, 3, false);
    
    // Set column address
    cmds[1] = 0x21;
    cmds[2] = col_start; //0 - 127
    cmds[3] = col_stop; //0 - 127
    ssd1306_write_cmds(cmds, 4, false);
    
    // Set page address
    cmds[1] = 0x22;
    cmds[2] = page_start; //0 - 7
    cmds[3] = page_stop; //0 - 7
    ssd1306_write_cmds(cmds, 4, false);
}

void ssd1306_set_page_mode(uint8_t page_start_address, uint8_t lower_column, uint8_t higher_column)
{
    uint8_t cmds[] = {0x00, page_start_address, lower_column, higher_column};
    ssd1306_write_cmds(cmds, 4, false);
    //Set page start address 0xB0 - 0xB7 max is B3, cause of smaller screen
    //Set lower start column 0x00 - 0x0F
    //Set upper start column 0x10 - 0x1F
}

void ssd1306_write_cmds(uint8_t *cmd_buffer, size_t buff_len, bool nostop)
{
    i2c_write_blocking(ssd1306_config.i2c_port, ssd1306_config.address, cmd_buffer, buff_len, nostop);
}

void ssd1306_set_pixel(uint8_t x, uint8_t y, bool pixel_on)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    uint16_t index = (page * SSD1306_WIDTH) + x;

    if (pixel_on) {
        ssd1306_main_buffer.buffer[index] |= (1 << bit);
    } else {
        ssd1306_main_buffer.buffer[index] &= ~(1 << bit);
    }
    // printf("Col: %d \nPage: %d\nBit: %d\n", column, page, bit);
}

void test_segments(uint8_t col_start, uint8_t col_count, uint8_t page_start, uint8_t page_count)
{
    ssd1306_buffer_t test_buffer;
    test_buffer.buffer[0] = 0;
    test_buffer.length = 1;

    //Check column mapping
    for(uint8_t i=col_start; i<col_start+col_count;i++)
    {
        for(uint8_t x=0; x<8; x++)
        {
            printf("column: %d, page: %d, value: %d\n", i, page_start, x);
            ssd1306_set_horizontal_mode(i, i+1, page_start, page_start+1);
            test_buffer.buffer[0] = (1 << x);
            ssd1306_send_data(&test_buffer);
            sleep_ms(400);
        }
    }
    
    //Check page mapping
}
