#ifndef ST7789_H
#define ST7789_H

#include "main.h"
#include "fonts.h"
#include <stdint.h>
#include <stdbool.h>

// ST7789命令定义
#define ST7789_NOP          0x00
#define ST7789_SWRESET      0x01
#define ST7789_RDDID        0x04
#define ST7789_RDDST        0x09
#define ST7789_SLPIN        0x10
#define ST7789_SLPOUT       0x11
#define ST7789_PTLON        0x12
#define ST7789_NORON        0x13
#define ST7789_INVOFF       0x20
#define ST7789_INVON        0x21
#define ST7789_DISPOFF      0x28
#define ST7789_DISPON       0x29
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_RAMRD        0x2E
#define ST7789_PTLAR        0x30
#define ST7789_COLMOD       0x3A
#define ST7789_MADCTL       0x36
#define ST7789_MADCTL_MY    0x80
#define ST7789_MADCTL_MX    0x40
#define ST7789_MADCTL_MV    0x20
#define ST7789_MADCTL_ML    0x10
#define ST7789_MADCTL_RGB   0x00
#define ST7789_MADCTL_BGR   0x08

// 配置选项
#define USE_DMA  // 启用DMA支持
#define HOR_LEN 5  // DMA传输时的行缓冲区大小

// 显示配置参数
#define DISPLAY_WIDTH         240
#define DISPLAY_HEIGHT        280
#define DISPLAY_SWAP_XY       false
#define DISPLAY_MIRROR_X      false
#define DISPLAY_MIRROR_Y      false
#define DISPLAY_INVERT_COLOR  false
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false
#define DISPLAY_OFFSET_X      0
#define DISPLAY_OFFSET_Y      20

// 显示屏参数 (基于显示配置计算)
#define ST7789_WIDTH          DISPLAY_WIDTH
#define ST7789_HEIGHT         DISPLAY_HEIGHT

// 颜色定义
#define COLOR_BLACK         0x0000
#define COLOR_BLUE          0x001F
#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_CYAN          0x07FF
#define COLOR_MAGENTA       0xF81F
#define COLOR_YELLOW        0xFFE0
#define COLOR_WHITE         0xFFFF
#define COLOR_ORANGE        0xFC00
#define COLOR_GRAY          0x8430
#define COLOR_BRED          0xF81F
#define COLOR_GRED          0xFFE0
#define COLOR_GBLUE         0x07FF
#define COLOR_BROWN         0xBC40
#define COLOR_BRRED         0xFC07
#define COLOR_DARKBLUE      0x01CF
#define COLOR_LIGHTBLUE     0x7D7C
#define COLOR_GRAYBLUE      0x5458
#define COLOR_LIGHTGREEN    0x841F
#define COLOR_LGRAY         0xC618
#define COLOR_LGRAYBLUE     0xA651
#define COLOR_LBBLUE        0x2B12

// 引脚定义 - 根据实际接线修改
#define LCD_RST_PIN         GPIO_PIN_1      // B1 -> RST
#define LCD_RST_PORT        GPIOB

#define LCD_DC_PIN          GPIO_PIN_0      // B0 -> DC
#define LCD_DC_PORT         GPIOB

#define LCD_CS_PIN          GPIO_PIN_15     // A15 -> CS
#define LCD_CS_PORT         GPIOA

#define LCD_SCK_PIN         GPIO_PIN_5      // A5 -> SCK
#define LCD_SCK_PORT        GPIOA

#define LCD_MOSI_PIN        GPIO_PIN_5      // B5 -> MOSI
#define LCD_MOSI_PORT       GPIOB

#define LCD_BL_PIN          GPIO_PIN_0      // 背光引脚，根据实际情况调整
#define LCD_BL_PORT         GPIOA

// 触摸屏引脚定义 - FT3168
#define TP_INT_PIN          GPIO_PIN_3
#define TP_INT_PORT         GPIOA

#define TP_RST_PIN          GPIO_PIN_2
#define TP_RST_PORT         GPIOA

#define TP_SDA_PIN          GPIO_PIN_7
#define TP_SDA_PORT         GPIOB

#define TP_SCL_PIN          GPIO_PIN_6
#define TP_SCL_PORT         GPIOB

// 函数声明
void ST7789_Init(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData(uint8_t *buff, size_t buff_size);
void ST7789_WriteData16(uint16_t data);
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_FillScreen(uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_Delay(uint32_t ms);
void ST7789_SetBacklight(uint8_t brightness);  // 亮度控制 0-100%
void ST7789_BacklightTick(void);               // 在主循环中调用以更新PWM

// 图形函数
void ST7789_Fill_Color(uint16_t color);
void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color);
void ST7789_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
void ST7789_InvertColors(uint8_t invert);
void ST7789_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

// 文本函数
void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);

// 测试函数
void ST7789_Test(void);

// GPIO控制函数（供LVGL使用）
static inline void lcd_cs_set(void) { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); }
static inline void lcd_cs_clr(void) { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET); }
static inline void lcd_dc_set(void) { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET); }
static inline void lcd_dc_clr(void) { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET); }

#endif