#include "i2c_hal.h"
#include <stdint.h>

uint i2c_hal_init(void)
{
    uint ret_val;
    ret_val = i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    return ret_val;
}

uint i2c_hal_write(i2c_hal_cmd_t cmd_type, uint8_t address, uint8_t *buffer, uint32_t buff_len)
{
    uint ret_val = I2C_UNINIT_VAL;
    const uint8_t control_byte[] = {(uint8_t)cmd_type};
    ret_val = i2c_write_burst_blocking(I2C_PORT, address ,control_byte, 1);
    ret_val = i2c_write_blocking(I2C_PORT, address, buffer, buff_len, false);
    return ret_val;
}