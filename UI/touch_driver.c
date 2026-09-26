#include "touch_driver.h"
#include "st7789.h"
#include "stm32wbxx_hal.h"

/* I2C句柄 - 如果未定义则使用模拟模式 */
#ifdef USE_HARDWARE_I2C
extern I2C_HandleTypeDef hi2c1;
#endif

/* 触摸芯片I2C地址 */
#define CST816S_I2C_ADDR 0x15

/* 触摸复位引脚 */
#define TOUCH_RST_PIN   TP_RST_PIN
#define TOUCH_RST_PORT  TP_RST_PORT

/* 芯片ID信息 */
static uint8_t chip_id = 0;
static uint8_t vendor_id = 0;
static uint8_t fw_version = 0;
static bool touch_initialized = false;

#ifdef USE_HARDWARE_I2C
/* I2C读取函数 */
static bool i2c_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, CST816S_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    return (status == HAL_OK);
}

/* I2C写入函数 */
static bool i2c_write(uint8_t reg, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, CST816S_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    return (status == HAL_OK);
}
#else
/* 模拟I2C读取函数 - 用于测试 */
static bool i2c_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    /* TODO: 实现软件I2C或等待硬件I2C配置完成 */
    (void)reg;
    (void)data;
    (void)len;
    return false;
}

/* 模拟I2C写入函数 */
static bool i2c_write(uint8_t reg, uint8_t *data, uint16_t len)
{
    /* TODO: 实现软件I2C或等待硬件I2C配置完成 */
    (void)reg;
    (void)data;
    (void)len;
    return false;
}
#endif

/* 初始化触摸驱动 */
bool touch_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIO时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置触摸复位引脚 */
    GPIO_InitStruct.Pin = TOUCH_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStruct);
    
    /* 硬件复位 */
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
    
    /* 读取芯片ID */
    if (!i2c_read(CST816S_REG_CHIP_ID, &chip_id, 1)) {
        chip_id = 0xFF;
        return false;
    }
    HAL_Delay(5);
    
    if (!i2c_read(CST816S_REG_VENDOR_ID, &vendor_id, 1)) {
        vendor_id = 0xFF;
        return false;
    }
    HAL_Delay(5);
    
    if (!i2c_read(CST816S_REG_FW_VERSION, &fw_version, 1)) {
        fw_version = 0xFF;
        return false;
    }
    
    /* 配置中断控制 */
    uint8_t motion_mask = 0x05;
    i2c_write(0xEC, &motion_mask, 1);
    
    uint8_t irq_ctl = 0x70;
    i2c_write(0xFA, &irq_ctl, 1);
    i2c_write(0xFB, (uint8_t[]){0x00}, 1);
    
    return true;
}

/* 获取触摸信息 */
touch_info_t touch_get_info(void)
{
    touch_info_t info = {0};
    uint8_t touch_data[6] = {0};
    
    /* 读取触摸数据 */
    if (!i2c_read(0x01, touch_data, sizeof(touch_data))) {
        info.is_valid = false;
        return info;
    }
    
    uint8_t touch_num = touch_data[1] & 0x0F;
    uint8_t x_high = touch_data[2] & 0x0F;
    uint8_t x_low = touch_data[3];
    uint16_t x = (x_high << 8) | x_low;
    
    uint8_t y_high = touch_data[4] & 0x0F;
    uint8_t y_low = touch_data[5];
    uint16_t y = (y_high << 8) | y_low;
    
    touch_gesture_t gesture = (touch_gesture_t)touch_data[0];
    
    /* 验证数据有效性 */
    if (x >= 240 || y >= 280 ||
        (gesture != TOUCH_GESTURE_NONE && 
         gesture != TOUCH_GESTURE_SLIDE_DOWN && 
         gesture != TOUCH_GESTURE_SLIDE_UP && 
         gesture != TOUCH_GESTURE_SLIDE_LEFT && 
         gesture != TOUCH_GESTURE_SLIDE_RIGHT && 
         gesture != TOUCH_GESTURE_SINGLE_TAP && 
         gesture != TOUCH_GESTURE_DOUBLE_TAP && 
         gesture != TOUCH_GESTURE_LONG_PRESS)) {
        info.is_valid = false;
        return info;
    }
    
    info.x = x;
    info.y = y;
    info.is_touching = (touch_num > 0);
    info.gesture = gesture;
    info.is_valid = true;
    
    return info;
}

/* 触摸休眠 */
void touch_sleep(void)
{
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
    
    uint8_t sleep_value = 0x03;
    i2c_write(0xA5, &sleep_value, 1);
}

/* 触摸唤醒 */
void touch_wakeup(void)
{
    touch_init();
}