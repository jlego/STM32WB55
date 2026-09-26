#include "main.h"
#include "st7789.h"
#include "lcd.h"  // 包含LCD相关的函数声明和常量定义
#include <stdbool.h>

// SPI句柄（从main.c引入）
extern SPI_HandleTypeDef hspi1;

// TIM2 PWM句柄
static TIM_HandleTypeDef htim2;
static uint8_t bl_brightness = 100;

/* 初始化TIM2 PWM输出（PA0 = TIM2_CH1） */
static void ST7789_PWM_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 64 - 1;       /* 64MHz / 64 = 1MHz */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 100 - 1;          /* 1MHz / 100 = 10kHz PWM频率 */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim2);
    
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 10;                 /* 10%占空比 */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    
    /* 配置PA0为TIM2_CH1复用功能 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

/* 设置PWM占空比 */
static void ST7789_PWM_SetDuty(uint8_t duty)
{
    if (duty > 100) duty = 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
}

// 延时函数
void ST7789_Delay(uint32_t ms) {
    HAL_Delay(ms);
}

// 使用硬件SPI传输数据
static void ST7789_WriteSPI(uint8_t data) {
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

void ST7789_WriteCommand(uint8_t cmd) {
    // 设置DC为低电平，表示命令
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
    
    // 拉低CS使能
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    
    ST7789_WriteSPI(cmd);
    
    // 拉高CS禁用
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void ST7789_WriteData(uint8_t data) {
    // 设置DC为高电平，表示数据
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    
    // 拉低CS使能
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    
    ST7789_WriteSPI(data);
    
    // 拉高CS禁用
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void ST7789_WriteData16(uint16_t data) {
    ST7789_WriteData((data >> 8) & 0xFF);  // 高字节
    ST7789_WriteData(data & 0xFF);         // 低字节
}

void ST7789_SetAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    // 计算实际坐标（加上偏移量）
    uint16_t x_start = x + DISPLAY_OFFSET_X;
    uint16_t y_start = y + DISPLAY_OFFSET_Y;
    uint16_t x_end = x_start + w - 1;
    uint16_t y_end = y_start + h - 1;
    
    // 设置列地址
    ST7789_WriteCommand(ST7789_CASET);
    ST7789_WriteData16(x_start);
    ST7789_WriteData16(x_end);
    
    // 设置行地址
    ST7789_WriteCommand(ST7789_RASET);
    ST7789_WriteData16(y_start);
    ST7789_WriteData16(y_end);
    
    // 写入内存
    ST7789_WriteCommand(ST7789_RAMWR);
}

void ST7789_FillScreen(uint16_t color) {
    // 使用完整的显示区域（包括偏移）进行填充
    ST7789_SetAddressWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    // 拉低CS使能
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    
    uint8_t colorBytes[2] = {(color >> 8) & 0xFF, color & 0xFF};
    for(uint32_t i = 0; i < (uint32_t)DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        HAL_SPI_Transmit(&hspi1, colorBytes, 2, HAL_MAX_DELAY);
    }
    
    // 拉高CS禁用
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if(x >= ST7789_WIDTH || y >= ST7789_HEIGHT) return;
    
    ST7789_SetAddressWindow(x, y, 1, 1);
    ST7789_WriteData16(color);
}

void ST7789_Init(void) {
    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 配置LCD控制引脚
    GPIO_InitStruct.Pin = LCD_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_RST_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LCD_DC_PIN;
    HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LCD_CS_PIN;
    HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);
    
    // 配置背光引脚
    GPIO_InitStruct.Pin = LCD_BL_PIN;
    HAL_GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);
    
    // 配置触摸屏引脚
    GPIO_InitStruct.Pin = TP_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TP_INT_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = TP_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_RST_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = TP_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_SDA_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = TP_SCL_PIN;
    HAL_GPIO_Init(TP_SCL_PORT, &GPIO_InitStruct);
    
    // 硬件复位
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    ST7789_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    ST7789_Delay(100);

    //************* ST7789初始化序列 **********//	
    ST7789_WriteCommand(0x36); 
    ST7789_WriteData(0x00);

    ST7789_WriteCommand(0x3A); 
    ST7789_WriteData(0x05);

    ST7789_WriteCommand(0xB2);
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x00);
    ST7789_WriteData(0x33);
    ST7789_WriteData(0x33);

    ST7789_WriteCommand(0xB7); 
    ST7789_WriteData(0x35);  

    ST7789_WriteCommand(0xBB);
    ST7789_WriteData(0x19);

    ST7789_WriteCommand(0xC0);
    ST7789_WriteData(0x2C);

    ST7789_WriteCommand(0xC2);
    ST7789_WriteData(0x01);

    ST7789_WriteCommand(0xC3);
    ST7789_WriteData(0x12);   

    ST7789_WriteCommand(0xC4);
    ST7789_WriteData(0x20);  

    ST7789_WriteCommand(0xC6); 
    ST7789_WriteData(0x0F);    

    ST7789_WriteCommand(0xD0); 
    ST7789_WriteData(0xA4);
    ST7789_WriteData(0xA1);

    ST7789_WriteCommand(0xE0);
    ST7789_WriteData(0xD0);
    ST7789_WriteData(0x04);
    ST7789_WriteData(0x0D);
    ST7789_WriteData(0x11);
    ST7789_WriteData(0x13);
    ST7789_WriteData(0x2B);
    ST7789_WriteData(0x3F);
    ST7789_WriteData(0x54);
    ST7789_WriteData(0x4C);
    ST7789_WriteData(0x18);
    ST7789_WriteData(0x0D);
    ST7789_WriteData(0x0B);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x23);

    ST7789_WriteCommand(0xE1);
    ST7789_WriteData(0xD0);
    ST7789_WriteData(0x04);
    ST7789_WriteData(0x0C);
    ST7789_WriteData(0x11);
    ST7789_WriteData(0x13);
    ST7789_WriteData(0x2C);
    ST7789_WriteData(0x3F);
    ST7789_WriteData(0x44);
    ST7789_WriteData(0x51);
    ST7789_WriteData(0x2F);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x1F);
    ST7789_WriteData(0x20);
    ST7789_WriteData(0x23);

    ST7789_WriteCommand(0x21); 

    ST7789_WriteCommand(0x11); 
    ST7789_Delay(120); 

    ST7789_WriteCommand(0x29); 

    // 设置列地址 (0 to 239)
    ST7789_WriteCommand(0x2A); // 列地址设置
    ST7789_WriteData16(DISPLAY_OFFSET_X);                    // 起始列地址 (0)
    ST7789_WriteData16(DISPLAY_OFFSET_X + DISPLAY_WIDTH - 1); // 结束列地址 (239)

    // 设置行地址 (20 to 299, for 240x280 resolution with 20 pixel offset)
    ST7789_WriteCommand(0x2B); // 行地址设置
    ST7789_WriteData16(DISPLAY_OFFSET_Y);                     // 起始行地址 (20)
    ST7789_WriteData16(DISPLAY_OFFSET_Y + DISPLAY_HEIGHT - 1); // 结束行地址 (299)

    ST7789_WriteCommand(0x2C); // RAMWR - 内存写入

    // 设置LCD显示方向
    LCD_direction(USE_HORIZONTAL);
    
    // 初始化TIM2 PWM背光控制（10%亮度）
    ST7789_PWM_Init();
    ST7789_PWM_SetDuty(10);
    
    // 全黑清屏
    ST7789_FillScreen(COLOR_BLACK);
}

/* 设置背光亮度
 * brightness: 0-100%
 * 使用TIM2硬件PWM实现精确亮度控制
 */
void ST7789_SetBacklight(uint8_t brightness)
{
    if (brightness > 100) brightness = 100;
    bl_brightness = brightness;
    ST7789_PWM_SetDuty(brightness);
}

/* 在主循环中调用此函数以更新背光 */
void ST7789_BacklightTick(void)
{
    /* 硬件PWM自动运行，无需软件干预 */
}