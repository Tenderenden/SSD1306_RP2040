#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include <errno.h>
#include <pico/types.h>
#include <stdbool.h>
#include <stdint.h>

#define SSD1306_ADDR (0x3CU)
#define SSD1306_WIDTH (128U)
#define SSD1306_HEIGHT (32U)

/* Fundamental commands */
#define SSD1306_SET_CONTRAST_CMD (0x81U)
#define SSD1306_DISPLAY_ON_RAM_CMD (0xA4U)
#define SSD1306_DISPLAY_ON_FULL_CMD (0xA5U)
#define SSD1306_SET_NORMAL_DISPLAY_CMD (0xA6U)
#define SSD1306_SET_INVERSE_DISPLAY_CMD (0xA7U)
#define SSD1306_SET_DISPLAY_ON_CMD (0xAFU)
#define SSD1306_SET_DISPLAY_OFF_CMD (0xAEU)

#define SSD1306_ADDRSET_MEMORY_ADDRESSING_MODE_CMD (0x20U)
#define SSD1306_ADDRHORIZONTAL_MODE (0x0U)
#define SSD1306_ADDRVERTICAL_MODE (0x1U)
#define SSD1306_ADDRPAGE_MODE (0x2U)

#define SSD1306_SET_COLUMN_ADDRESS_CMD (0x21U)
#define SSD1306_SET_PAGE_ADDRESS_CMD (0x22U)
#define SSD1306_SET_DISPLAY_START_LINE_CMD (0x40U)
#define SSD1306_SET_SEGMENT_REMAP_NORMAL_CMD (0xA0U)
#define SSD1306_SET_SEGMENT_REMAP_REMAPPED_CMD (0xA1U)

#define SSD1306_SET_MULTIPLEX_RATIO_CMD (0xA8U)
#define SSD1306_MUX_RATIO_32 (0x1FU)

#define SSD1306_SET_COM_SCAN_DIR_NORMAL_CMD (0xC0U)
#define SSD1306_SET_COM_SCAN_DIR_REMAPPED_CMD (0xC8U)
#define SSD1306_SET_DISPLAY_OFFSET_CMD (0xD3U)
#define SSD1306_SET_COM_HW_CONFIG_CMD (0xDAU)

#define SSD1306_SET_DISPLAY_CLK_DIVIDE_FREQ_CMD (0xD5U)


#define SSD1306_CHARGE_PUMP_SETTING_CMD (0x8D)

#define SSD1306_CLK_FREQ_VAL ((8<<4)|0)
#define SSD1306_DISP_OFFSET_VAL (0x0U)
#define SSD1306_HW_CONFIG_VAL (0x02U)
#define SSD1306_CONTRAST_VAL (0x7FU)
#define SSD1306_CHARGE_PUMP_VAL (0x14U)


#define SSD1306_DATA_BUF_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

#define SSD1306_UNINIT_VAL (0xFFU)

typedef struct ssd1306_type {
    uint8_t buffer[SSD1306_DATA_BUF_SIZE];
} ssd1306_t;

typedef enum 
{
    ADDR_MODE_HORIZONTAL= 0b00,
    ADDR_MODE_VERTICAL = 0b01,
    ADDR_MODE_PAGE = 0b10,
    ADDR_MODE_ERR = 0b11,
}ssd1306_addr_mode_t;

typedef enum
{
    SSD1306_NO_ERR = 0,
    SSD1306_GENERIC_ERR,
    SSD1306_I2C_INIT_ERR,
    SSD1306_I2C_SEND_ERR

} ssd1306_err_t;

/* Private functions */
static uint ssd1306_set_mem_addr_mode(ssd1306_addr_mode_t mode, uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop);

/* Public functions */
uint ssd1306_init();
uint ssd1306_draw();
void ssd1306_set_pixel(uint8_t x, uint8_t y, bool pixel_on);
void ssd1306_clear_buff();
void ssd1306_fill_buff(uint8_t val);
bool ssd1306_get_pixel(uint8_t x, uint8_t y);
void ssd1306_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ssd1306_h_line(uint8_t x, uint8_t y, uint8_t length);
void ssd1306_v_line(uint8_t x, uint8_t y, uint8_t length);

#endif /* SSD1306_H */