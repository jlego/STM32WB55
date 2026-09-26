#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t id;        // LCD ID
    uint8_t dir;
    uint16_t wramcmd;
    uint16_t setxcmd;
    uint16_t setycmd;
    uint8_t xoffset;
    uint8_t yoffset;
} _lcd_dev;

extern _lcd_dev lcddev;
#define USE_HORIZONTAL 0

#define LCD_W 240
#define LCD_H 284  // 根据实际屏幕高度

extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

// 引脚定义
#define LCD_RST_PIN   GPIO_PIN_1
#define LCD_RST_PORT  GPIOB

#define LCD_DC_PIN    GPIO_PIN_0
#define LCD_DC_PORT   GPIOB

#define LCD_CS_PIN    GPIO_PIN_15
#define LCD_CS_PORT   GPIOA

#define LCD_BL_PIN    GPIO_PIN_0
#define LCD_BL_PORT   GPIOA

// ================= 颜色定义 =================
#define WHITE   0xFFFF
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define MAGENTA 0xF81F
#define BROWN   0xBC40
#define GRAY    0x8430

// ================= LCD 函数声明 =================
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPoint(uint16_t x,uint16_t y);
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WR_DATA(uint8_t data);
void LCD_WR_REG(uint8_t data);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);
uint16_t LCD_BGR2RGB(uint16_t c);
void LCD_direction(uint8_t direction);
void Lcd_WriteData_16Bit(uint16_t Data);

#endif
