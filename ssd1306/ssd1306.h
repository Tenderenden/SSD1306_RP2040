#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include <stdint.h>

#define SSD1306_ADDR 0x3CU
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

#define SSD1306_SDA_PIN 4
#define SSD1306_SCK_PIN 5

/* Fundamental commands */
#define SSD1306_SET_CONTRAST_CMD 0x81U
#define SSD1306_DISPLAY_ON_RAM_CMD 0xA4U
#define SSD1306_DISPLAY_ON_FULL_CMD 0xA5U
#define SSD1306_SET_NORMAL_DISPLAY_CMD 0xA6U
#define SSD1306_SET_INVERSE_DISPLAY_CMD 0xA7U
#define SSD1306_SET_DISPLAY_ON_CMD 0xAFU
#define SSD1306_SET_DISPLAY_OFF_CMD 0xAEU

#define SSD1306_ADDRSET_MEMORY_ADDRESSING_MODE_CMD 0x20U
#define SSD1306_ADDRHORIZONTAL_MODE 0x0U
#define SSD1306_ADDRVERTICAL_MODE 0x1U
#define SSD1306_ADDRPAGE_MODE 0x2U

#define SSD1306_SET_COLUMN_ADDRESS_CMD 0x21U
#define SSD1306_SET_PAGE_ADDRESS_CMD 0x22U
#define SSD1306_SET_DISPLAY_START_LINE_CMD 0x40U
#define SSD1306_SET_SEGMENT_REMAP_NORMAL_CMD 0xA0U
#define SSD1306_SET_SEGMENT_REMAP_REMAPPED_CMD 0xA1U

#define SSD1306_SET_MULTIPLEX_RATIO_CMD 0xA8U
#define SSD1306_MUX_RATIO_32 0x20U

#define SSD1306_SET_COM_SCAN_DIR_NORMAL_CMD 0xC0U
#define SSD1306_SET_COM_SCAN_DIR_REMAPPED_CMD 0xC8U
#define SSD1306_SET_DISPLAY_OFFSET_CMD 0xD3U
#define SSD1306_SET_COM_HW_CONFIG_CMD 0xDAU
#define SSD1306_SET_DISPLAY_CLK_DIVIDE_FREQ_CMD 0xD5U

#define SSD1306_CHARGE_PUMP_SETTING_CMD 0x8D

typedef struct ssd1306_buffer{
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT];
    size_t length;
}ssd1306_buffer_t;

typedef struct ssd1306_cfg{
    uint32_t address;
    i2c_inst_t *i2c_port;
}ssd1306_cfg_t;

void ssd1306_init(void);
void ssd1306_draw(void);
void ssd1306_send_cmd(uint8_t cmd);
void ssd1306_send_data(ssd1306_buffer_t *buffer);
void ssd1306_clear_buff(void);
void ssd1306_fill_buff(void);
void ssd1306_set_horizontal_mode(uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop);
void ssd1306_set_page_mode(uint8_t page_start_address, uint8_t lower_column, uint8_t higher_column);

#endif /* SSD1306_H */