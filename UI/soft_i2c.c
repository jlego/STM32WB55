#include "soft_i2c.h"
#include "stm32wbxx_hal.h"

/* SDA方向控制 */
static void soft_i2c_sda_set_output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}

static void soft_i2c_sda_set_input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}

/* 初始化软件I2C */
void soft_i2c_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIO时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* 配置SCL引脚 - 开漏输出 */
    GPIO_InitStruct.Pin = SOFT_I2C_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SCL_PORT, &GPIO_InitStruct);
    
    /* 配置SDA引脚 - 初始为输出 */
    soft_i2c_sda_set_output();
    
    /* 初始状态为高电平 */
    SOFT_I2C_SCL_HIGH();
    SOFT_I2C_SDA_HIGH();
    SOFT_I2C_DELAY();
}

/* I2C起始信号 */
void soft_i2c_start(void)
{
    soft_i2c_sda_set_output();
    SOFT_I2C_SDA_HIGH();
    SOFT_I2C_SCL_HIGH();
    SOFT_I2C_DELAY();
    SOFT_I2C_SDA_LOW();
    SOFT_I2C_DELAY();
    SOFT_I2C_SCL_LOW();
    SOFT_I2C_DELAY();
}

/* I2C停止信号 */
void soft_i2c_stop(void)
{
    soft_i2c_sda_set_output();
    SOFT_I2C_SDA_LOW();
    SOFT_I2C_DELAY();
    SOFT_I2C_SCL_HIGH();
    SOFT_I2C_DELAY();
    SOFT_I2C_SDA_HIGH();
    SOFT_I2C_DELAY();
}

/* 写入一个字节 */
bool soft_i2c_write_byte(uint8_t byte)
{
    bool ack;
    
    soft_i2c_sda_set_output();
    
    for (int i = 0; i < 8; i++) {
        if (byte & 0x80) {
            SOFT_I2C_SDA_HIGH();
        } else {
            SOFT_I2C_SDA_LOW();
        }
        SOFT_I2C_DELAY();
        SOFT_I2C_SCL_HIGH();
        SOFT_I2C_DELAY();
        SOFT_I2C_SCL_LOW();
        SOFT_I2C_DELAY();
        byte <<= 1;
    }
    
    /* 读取ACK - 切换SDA为输入 */
    soft_i2c_sda_set_input();
    SOFT_I2C_DELAY();
    SOFT_I2C_SCL_HIGH();
    SOFT_I2C_DELAY();
    ack = !SOFT_I2C_SDA_READ();
    SOFT_I2C_SCL_LOW();
    SOFT_I2C_DELAY();
    
    return ack;
}

/* 读取一个字节 */
uint8_t soft_i2c_read_byte(bool ack)
{
    uint8_t byte = 0;
    
    /* 设置SDA为输入模式 */
    soft_i2c_sda_set_input();
    
    for (int i = 0; i < 8; i++) {
        SOFT_I2C_SCL_HIGH();
        SOFT_I2C_DELAY();
        byte <<= 1;
        if (SOFT_I2C_SDA_READ()) {
            byte |= 0x01;
        }
        SOFT_I2C_SCL_LOW();
        SOFT_I2C_DELAY();
    }
    
    /* 发送ACK/NACK */
    soft_i2c_sda_set_output();
    if (ack) {
        SOFT_I2C_SDA_LOW();
    } else {
        SOFT_I2C_SDA_HIGH();
    }
    SOFT_I2C_DELAY();
    SOFT_I2C_SCL_HIGH();
    SOFT_I2C_DELAY();
    SOFT_I2C_SCL_LOW();
    SOFT_I2C_DELAY();
    
    return byte;
}

/* 写入数据到寄存器 */
bool soft_i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    soft_i2c_start();
    
    /* 发送设备地址+写 */
    if (!soft_i2c_write_byte(dev_addr << 1)) {
        soft_i2c_stop();
        return false;
    }
    
    /* 发送寄存器地址 */
    if (!soft_i2c_write_byte(reg)) {
        soft_i2c_stop();
        return false;
    }
    
    /* 发送数据 */
    for (uint16_t i = 0; i < len; i++) {
        if (!soft_i2c_write_byte(data[i])) {
            soft_i2c_stop();
            return false;
        }
    }
    
    soft_i2c_stop();
    return true;
}

/* 从寄存器读取数据 */
bool soft_i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    soft_i2c_start();
    
    /* 发送设备地址+写 */
    if (!soft_i2c_write_byte(dev_addr << 1)) {
        soft_i2c_stop();
        return false;
    }
    
    /* 发送寄存器地址 */
    if (!soft_i2c_write_byte(reg)) {
        soft_i2c_stop();
        return false;
    }
    
    /* 重复起始信号 */
    soft_i2c_start();
    
    /* 发送设备地址+读 */
    if (!soft_i2c_write_byte((dev_addr << 1) | 0x01)) {
        soft_i2c_stop();
        return false;
    }
    
    /* 读取数据 */
    for (uint16_t i = 0; i < len; i++) {
        data[i] = soft_i2c_read_byte(i < (len - 1));
    }
    
    soft_i2c_stop();
    return true;
}