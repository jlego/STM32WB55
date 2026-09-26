#ifndef ST7789_H
#define ST7789_H

#include "main.h"
#include <stdint.h>

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
#define ST7789_FRMCTR1      0xB1
#define ST7789_FRMCTR2      0xB2
#define ST7789_FRMCTR3      0xB3
#define ST7789_INVCTR       0xB4
#define ST7789_DISSET5      0xB6
#define ST7789_PWCTR1       0xC0
#define ST7789_PWCTR2       0xC1
#define ST7789_PWCTR3       0xC2
#define ST7789_PWCTR4       0xC3
#define ST7789_PWCTR5       0xC4
#define ST7789_VMCTR1       0xC5
#define ST7789_VMOFCTR      0xC7
#define ST7789_WRID2        0xD1
#define ST7789_WRID3        0xD2
#define ST7789_PWCTR6       0xFC
#define ST7789_GMCTRP1      0xE0
#define ST7789_GMCTRN1      0xE1

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

// 触摸屏引脚定义 - 根据需要调整
#define TP_INT_PIN          GPIO_PIN_2
#define TP_INT_PORT         GPIOB

#define TP_RST_PIN          GPIO_PIN_14
#define TP_RST_PORT         GPIOA

#define TP_SDA_PIN          GPIO_PIN_7
#define TP_SDA_PORT         GPIOB

#define TP_SCL_PIN          GPIO_PIN_6
#define TP_SCL_PORT         GPIOB

// 函数声明
void ST7789_Init(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData(uint8_t data);
void ST7789_WriteData16(uint16_t data);
void ST7789_SetAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void ST7789_FillScreen(uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_Delay(uint32_t ms);

// GPIO控制函数（供LVGL使用）
static inline void lcd_cs_set(void) { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); }
static inline void lcd_cs_clr(void) { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET); }
static inline void lcd_dc_set(void) { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET); }
static inline void lcd_dc_clr(void) { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET); }

#endif