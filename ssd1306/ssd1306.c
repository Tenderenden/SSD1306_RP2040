#include "ssd1306.h"

static uint8_t ssd1306_cfg_cmds[] = {
    SSD1306_SET_DISPLAY_OFF_CMD,
    SSD1306_SET_DISPLAY_CLK_DIVIDE_FREQ_CMD, SSD1306_CLK_FREQ_VAL,
    SSD1306_SET_MULTIPLEX_RATIO_CMD, SSD1306_MUX_RATIO_32,
    SSD1306_SET_DISPLAY_OFFSET_CMD, SSD1306_DISP_OFFSET_VAL,
    SSD1306_SET_DISPLAY_START_LINE_CMD,
    SSD1306_SET_SEGMENT_REMAP_NORMAL_CMD,
    SSD1306_SET_COM_SCAN_DIR_NORMAL_CMD,
    SSD1306_SET_COM_HW_CONFIG_CMD, SSD1306_HW_CONFIG_VAL,
    SSD1306_SET_CONTRAST_CMD, SSD1306_CONTRAST_VAL,
    SSD1306_DISPLAY_ON_RAM_CMD,
    SSD1306_SET_NORMAL_DISPLAY_CMD,
    SSD1306_CHARGE_PUMP_SETTING_CMD, SSD1306_CHARGE_PUMP_VAL,
    SSD1306_SET_DISPLAY_ON_CMD,
    SSD1306_DISPLAY_ON_FULL_CMD,
    SSD1306_DISPLAY_ON_RAM_CMD
    };

static uint8_t screen_data_buffer[SSD1306_DATA_BUF_SIZE];

/* Private functions */
static ssd1306_status_t ssd1306_set_mem_addr_mode(ssd1306_addr_mode_t mode, uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop);

ssd1306_status_t ssd1306_init(void) {
    ssd1306_status_t ret_val = SSD1306_NOT_INITIALIZED_ERR;
    i2c_hal_init(); //TODO: Think how to verify if this init went well. Especially i2c_init from sdk
    ret_val = i2c_hal_write(I2C_CMD_STREAM_TYPE, SSD1306_ADDR, ssd1306_cfg_cmds, count_of(ssd1306_cfg_cmds));
    if(ret_val != count_of(ssd1306_cfg_cmds)) {
        return SSD1306_I2C_INIT_ERR;
    }

    //Set horizontal mode for full screen
    ret_val = ssd1306_set_mem_addr_mode(ADDR_MODE_HORIZONTAL, 0, SSD1306_WIDTH-1, 0, (SSD1306_HEIGHT/8)-1);
    if (ret_val != SSD1306_NO_ERR) {
        return ret_val;
    }
    ssd1306_clear_buff();
    ret_val = ssd1306_update();
    return ret_val;
}

ssd1306_status_t ssd1306_update(void)
{
    ssd1306_status_t ret_val = SSD1306_NOT_INITIALIZED_ERR;
    ret_val = i2c_hal_write(I2C_DATA_STREAM_TYPE, SSD1306_ADDR, screen_data_buffer, SSD1306_DATA_BUF_SIZE);
    return ret_val;
}

void ssd1306_fill_buff(uint8_t val)
{
    memset(screen_data_buffer, val, SSD1306_DATA_BUF_SIZE);
}

static ssd1306_status_t ssd1306_set_mem_addr_mode(ssd1306_addr_mode_t mode, uint8_t col_start, uint8_t col_stop, uint8_t page_start, uint8_t page_stop)
{
    ssd1306_status_t ret_val = SSD1306_NOT_INITIALIZED_ERR;
    uint8_t cmds[] = {
        SSD1306_ADDRSET_MEMORY_ADDRESSING_MODE_CMD, 
        mode, 
        SSD1306_SET_COLUMN_ADDRESS_CMD,
        col_start,
        col_stop,
        SSD1306_SET_PAGE_ADDRESS_CMD,
        page_start,
        page_stop
    };

    ret_val = i2c_hal_write(I2C_CMD_STREAM_TYPE,SSD1306_ADDR, cmds, 8);
    if (SSD1306_NO_ERR != ret_val){
        return ret_val;
    }
    return SSD1306_NO_ERR;
}

void ssd1306_set_pixel(uint8_t x, uint8_t y, bool pixel_on)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    uint16_t index =(page * SSD1306_WIDTH) + x;

    if (pixel_on) {
        screen_data_buffer[index] |= (1 << bit);
    } else {
        screen_data_buffer[index] &= ~(1 << bit);
    }
}

bool ssd1306_get_pixel(uint8_t x, uint8_t y)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return false;
    bool ret_val;
    uint8_t page = y / 8;
    uint16_t index = (page * SSD1306_WIDTH) + x;
    ret_val = (bool)((screen_data_buffer[index]) & (1 << (y % 8)));
    return ret_val;
}

void ssd1306_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    /* source: https://zingl.github.io/bresenham.html */
    int dx = abs((int)x1-(int)x0);
    int sx = (x0<x1) ? 1: -1;
    int dy = -abs((int)y1-(int)y0);
    int sy = (y0<y1) ? 1: -1;
    int err = dx + dy;
    int e2;

    while (true) {
        ssd1306_set_pixel(x0, y0, true);
        if(x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

ssd1306_status_t ssd1306_h_line(uint8_t x, uint8_t y, uint8_t length)
{
    if((SSD1306_WIDTH < (x+length)) || (SSD1306_HEIGHT < y)) return SSD1306_INV_PARAM_ERR;
    for(uint16_t i=x; i<(x+length); i++)
    {
        ssd1306_set_pixel(i, y, true);
    }
    return SSD1306_NO_ERR;
}

ssd1306_status_t ssd1306_v_line(uint8_t x, uint8_t y, uint8_t length)
{
    if((SSD1306_HEIGHT < (y+length)) || (SSD1306_WIDTH < x)) return SSD1306_INV_PARAM_ERR;
    for(uint16_t i=y; i<(y+length); i++)
    {
        ssd1306_set_pixel(x, i, true);
    }
    return SSD1306_NO_ERR;
}

void ssd1306_clear_buff(void)
{
    ssd1306_fill_buff(0x00);
}