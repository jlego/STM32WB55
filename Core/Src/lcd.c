#include "lcd.h"
#include "stm32wbxx_hal.h"
#include "main.h"

// SPI 句柄
extern SPI_HandleTypeDef hspi1;

_lcd_dev lcddev;
uint16_t POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;

// ================= GPIO 控制函数 =================
static inline void lcd_cs_set(void)   { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); }
static inline void lcd_cs_clr(void)   { HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET); }
static inline void lcd_dc_set(void)   { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET); }
static inline void lcd_dc_clr(void)   { HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET); }
static inline void lcd_rst_set(void)  { HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET); }
static inline void lcd_rst_clr(void)  { HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET); }
static inline void lcd_bl_on(void)    { HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_SET); }
static inline void lcd_bl_off(void)   { HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_RESET); }

// ================= SPI 写接口 =================
// static inline void LCD_SPI_Write(uint8_t data, uint8_t isData)
// {
//     uint16_t tx = ((uint16_t)isData << 15) | ((uint16_t)data << 7);
//     HAL_SPI_Transmit(&hspi1, (uint8_t *)&tx, 1, HAL_MAX_DELAY);
// }
static inline void LCD_SPI_Write(uint8_t data, uint8_t isData)
{
    uint8_t tx1, tx2;

    /*
     * 9bit = [D/C][D7..D0]
     * 拆成两个 8bit：
     * tx1: bit7=D/C, bit6..0=data[7..1]
     * tx2: bit7=data[0], bit6..0=0
     */
    tx1 = (isData << 7) | (data >> 1);
    tx2 = (data & 0x01) << 7;

    HAL_SPI_Transmit(&hspi1, &tx1, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, &tx2, 1, HAL_MAX_DELAY);
}

void LCD_WR_DATA(uint8_t data)
{
    lcd_dc_set();
    lcd_cs_clr();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    lcd_cs_set();
}

void LCD_WR_REG(uint8_t data)
{
    lcd_dc_clr();
    lcd_cs_clr();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    lcd_cs_set();
}

void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
    lcd_dc_clr();
    lcd_cs_clr();
    HAL_SPI_Transmit(&hspi1, &LCD_Reg, 1, HAL_MAX_DELAY);
    lcd_cs_set();

    lcd_dc_set();
    lcd_cs_clr();
    uint8_t val[2] = { LCD_RegValue >> 8, LCD_RegValue & 0xFF };
    HAL_SPI_Transmit(&hspi1, val, 2, HAL_MAX_DELAY);
    lcd_cs_set();
}

void Lcd_WriteData_16Bit(uint16_t color)
{
    lcd_dc_set();
    lcd_cs_clr();
    uint8_t buf[2] = { color >> 8, color & 0xFF };
    HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    lcd_cs_set();
}

// ================= LCD 控制 =================
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LCD_RST_PIN | LCD_DC_PIN | LCD_CS_PIN;
    HAL_GPIO_Init(LCD_RST_PORT == GPIOB ? GPIOB : GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD_BL_PIN;
    HAL_GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);
}

void LCD_RESET(void)
{
    lcd_rst_clr();
    HAL_Delay(100);
    lcd_rst_set();
    HAL_Delay(50);
}

void LCD_Init(void)
{
    LCD_GPIOInit();
    LCD_RESET();
    lcd_bl_on();
    HAL_Delay(200);

    //************* Start Initial Sequence **********// 
    LCD_WriteReg(0x36, 0x00); 

    LCD_WriteReg(0x3A, 0x05); // 16位色

    LCD_WriteReg(0xB2, 0x0C);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x33);

    LCD_WriteReg(0xB7, 0x35);  

    LCD_WriteReg(0xBB, 0x19);

    LCD_WriteReg(0xC0, 0x2C);

    LCD_WriteReg(0xC2, 0x01);
    LCD_WriteReg(0xC3, 0x12);   

    LCD_WriteReg(0xC4, 0x20);  

    LCD_WriteReg(0xC6, 0x0F);    

    LCD_WriteReg(0xD0, 0xA4);
    LCD_WR_DATA(0xA1);

    LCD_WriteReg(0xE0, 0xD0);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x4C);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x23);

    LCD_WriteReg(0xE1, 0xD0);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x1F);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x23);

    LCD_WR_REG(0x21); // INVON - Display Inversion On

    LCD_WR_REG(0x11); // SLPOUT - Sleep Out
    HAL_Delay(120); 

    LCD_WR_REG(0x29); // DISPON - Display On

    // 设置列地址 (0 to 239)
    LCD_WR_REG(0x2A); // 列地址设置
    LCD_WR_DATA(0x00); // 起始列地址高位
    LCD_WR_DATA(0x00); // 起始列地址低位
    LCD_WR_DATA(0x00); // 结束列地址高位
    LCD_WR_DATA(0xEF); // 结束列地址低位（240 列）

    // 设置行地址 
    LCD_WR_REG(0x2B); // 行地址设置
    LCD_WR_DATA(0x00); // 起始行地址高位
    LCD_WR_DATA(0x00); // 起始行地址低位
    LCD_WR_DATA(0x01); // 结束行地址高位
    LCD_WR_DATA(0x17); // 结束行地址低位（279 行，0x117 = 279）

    LCD_WR_REG(0x2C); // RAMWR - 内存写入

    LCD_direction(USE_HORIZONTAL);
    LCD_Clear(BLUE);
}

void LCD_Clear(uint16_t Color)
{
    LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);
    lcd_dc_set();
    lcd_cs_clr();

    for(uint32_t i=0; i<lcddev.width * lcddev.height; i++)
    {
        uint8_t buf[2] = { Color >> 8, Color & 0xFF };
        HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    }

    lcd_cs_set();
}

void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(xStar & 0xFF);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(xEnd & 0xFF);
    
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(yStar & 0xFF);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(yEnd & 0xFF);
    
    LCD_WriteRAM_Prepare();
}

void LCD_WriteRAM_Prepare(void)
{
    LCD_WriteReg(lcddev.wramcmd, 0);
}

void LCD_direction(uint8_t direction)
{
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
    lcddev.wramcmd = 0x2C;

    switch(direction)
    {
        case 0: lcddev.width=LCD_W; lcddev.height=LCD_H; lcddev.xoffset=0; lcddev.yoffset=0; LCD_WriteReg(0x36,0x00); break;
        case 1: lcddev.width=LCD_H; lcddev.height=LCD_W; lcddev.xoffset=0; lcddev.yoffset=0; LCD_WriteReg(0x36,0x60); break;
        case 2: lcddev.width=LCD_W; lcddev.height=LCD_H; lcddev.xoffset=0; lcddev.yoffset=37; LCD_WriteReg(0x36,0xC0); break;
        case 3: lcddev.width=LCD_H; lcddev.height=LCD_W; lcddev.xoffset=37; lcddev.yoffset=0; LCD_WriteReg(0x36,0xA0); break;
    }
}
