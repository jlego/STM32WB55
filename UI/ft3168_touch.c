#include "ft3168_touch.h"
#include "soft_i2c.h"
#include "stm32wbxx_hal.h"
#include "st7789.h"

/* FT3168 I2C地址 */
#define FT3168_I2C_ADDR         0x38

/* GPIO引脚定义 */
#define FT3168_INT_PIN      GPIO_PIN_3
#define FT3168_INT_PORT     GPIOA

#define FT3168_RST_PIN      GPIO_PIN_2
#define FT3168_RST_PORT     GPIOA

/* 芯片ID */
static uint8_t chip_id = 0;
static uint8_t firmware_id = 0;
static bool touch_initialized = false;

/* I2C读取函数 */
static bool ft3168_i2c_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    return soft_i2c_read_reg(FT3168_I2C_ADDR, reg, data, len);
}

/* I2C写入函数 */
static bool ft3168_i2c_write(uint8_t reg, uint8_t *data, uint16_t len)
{
    return soft_i2c_write_reg(FT3168_I2C_ADDR, reg, data, len);
}

/* 硬件复位 */
static void ft3168_hard_reset(void)
{
    HAL_GPIO_WritePin(FT3168_RST_PORT, FT3168_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(FT3168_RST_PORT, FT3168_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(200);
}

/* 软件复位 */
static bool ft3168_soft_reset(void)
{
    uint8_t reset_cmd = 0x01;
    return ft3168_i2c_write(0xFC, &reset_cmd, 1);
}

/* 初始化FT3168触摸驱动 */
bool ft3168_touch_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 初始化软件I2C */
    soft_i2c_init();
    
    /* 使能GPIO时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置复位引脚 */
    GPIO_InitStruct.Pin = FT3168_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT3168_RST_PORT, &GPIO_InitStruct);
    
    /* 配置中断引脚 */
    GPIO_InitStruct.Pin = FT3168_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(FT3168_INT_PORT, &GPIO_InitStruct);
    
    /* 硬件复位 */
    ft3168_hard_reset();
    
    /* 读取芯片ID */
    if (!ft3168_i2c_read(FT3168_REG_CHIP_ID, &chip_id, 1)) {
        chip_id = 0xFF;
        return false;
    }
    HAL_Delay(5);
    
    /* 验证芯片ID - FT3168的芯片ID通常是0x64或0x00 */
    if (chip_id != 0x00 && chip_id != 0x06 && chip_id != 0x64) {
        return false;
    }
    
    /* 读取固件版本 */
    ft3168_i2c_read(FT3168_REG_FIRMWARE_ID, &firmware_id, 1);
    HAL_Delay(5);
    
    /* 配置中断模式 - 轮询模式 */
    uint8_t int_mode = 0x00;
    ft3168_i2c_write(0xA4, &int_mode, 1);
    
    /* 设置报告率 */
    uint8_t report_rate = 0x0A; /* 100Hz */
    ft3168_i2c_write(0x88, &report_rate, 1);
    
    touch_initialized = true;
    return true;
}

/* 获取触摸信息 */
ft3168_touch_info_t ft3168_touch_get_info(void)
{
    ft3168_touch_info_t info = {0};
    uint8_t touch_data[13] = {0};
    
    if (!touch_initialized) {
        return info;
    }
    
    /* 读取触摸状态 */
    if (!ft3168_i2c_read(FT3168_REG_TD_STATUS, touch_data, 13)) {
        return info;
    }
    
    /* 获取触摸点数 */
    info.touch_points = touch_data[0] & 0x0F;
    if (info.touch_points > 2) {
        info.touch_points = 2;
    }
    
    /* 获取手势 */
    info.gesture = (ft3168_gesture_t)touch_data[1];
    
    /* 解析第一个触摸点 */
    if (info.touch_points >= 1) {
        info.points[0].event = (touch_data[2] >> 6) & 0x03;
        info.points[0].id = (touch_data[2] >> 4) & 0x0F;
        info.points[0].x = ((touch_data[2] & 0x0F) << 8) | touch_data[3];
        info.points[0].y = ((touch_data[4] & 0x0F) << 8) | touch_data[5];
        info.points[0].is_valid = (info.points[0].x < 240 && info.points[0].y < 280);
    }
    
    /* 解析第二个触摸点 */
    if (info.touch_points >= 2) {
        info.points[1].event = (touch_data[8] >> 6) & 0x03;
        info.points[1].id = (touch_data[8] >> 4) & 0x0F;
        info.points[1].x = ((touch_data[8] & 0x0F) << 8) | touch_data[9];
        info.points[1].y = ((touch_data[10] & 0x0F) << 8) | touch_data[11];
        info.points[1].is_valid = (info.points[1].x < 240 && info.points[1].y < 280);
    }
    
    /* 判断是否有触摸 */
    info.is_touching = (info.touch_points > 0);
    
    return info;
}

/* 读取芯片ID */
uint8_t ft3168_touch_get_chip_id(void)
{
    return chip_id;
}

/* 读取固件版本 */
uint8_t ft3168_touch_get_firmware_id(void)
{
    return firmware_id;
}

/* 进入工厂模式 */
bool ft3168_touch_enter_factory_mode(void)
{
    uint8_t mode = 0x40;
    return ft3168_i2c_write(FT3168_REG_DEVICE_MODE, &mode, 1);
}

/* 退出工厂模式 */
bool ft3168_touch_exit_factory_mode(void)
{
    uint8_t mode = 0x00;
    return ft3168_i2c_write(FT3168_REG_DEVICE_MODE, &mode, 1);
}

/* 设置中断模式 */
void ft3168_touch_set_interrupt_mode(uint8_t mode)
{
    ft3168_i2c_write(0xA4, &mode, 1);
}

/* 中断处理函数 */
void FT3168_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(FT3168_INT_PIN);
}

/* 中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == FT3168_INT_PIN) {
        /* 触摸中断触发，可以在这里处理触摸事件 */
        /* 实际处理在lvgl_stm32.c的touchpad_read中完成 */
    }
}