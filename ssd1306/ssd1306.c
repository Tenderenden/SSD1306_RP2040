#include "ssd1306.h"
#include <hardware/i2c.h>
#include <pico/platform/compiler.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

ssd1306_buffer_t ssd1306_buffer;
ssd1306_cfg_t ssd1306_config = {
    .address = SSD1306_ADDR,
    .i2c_port = i2c0
};

void ssd1306_write_cmds(uint8_t *cmd_buffer, uint8_t buff_len);

void ssd1306_send_cmd(uint8_t cmd){
    uint8_t cmd_buffer[] = {0x80, cmd};
    ssd1306_write_cmds(cmd_buffer, 2);
}

void ssd1306_send_data(ssd1306_buffer_t *buffer)
{
    uint8_t *data_buffer = malloc(buffer->length + 1);
    data_buffer[0] = 0x40;
    memcpy(data_buffer+1, buffer->buffer, buffer->length);
    ssd1306_write_cmds(data_buffer, (buffer->length)+1);
    free(data_buffer);
}


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
        SSD1306_DISPLAY_ON_FULL_CMD
    };

    //Initialize buffer
    ssd1306_buffer.length = SSD1306_WIDTH * SSD1306_HEIGHT;
    memset(ssd1306_buffer.buffer, 0, ssd1306_buffer.length);

    i2c_init(ssd1306_config.i2c_port, 400 * 1000);
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCK_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCK_PIN);

    for(uint8_t i=0; i<count_of(config_cmds);i++){
        ssd1306_send_cmd(config_cmds[i]);
    }
}

void ssd1306_draw(void)
{
    ssd1306_send_data(&ssd1306_buffer);
}

void ssd1306_clear_buff(void)
{
    memset(ssd1306_buffer.buffer, 0, ssd1306_buffer.length);
}

void ssd1306_fill_buff(void)
{
    memset(ssd1306_buffer.buffer, 0xFF, ssd1306_buffer.length);
}

void ssd1306_set_horizontal_mode(uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop)
{
    uint8_t cmds[] = {0x80, 0x0, 0x0, 0x0};
    
    // Set horizontal adressing mode
    cmds[1] = 0x20;
    ssd1306_write_cmds(cmds, 2);
    
    // Set column address
    cmds[1] = 0x21;
    cmds[2] = col_start; //0 - 127
    cmds[3] = col_stop; //0 - 127
    ssd1306_write_cmds(cmds, 4);
    
    // Set page address
    cmds[1] = 0x22;
    cmds[2] = page_start; //0 - 7
    cmds[3] = page_stop; //0 - 7
    ssd1306_write_cmds(cmds, 4);
}

void ssd1306_set_page_mode(uint8_t page_start_address, uint8_t lower_column, uint8_t higher_column)
{
    uint8_t cmds[] = {0x80, page_start_address, lower_column, higher_column};
    ssd1306_write_cmds(cmds, 4);
    //Set page start address 0xB0 - 0xB7
    //Set lower start column 0x00 - 0x0F
    //Set upper start column 0x10 - 0x1F
}

void ssd1306_write_cmds(uint8_t *cmd_buffer, uint8_t buff_len)
{
    i2c_write_blocking(ssd1306_config.i2c_port, ssd1306_config.address, cmd_buffer, buff_len, false);
}