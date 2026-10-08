#ifndef I2C_HAL_H
#define I2C_HAL_H

#include <stdint.h>
#include <pico/types.h>
#include "hardware/i2c.h"
#include <hardware/gpio.h>

#define I2C_SDA_PIN (4U)
#define I2C_SCL_PIN (5U)
#define I2C_BAUDRATE (400000U)
#define I2C_PORT (i2c0)

#define I2C_UNINIT_VAL (0xFFU)

typedef enum
{
    I2C_CMD_STREAM_TYPE = (0x00U),
    I2C_DATA_STREAM_TYPE = (0x40U),
    I2C_CMD_TYPE = (0x80U),

} i2c_hal_cmd_t;

uint i2c_hal_init(void);
uint i2c_hal_write(i2c_hal_cmd_t cmd_type, uint8_t address, uint8_t *buffer, uint32_t buff_len);

#endif /* I2C_HAL_H */