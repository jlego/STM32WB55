#include "main.h"
#include "st7789.h"
#include <stdbool.h>
#include <string.h>
#include "core_cm4.h"  // 用于访问DWT寄存器

#ifdef USE_DMA
uint16_t DMA_MIN_SIZE = 16;
uint16_t disp_buf[ST7789_WIDTH * HOR_LEN];
#endif

// SPI句柄（从main.c引入）
extern SPI_HandleTypeDef hspi1;

// TIM2 PWM句柄（使用main.c中的htim2，由MX_TIM2_Init初始化）
extern TIM_HandleTypeDef htim2;

static uint8_t bl_brightness = 100;

/* DMA等待函数 - 带超时保护，防止永久阻塞 */
static void ST7789_WaitForDMA(uint32_t timeout_ms)
{
    uint32_t start_tick = HAL_GetTick();
    while (hspi1.hdmatx->State != HAL_DMA_STATE_READY) {
        /* 检查超时 */
        if ((HAL_GetTick() - start_tick) > timeout_ms) {
            break;  // 超时退出，防止永久阻塞
        }
        /* 检查并恢复 SysTick 中断 */
        if ((SysTick->CTRL & SysTick_CTRL_TICKINT_Msk) == 0) {
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
        }
    }
}

#define ABS(x) ((x) > 0 ? (x) : -(x))

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

/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */
void ST7789_WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, sizeof(cmd), 100);  // 添加100ms超时
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
void ST7789_WriteData(uint8_t *buff, size_t buff_size)
{
    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

    while (buff_size > 0) {
        uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
#ifdef USE_DMA
        if (DMA_MIN_SIZE <= buff_size) {
            HAL_SPI_Transmit_DMA(&hspi1, buff, chunk_size);
            ST7789_WaitForDMA(500);  // 500ms超时
        } else {
            HAL_SPI_Transmit(&hspi1, buff, chunk_size, 500);  // 500ms超时
        }
#else
        HAL_SPI_Transmit(&hspi1, buff, chunk_size, 500);  // 500ms超时
#endif
        buff += chunk_size;
        buff_size -= chunk_size;
    }

    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief Write data to ST7789 controller, simplify for 16bit data.
 * @param data -> data to write
 * @return none
 */
void ST7789_WriteData16(uint16_t data)
{
    uint8_t dataBuf[2] = {data >> 8, data & 0xFF};
    ST7789_WriteData(dataBuf, sizeof(dataBuf));
}

/**
 * @brief Set address of DisplayWindow
 * @param x0&y0 -> coordinates of start point
 * @param x1&y1 -> coordinates of end point
 * @return none
 */
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint16_t x_start = x0 + DISPLAY_OFFSET_X;
    uint16_t y_start = y0 + DISPLAY_OFFSET_Y;
    uint16_t x_end = x1 + DISPLAY_OFFSET_X;
    uint16_t y_end = y1 + DISPLAY_OFFSET_Y;

    /* Column Address set */
    ST7789_WriteCommand(ST7789_CASET);
    {
        uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
        ST7789_WriteData(data, sizeof(data));
    }

    /* Row Address set */
    ST7789_WriteCommand(ST7789_RASET);
    {
        uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
        ST7789_WriteData(data, sizeof(data));
    }
    /* Write to RAM */
    ST7789_WriteCommand(ST7789_RAMWR);
}

// 延时函数 - 使用DWT周期计数器，不依赖SysTick
void ST7789_Delay(uint32_t ms) {
    /* 启用DWT周期计数器 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    /* STM32WB55 SYSCLK=64MHz, 1ms ≈ 64000 cycles */
    uint32_t cycles_per_ms = 64000;
    uint32_t target_cycles = ms * cycles_per_ms;
    
    /* 读取当前周期计数 */
    uint32_t start = DWT->CYCCNT;
    
    /* 等待直到达到目标周期数 */
    while ((DWT->CYCCNT - start) < target_cycles) {
        /* 软件超时：防止DWT溢出或其他问题 */
        volatile uint32_t timeout = 10000000;
        if ((DWT->CYCCNT - start) > 100000000) break; /* 约1.5秒超时 */
    }
}

/**
 * @brief Initialize ST7789 controller
 * @param none
 * @return none
 */
void ST7789_Init(void)
{
#ifdef USE_DMA
    memset(disp_buf, 0, sizeof(disp_buf));
#endif

    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 配置LCD控制引脚
    GPIO_InitStruct.Pin = LCD_RST_PIN | LCD_DC_PIN | LCD_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_RST_PORT, &GPIO_InitStruct);
    HAL_GPIO_Init(LCD_DC_PORT, &GPIO_InitStruct);
    HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);

    // 硬件复位
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    ST7789_Delay(10);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    ST7789_Delay(20);

    // ST7789初始化序列
    ST7789_WriteCommand(ST7789_COLMOD);     // Set color mode
    {
        uint8_t data = 0x55;                // 16bit RGB565
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xB2);              // Porch control
    {
        uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
        ST7789_WriteData(data, sizeof(data));
    }

    ST7789_WriteCommand(0xB7);              // Gate Control
    {
        uint8_t data = 0x35;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xBB);              // VCOM setting
    {
        uint8_t data = 0x19;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xC0);              // LCMCTRL
    {
        uint8_t data = 0x2C;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xC2);              // VDV and VRH command Enable
    {
        uint8_t data = 0x01;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xC3);              // VRH set
    {
        uint8_t data = 0x12;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xC4);              // VDV set
    {
        uint8_t data = 0x20;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xC6);              // Frame rate control
    {
        uint8_t data = 0x0F;
        ST7789_WriteData(&data, 1);
    }

    ST7789_WriteCommand(0xD0);              // Power control
    {
        uint8_t data[] = {0xA4, 0xA1};
        ST7789_WriteData(data, sizeof(data));
    }

    // Gamma curve settings
    ST7789_WriteCommand(0xE0);
    {
        uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
        ST7789_WriteData(data, sizeof(data));
    }

    ST7789_WriteCommand(0xE1);
    {
        uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
        ST7789_WriteData(data, sizeof(data));
    }

    ST7789_WriteCommand(ST7789_INVON);      // Inversion ON
    ST7789_WriteCommand(ST7789_SLPOUT);     // Out of sleep mode
    ST7789_Delay(120);
    ST7789_WriteCommand(ST7789_NORON);      // Normal Display on
    ST7789_WriteCommand(ST7789_DISPON);     // Main screen turned on
    ST7789_Delay(50);

    // 设置显示窗口
    ST7789_WriteCommand(ST7789_CASET);
    {
        uint16_t x_start = DISPLAY_OFFSET_X;
        uint16_t x_end = DISPLAY_OFFSET_X + DISPLAY_WIDTH - 1;
        uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
        ST7789_WriteData(data, sizeof(data));
    }

    ST7789_WriteCommand(ST7789_RASET);
    {
        uint16_t y_start = DISPLAY_OFFSET_Y;
        uint16_t y_end = DISPLAY_OFFSET_Y + DISPLAY_HEIGHT - 1;
        uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
        ST7789_WriteData(data, sizeof(data));
    }

    // 初始化PWM背光
    ST7789_PWM_Init();
    ST7789_PWM_SetDuty(10);

    // 全黑清屏
    ST7789_Fill_Color(COLOR_BLACK);
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill_Color(uint16_t color)
{
    uint16_t i;
    ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);

#ifdef USE_DMA
    for (i = 0; i < ST7789_HEIGHT / HOR_LEN; i++) {
        memset(disp_buf, color, sizeof(disp_buf));
        ST7789_WriteData((uint8_t *)disp_buf, sizeof(disp_buf));
    }
#else
    uint16_t j;
    for (i = 0; i < ST7789_WIDTH; i++) {
        for (j = 0; j < ST7789_HEIGHT; j++) {
            uint8_t data[] = {color >> 8, color & 0xFF};
            ST7789_WriteData(data, sizeof(data));
        }
    }
#endif
}

/**
 * @brief Fill Screen with single color (alias for Fill_Color)
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_FillScreen(uint16_t color)
{
    ST7789_Fill_Color(color);
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x < 0) || (x >= ST7789_WIDTH) || (y < 0) || (y >= ST7789_HEIGHT)) return;
    
    ST7789_SetAddressWindow(x, y, x, y);
    uint8_t data[] = {color >> 8, color & 0xFF};
    ST7789_WriteData(data, sizeof(data));
}

/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
    if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) || (yEnd < 0) || (yEnd >= ST7789_HEIGHT)) return;
    
    uint16_t i, j;
    ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
    for (i = ySta; i <= yEnd; i++) {
        for (j = xSta; j <= xEnd; j++) {
            uint8_t data[] = {color >> 8, color & 0xFF};
            ST7789_WriteData(data, sizeof(data));
        }
    }
}

/**
 * @brief Draw a big Pixel at a point (4px)
 * @param x&y -> coordinate of the point
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x <= 0) || (x > ST7789_WIDTH) || (y <= 0) || (y > ST7789_HEIGHT)) return;
    ST7789_Fill(x - 1, y - 1, x + 1, y + 1, color);
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
        swap = x0; x0 = y0; y0 = swap;
        swap = x1; x1 = y1; y1 = swap;
    }

    if (x0 > x1) {
        swap = x0; x0 = x1; x1 = swap;
        swap = y0; y0 = y1; y1 = swap;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            ST7789_DrawPixel(y0, x0, color);
        } else {
            ST7789_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**
 * @brief Draw a Rectangle with single color
 * @param x1&y1 -> coordinate of top-left point
 * @param x2&y2 -> coordinate of bottom-right point
 * @param color -> color of the Rectangle line
 * @return none
 */
void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    ST7789_DrawLine(x1, y1, x2, y1, color);
    ST7789_DrawLine(x1, y1, x1, y2, color);
    ST7789_DrawLine(x1, y2, x2, y2, color);
    ST7789_DrawLine(x2, y1, x2, y2, color);
}

/**
 * @brief Draw a circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle line
 * @return none
 */
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ST7789_DrawPixel(x0, y0 + r, color);
    ST7789_DrawPixel(x0, y0 - r, color);
    ST7789_DrawPixel(x0 + r, y0, color);
    ST7789_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7789_DrawPixel(x0 + x, y0 + y, color);
        ST7789_DrawPixel(x0 - x, y0 + y, color);
        ST7789_DrawPixel(x0 + x, y0 - y, color);
        ST7789_DrawPixel(x0 - x, y0 - y, color);

        ST7789_DrawPixel(x0 + y, y0 + x, color);
        ST7789_DrawPixel(x0 - y, y0 + x, color);
        ST7789_DrawPixel(x0 + y, y0 - x, color);
        ST7789_DrawPixel(x0 - y, y0 - x, color);
    }
}

/**
 * @brief Draw an Image on the screen
 * @param x&y -> start point of the Image
 * @param w&h -> width & height of the Image to Draw
 * @param data -> pointer of the Image array
 * @return none
 */
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
    if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT)) return;
    if ((x + w - 1) >= ST7789_WIDTH) return;
    if ((y + h - 1) >= ST7789_HEIGHT) return;

    ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
    ST7789_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
}

/**
 * @brief Invert Fullscreen color
 * @param invert -> Whether to invert
 * @return none
 */
void ST7789_InvertColors(uint8_t invert)
{
    ST7789_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
}

/**
 * @brief Write a char
 * @param x&y -> cursor of the start point
 * @param ch -> char to write
 * @param font -> fontstyle of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return none
 */
void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;
    ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++) {
            if ((b << j) & 0x8000) {
                uint8_t data[] = {color >> 8, color & 0xFF};
                ST7789_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
                ST7789_WriteData(data, sizeof(data));
            }
        }
    }
}

/**
 * @brief Write a string
 * @param x&y -> cursor of the start point
 * @param str -> string to write
 * @param font -> fontstyle of the string
 * @param color -> color of the string
 * @param bgcolor -> background color of the string
 * @return none
 */
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    while (*str) {
        if (x + font.width >= ST7789_WIDTH) {
            x = 0;
            y += font.height;
            if (y + font.height >= ST7789_HEIGHT) {
                break;
            }

            if (*str == ' ') {
                str++;
                continue;
            }
        }
        ST7789_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
}

/**
 * @brief Draw a filled Rectangle with single color
 * @param x&y -> coordinates of the starting point
 * @param w&h -> width & height of the Rectangle
 * @param color -> color of the Rectangle
 * @return none
 */
void ST7789_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t i;

    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT) {
        return;
    }

    if ((x + w) >= ST7789_WIDTH) {
        w = ST7789_WIDTH - x;
    }
    if ((y + h) >= ST7789_HEIGHT) {
        h = ST7789_HEIGHT - y;
    }

    for (i = 0; i <= h; i++) {
        ST7789_DrawLine(x, y + i, x + w, y + i, color);
    }
}

/**
 * @brief Draw a Triangle with single color
 * @param x1&y1 -> coordinate of point 1
 * @param x2&y2 -> coordinate of point 2
 * @param x3&y3 -> coordinate of point 3
 * @param color -> color of the lines
 * @return none
 */
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
    ST7789_DrawLine(x1, y1, x2, y2, color);
    ST7789_DrawLine(x2, y2, x3, y3, color);
    ST7789_DrawLine(x3, y3, x1, y1, color);
}

/**
 * @brief Draw a filled Triangle with single color
 * @param x1&y1 -> coordinate of point 1
 * @param x2&y2 -> coordinate of point 2
 * @param x3&y3 -> coordinate of point 3
 * @param color -> color of the triangle
 * @return none
 */
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1; xinc2 = 1;
    } else {
        xinc1 = -1; xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1; yinc2 = 1;
    } else {
        yinc1 = -1; yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0; yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0; yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        ST7789_DrawLine(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

/**
 * @brief Draw a Filled circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle
 * @return none
 */
void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ST7789_DrawPixel(x0, y0 + r, color);
    ST7789_DrawPixel(x0, y0 - r, color);
    ST7789_DrawPixel(x0 + r, y0, color);
    ST7789_DrawPixel(x0 - r, y0, color);
    ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

/**
 * @brief A Simple test function for ST7789
 * @param none
 * @return none
 */
void ST7789_Test(void)
{
    ST7789_Fill_Color(COLOR_WHITE);
    ST7789_Delay(1000);
    ST7789_WriteString(10, 20, "Speed Test", Font_11x18, COLOR_RED, COLOR_WHITE);
    ST7789_Delay(1000);
    ST7789_Fill_Color(COLOR_CYAN);
    ST7789_Delay(500);
    ST7789_Fill_Color(COLOR_RED);
    ST7789_Delay(500);
    ST7789_Fill_Color(COLOR_BLUE);
    ST7789_Delay(500);
    ST7789_Fill_Color(COLOR_GREEN);
    ST7789_Delay(500);
    ST7789_Fill_Color(COLOR_YELLOW);
    ST7789_Delay(500);
    ST7789_Fill_Color(COLOR_WHITE);
    ST7789_Delay(500);

    ST7789_WriteString(10, 10, "Font test.", Font_16x26, COLOR_GBLUE, COLOR_WHITE);
    ST7789_WriteString(10, 50, "Hello!", Font_7x10, COLOR_RED, COLOR_WHITE);
    ST7789_WriteString(10, 75, "Hello!", Font_11x18, COLOR_YELLOW, COLOR_WHITE);
    ST7789_WriteString(10, 100, "Hello!", Font_16x26, COLOR_MAGENTA, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Rect./Line.", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawRectangle(30, 30, 100, 100, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Filled Rect.", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawFilledRectangle(30, 30, 50, 50, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Circle.", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawCircle(60, 60, 25, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Filled Cir.", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawFilledCircle(60, 60, 25, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Triangle", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawTriangle(30, 30, 30, 70, 60, 40, COLOR_WHITE);
    ST7789_Delay(1000);

    ST7789_Fill_Color(COLOR_RED);
    ST7789_WriteString(10, 10, "Filled Tri", Font_11x18, COLOR_YELLOW, COLOR_BLACK);
    ST7789_DrawFilledTriangle(30, 30, 30, 70, 60, 40, COLOR_WHITE);
    ST7789_Delay(1000);
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