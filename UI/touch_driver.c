#include "touch_driver.h"
#include "st7789.h"
#include "stm32wbxx_hal.h"

/* FT6X36触摸芯片I2C地址 */
#define FT6X36_I2C_ADDR 0x38

/* FT6X36寄存器地址 */
#define FT6X36_REG_DEVICE_MODE  0x00
#define FT6X36_REG_GESTURE_ID   0x01
#define FT6X36_REG_TD_STATUS    0x02
#define FT6X36_REG_TOUCH1_XH    0x03
#define FT6X36_REG_TOUCH1_XL    0x04
#define FT6X36_REG_TOUCH1_YH    0x05
#define FT6X36_REG_TOUCH1_YL    0x06
#define FT6X36_REG_TOUCH2_XH    0x09
#define FT6X36_REG_TOUCH2_XL    0x0A
#define FT6X36_REG_TOUCH2_YH    0x0B
#define FT6X36_REG_TOUCH2_YL    0x0C
#define FT6X36_REG_CHIP_ID      0xA3
#define FT6X36_REG_FIRMWARE_ID  0xA6
#define FT6X36_REG_RELEASE_TH   0x80

/* FT6X36芯片ID */
#define FT6X36_CHIP_ID          0x11

/* 触摸复位引脚 */
#define TOUCH_RST_PIN   TP_RST_PIN
#define TOUCH_RST_PORT  TP_RST_PORT

/* I2C句柄 */
extern I2C_HandleTypeDef hi2c1;

/* 芯片信息 */
static uint8_t chip_id = 0;
static uint8_t fw_version = 0;
static bool touch_initialized = false;

/* I2C读取函数 - 使用软件超时，不依赖SysTick */
static bool ft6x36_i2c_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    /* 使用10ms超时，避免SysTick停止时永久阻塞 */
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, FT6X36_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, 10);
    return (status == HAL_OK);
}

/* I2C写入函数 - 使用软件超时，不依赖SysTick */
static bool ft6x36_i2c_write(uint8_t reg, uint8_t *data, uint16_t len)
{
    /* 使用10ms超时，避免SysTick停止时永久阻塞 */
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, FT6X36_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, 10);
    return (status == HAL_OK);
}

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
    
    /* 硬件复位 - 使用软件延迟 */
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_RESET);
    ST7789_Delay(10);
    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET);
    ST7789_Delay(50);
    
    /* 读取芯片ID */
    if (!ft6x36_i2c_read(FT6X36_REG_CHIP_ID, &chip_id, 1)) {
        chip_id = 0xFF;
        return false;
    }
    
    /* 验证芯片ID */
    if (chip_id != FT6X36_CHIP_ID) {
        return false;
    }
    
    /* 读取固件版本 */
    ft6x36_i2c_read(FT6X36_REG_FIRMWARE_ID, &fw_version, 1);
    
    /* 设置工作模式为正常模式 */
    uint8_t mode = 0x00;
    ft6x36_i2c_write(FT6X36_REG_DEVICE_MODE, &mode, 1);
    
    /* 设置触摸阈值 */
    uint8_t threshold = 0x16;
    ft6x36_i2c_write(FT6X36_REG_RELEASE_TH, &threshold, 1);
    
    touch_initialized = true;
    return true;
}

/* 获取触摸信息 */
touch_info_t touch_get_info(void)
{
    touch_info_t info = {0};
    uint8_t touch_data[13] = {0};
    
    if (!touch_initialized) {
        return info;
    }
    
    /* 读取触摸数据 */
    if (!ft6x36_i2c_read(FT6X36_REG_GESTURE_ID, touch_data, 13)) {
        info.is_valid = false;
        return info;
    }
    
    uint8_t td_status = touch_data[1];
    uint8_t touch_points = td_status & 0x0F;
    
    /* 获取手势 */
    info.gesture = (touch_gesture_t)touch_data[0];
    
    /* 获取第一个触摸点 */
    if (touch_points >= 1) {
        uint8_t event1 = (touch_data[2] >> 6) & 0x03;
        uint8_t x1 = ((touch_data[2] & 0x0F) << 8) | touch_data[3];
        uint8_t y1 = ((touch_data[4] & 0x0F) << 8) | touch_data[5];
        
        info.x = x1;
        info.y = y1;
        info.is_touching = (event1 == 0x00 || event1 == 0x02);
        info.is_valid = (x1 < 240 && y1 < 280);
    } else {
        info.is_touching = false;
        info.is_valid = true;
    }
    
    return info;
}

/* 触摸休眠 */
void touch_sleep(void)
{
    uint8_t mode = 0x03;
    ft6x36_i2c_write(FT6X36_REG_DEVICE_MODE, &mode, 1);
}

/* 触摸唤醒 */
void touch_wakeup(void)
{
    uint8_t mode = 0x00;
    ft6x36_i2c_write(FT6X36_REG_DEVICE_MODE, &mode, 1);
}