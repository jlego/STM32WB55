#include "lvgl_stm32.h"
#include "infinitime_adapter.h"
#include "st7789.h"
#include "ft3168_touch.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_it.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

/* DWT是否工作 */
static volatile bool dwt_working = false;

/* DWT周期计数器初始化 - 用于不依赖SysTick的超时检测 */
static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    /* 验证 CYCCNT 是否工作：读取两次，确保值在变化 */
    uint32_t t1 = DWT->CYCCNT;
    for (volatile int i = 0; i < 1000; i++); /* 短暂延迟 */
    uint32_t t2 = DWT->CYCCNT;
    dwt_working = (t2 != t1);
}

/* 获取DWT周期数（假设CPU频率64MHz，1ms ≈ 64000 cycles） */
static inline uint32_t DWT_GetTicks(void)
{
    return DWT->CYCCNT;
}

/* DWT周期转毫秒 - 使用正确的64MHz系统时钟 */
#define DWT_CYCLES_PER_MS (64000)  /* STM32WB55 PLL配置后SYSCLK=64MHz */

/* 软件延迟函数 - 使用DWT周期计数器，不依赖SysTick */
static void sw_delay_ms(uint32_t ms)
{
    if (!dwt_working) {
        /* 如果DWT不工作，使用简单的软件循环 */
        volatile uint32_t count = ms * 10000;
        while (count--) {
            __NOP();
        }
        return;
    }
    
    uint32_t start = DWT->CYCCNT;
    uint32_t target_cycles = ms * DWT_CYCLES_PER_MS;
    
    /* 等待直到达到目标周期数 */
    while ((DWT->CYCCNT - start) < target_cycles) {
        /* 软件超时：防止DWT溢出或其他问题 */
        if ((DWT->CYCCNT - start) > 100000000) break; /* 约1.5秒超时 */
    }
}

/* 软件超时计数器 - 不依赖DWT */
static volatile uint32_t sw_timeout_counter = 0;

/* 保存显示驱动指针，用于DMA完成回调 */
static lv_disp_drv_t *current_disp_drv = NULL;
static volatile bool dma_transfer_in_progress = false;
static volatile uint32_t dma_err_count = 0;
static volatile uint32_t dma_half_count = 0;

static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 100];
static lv_color_t buf2[LV_HOR_RES_MAX * 50];

/* DMA传输专用buffer，防止LVGL在DMA传输时覆盖数据 */
#define DMA_TRANSFER_BUF_SIZE (LV_HOR_RES_MAX * 60)
static lv_color_t dma_transfer_buf[DMA_TRANSFER_BUF_SIZE];

static lv_indev_drv_t touch_indev_drv;
static lv_indev_t *touch_indev = NULL;
static bool debug_enabled = true;

/* LVGL 调试图层 label（在 system layer 上，始终置顶） */
static lv_obj_t *dbg_label = NULL;

/* SPI 错误计数 */
static volatile uint32_t spi_err_count = 0;
static volatile uint32_t dma_complete_count = 0;
static volatile uint32_t dma_start_count = 0;
static volatile uint32_t dma_timeout_count = 0;

/* DWT-based counter that increments even if SysTick stops */
static volatile uint32_t dwt_debug_counter = 0;

/* Increment DWT debug counter - called from main loop */
void lvgl_increment_dwt_counter(void)
{
    dwt_debug_counter++;
}

/* Get DWT debug counter value */
uint32_t lvgl_get_dwt_counter(void)
{
    return dwt_debug_counter;
}

/* Get DWT CYCCNT value */
uint32_t lvgl_get_dwt_cyccnt(void)
{
    return DWT->CYCCNT;
}

/* 5x7 ASCII字体位图 */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /*   */
    {0x00,0x00,0x5F,0x00,0x00}, /* ! */
    {0x00,0x07,0x00,0x07,0x00}, /* " */
    {0x14,0x7F,0x14,0x7F,0x14}, /* # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
    {0x23,0x13,0x08,0x64,0x62}, /* % */
    {0x36,0x49,0x55,0x22,0x50}, /* & */
    {0x00,0x05,0x03,0x00,0x00}, /* ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* ( */
    {0x00,0x41,0x22,0x1C,0x00}, /* ) */
    {0x14,0x08,0x3E,0x08,0x14}, /* * */
    {0x08,0x08,0x3E,0x08,0x08}, /* + */
    {0x00,0x50,0x30,0x00,0x00}, /* , */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x00,0x60,0x60,0x00,0x00}, /* . */
    {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
    {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 2 */
    {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1E}, /* 9 */
    {0x00,0x36,0x36,0x00,0x00}, /* : */
    {0x00,0x56,0x36,0x00,0x00}, /* ; */
    {0x08,0x14,0x22,0x41,0x00}, /* < */
    {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x00,0x41,0x22,0x14,0x08}, /* > */
    {0x02,0x01,0x51,0x09,0x06}, /* ? */
    {0x32,0x49,0x79,0x41,0x3E}, /* @ */
    {0x7E,0x11,0x11,0x11,0x7E}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */
    {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x22,0x1C}, /* D */
    {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */
    {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */
    {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7F,0x01,0x01}, /* T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}, /* Z */
};

#define DEBUG_BG_X     27
#define DEBUG_BG_Y     2
#define DEBUG_BG_W     200
#define DEBUG_BG_H     14

static void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color)
{
    uint8_t idx;
    if (c >= ' ' && c <= 'z') idx = c - ' ';
    else if (c == '\n' || c == '\r') return;
    else idx = 0;
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x7[idx][col];
        for (uint8_t row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                ST7789_DrawPixel(x + col, y + row, color);
            }
        }
    }
}

/* 软件超时等待DMA完成 - 不依赖DWT */
static void wait_dma_complete(uint32_t timeout_ms)
{
    /* 使用简单的软件循环超时 */
    /* 64MHz CPU，每个循环约几个周期，100ms约需数百万次循环 */
    volatile uint32_t count = 0;
    volatile uint32_t max_count = timeout_ms * 50000; /* 增大超时计数 */
    
    while (dma_transfer_in_progress) {
        count++;
        if (count >= max_count) {
            /* 超时，强制重置状态 */
            dma_timeout_count++;
            dma_err_count++;
            dma_transfer_in_progress = false;
            /* 关键修复：必须调用 lv_disp_flush_ready，否则 LVGL 会永久等待 */
            if (current_disp_drv) {
                lv_disp_flush_ready(current_disp_drv);
                current_disp_drv = NULL;
            }
            break;
        }
    }
}

static void st7789_draw_debug_text(const char *str)
{
    /* LV_COLOR_16_SWAP=1，所以颜色值需要字节交换：0x07E0 → 0xE007 */
    uint16_t fg_color = 0xE007;
    static uint16_t render_buf[DEBUG_BG_W * DEBUG_BG_H];
    
    /* 清空渲染缓冲区为黑色 */
    for (int i = 0; i < DEBUG_BG_W * DEBUG_BG_H; i++) {
        render_buf[i] = 0x0000;
    }
    
    /* 在内存中渲染字符 */
    uint16_t str_len = 0;
    for (uint16_t i = 0; str[i] && i < 30; i++) {
        char c = str[i];
        uint8_t idx;
        if (c >= ' ' && c <= 'z') idx = c - ' ';
        else if (c == '\n' || c == '\r') continue;
        else idx = 0;
        
        uint16_t char_x = 2 + i * 6;
        uint16_t char_y = 2;
        
        for (uint8_t col = 0; col < 5; col++) {
            uint8_t line = font5x7[idx][col];
            for (uint8_t row = 0; row < 7; row++) {
                if (line & (1 << row)) {
                    uint16_t px = char_x + col;
                    uint16_t py = char_y + row;
                    if (px < DEBUG_BG_W && py < DEBUG_BG_H) {
                        render_buf[py * DEBUG_BG_W + px] = fg_color;
                    }
                }
            }
        }
        str_len++;
    }
    
    /* 等待之前的DMA传输完成 */
    wait_dma_complete(100);
    
    /* 使用DMA传输（带DWT超时保护） */
    ST7789_SetAddressWindow(DEBUG_BG_X, DEBUG_BG_Y, DEBUG_BG_W, DEBUG_BG_H);
    lcd_dc_set();
    lcd_cs_clr();
    
    /* 使用DMA传输 */
    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)render_buf, DEBUG_BG_W * DEBUG_BG_H * 2);
    if (status != HAL_OK) {
        /* DMA启动失败，标记错误并返回 */
        dma_err_count++;
        spi_err_count++;
        lcd_cs_set();
        return;
    }
    
    /* 等待本次DMA传输完成 */
    wait_dma_complete(100);
    
    lcd_cs_set();
}

/* 显示刷新回调函数 - 启动DMA后立即返回,不阻塞等待 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t width = area->x2 - area->x1 + 1;
    uint16_t height = area->y2 - area->y1 + 1;
    int32_t total_pixels = width * height;
    
    if (total_pixels <= 0) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    /* 如果DMA传输正在进行,强制等待完成(使用软件超时) */
    if (dma_transfer_in_progress) {
        wait_dma_complete(100);
    }
    
    ST7789_SetAddressWindow(area->x1, area->y1, area->x2, area->y2);
    
    lcd_dc_set();
    lcd_cs_clr();
    
    /* 复制数据到DMA专用buffer */
    if (total_pixels <= DMA_TRANSFER_BUF_SIZE) {
        memcpy(dma_transfer_buf, color_p, total_pixels * sizeof(lv_color_t));
    } else {
        /* 超大区域,分段传输 */
        total_pixels = DMA_TRANSFER_BUF_SIZE;
        memcpy(dma_transfer_buf, color_p, total_pixels * sizeof(lv_color_t));
    }
    
    /* 保存显示驱动指针,用于DMA完成回调 */
    current_disp_drv = disp_drv;
    dma_transfer_in_progress = true;
    dma_start_count++;
    
    /* 使用DMA非阻塞传输 */
    HAL_StatusTypeDef spi_status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)dma_transfer_buf, total_pixels * sizeof(lv_color_t));
    
    if (spi_status != HAL_OK) {
        spi_err_count++;
        lcd_cs_set();
        current_disp_drv = NULL;
        dma_transfer_in_progress = false;
        /* DMA启动失败,直接调用flush_ready */
        if (spi_status == HAL_BUSY) {
            hspi1.State = HAL_SPI_STATE_READY;
        }
        lv_disp_flush_ready(disp_drv);
    } else {
        /* 关键修复:DMA启动成功后立即标记刷新完成,不等待DMA回调 */
        /* 这样LVGL不会阻塞在while(vdb->flushing)上 */
        lv_disp_flush_ready(disp_drv);
    }
}

/* DMA传输完成回调 - 仅更新计数,不再调用lv_disp_flush_ready */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        dma_complete_count++;
        lcd_cs_set();
        /* 不再调用lv_disp_flush_ready,因为disp_flush已经立即返回 */
        current_disp_drv = NULL;
        dma_transfer_in_progress = false;
    }
}

/* DMA半传输回调 */
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        dma_half_count++;
    }
}

/* DMA错误回调 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        dma_err_count++;
        lcd_cs_set();
        
        /* 重置SPI句柄状态，防止后续传输永久阻塞 */
        hspi->State = HAL_SPI_STATE_READY;
        hspi->ErrorCode = HAL_SPI_ERROR_NONE;
        
        /* 确保总是调用 lv_disp_flush_ready，即使 current_disp_drv 为 NULL */
        if (current_disp_drv) {
            lv_disp_flush_ready(current_disp_drv);
            current_disp_drv = NULL;
        } else {
            /* current_disp_drv 为 NULL，尝试获取当前显示驱动 */
            lv_disp_t * disp = lv_disp_get_default();
            if (disp) {
                lv_disp_flush_ready(&disp->driver);
            }
        }
        dma_transfer_in_progress = false;
    }
}

/* 中断驱动的触摸数据缓存 */
static volatile bool touch_irq_flag = false;
static volatile lv_coord_t touch_buf_x = 0;
static volatile lv_coord_t touch_buf_y = 0;
static volatile bool touch_buf_pressed = false;
static volatile uint32_t touch_irq_count = 0;
static volatile uint32_t touch_read_ok_count = 0;
static volatile uint8_t ft_raw[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static volatile uint8_t ft_chip_id = 0xFF;
static volatile uint8_t ft_vend_id = 0xFF;
static volatile uint8_t ft_pwr_mode = 0xFF;
static volatile uint8_t ft_threshold = 0xFF;
static volatile uint8_t ft_int_mode = 0xFF;
static bool ft_diag_done = false;
static volatile uint32_t i2c_err_count = 0;

/* 获取 SPI 错误计数 */
uint32_t lvgl_get_spi_err_count(void)
{
    return spi_err_count;
}

static void i2c_bus_recover(void)
{
    extern I2C_HandleTypeDef hi2c1;
    
    HAL_I2C_DeInit(&hi2c1);
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &g);
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    for (int i = 0; i < 18; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++) __NOP();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        for (volatile int d = 0; d < 100; d++) __NOP();
    }
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++) __NOP();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    for (volatile int d = 0; d < 100; d++) __NOP();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++) __NOP();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    for (volatile int d = 0; d < 100; d++) __NOP();
    
    g.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    g.Mode = GPIO_MODE_AF_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &g);
    
    HAL_I2C_Init(&hi2c1);
}

static bool i2c_safe_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    extern I2C_HandleTypeDef hi2c1;
    if (hi2c1.State != HAL_I2C_STATE_READY) {
        i2c_bus_recover();
        i2c_err_count++;
    }
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(&hi2c1, (0x38 << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, 50);
    if (st != HAL_OK) {
        i2c_bus_recover();
        i2c_err_count++;
        return false;
    }
    return true;
}

static bool i2c_safe_write(uint8_t reg, uint8_t val)
{
    extern I2C_HandleTypeDef hi2c1;
    if (hi2c1.State != HAL_I2C_STATE_READY) {
        i2c_bus_recover();
        i2c_err_count++;
    }
    HAL_StatusTypeDef st = HAL_I2C_Mem_Write(&hi2c1, (0x38 << 1), reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 50);
    if (st != HAL_OK) {
        i2c_bus_recover();
        i2c_err_count++;
        return false;
    }
    return true;
}

static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /* 纯中断驱动：只返回缓存数据，不做任何 I2C/GPIO 操作 */
    data->point.x = touch_buf_x;
    data->point.y = touch_buf_y;
    data->state = touch_buf_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    
    return false;
}

/* 等待回调 - LVGL在等待DMA完成时调用此函数 */
static void disp_wait_cb(lv_disp_drv_t * disp_drv)
{
    /* 使用DWT周期计数器检测超时，不依赖SysTick */
    static uint32_t wait_start = 0;
    
    if (wait_start == 0) {
        wait_start = DWT_GetTicks();
    }
    
    uint32_t elapsed = DWT_GetTicks() - wait_start;
    uint32_t timeout_cycles = 500 * DWT_CYCLES_PER_MS; /* 500ms超时 */
    
    if (elapsed > timeout_cycles) {
        /* 超时，强制重置状态 */
        dma_timeout_count++;
        dma_err_count++;
        dma_transfer_in_progress = false;
        if (current_disp_drv) {
            lv_disp_flush_ready(current_disp_drv);
            current_disp_drv = NULL;
        }
        wait_start = 0;
    }
}

void lvgl_init(void)
{
    /* 初始化DWT周期计数器（用于不依赖SysTick的超时检测） */
    DWT_Init();
    
    lv_init();
    
    /* 使用单缓冲区，避免双缓冲模式下的 while(vdb->flushing) 阻塞循环 */
    lv_disp_buf_init(&disp_buf, buf1, NULL, LV_HOR_RES_MAX * 40);
    
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_flush;
    /* 关键修复：启用 wait_cb，LVGL 在等待 lv_disp_flush_ready 时会调用此回调 */
    disp_drv.wait_cb = disp_wait_cb;
    disp_drv.buffer = &disp_buf;
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    lv_disp_drv_register(&disp_drv);
    
    lv_indev_drv_init(&touch_indev_drv);
    touch_indev_drv.type = LV_INDEV_TYPE_POINTER;
    touch_indev_drv.read_cb = touchpad_read;
    touch_indev = lv_indev_drv_register(&touch_indev_drv);
    
    /* 初始时禁用触摸输入，等待触摸中断触发后再启用 */
    if (touch_indev) {
        lv_indev_enable(touch_indev, false);
    }
    
    /* 创建调试图层 label（在 system layer 上，始终置顶） */
    dbg_label = lv_label_create(lv_disp_get_layer_sys(NULL), NULL);
    lv_obj_set_style_local_text_color(dbg_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FF00));
    lv_obj_set_style_local_text_font(dbg_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
    lv_obj_set_pos(dbg_label, 25, 5);
    lv_label_set_long_mode(dbg_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(dbg_label, lv_disp_get_hor_res(NULL) - 30, 60);
    lv_label_set_text(dbg_label, "");
}

/* 触摸中断处理函数 - 在 EXTI 回调中调用，只设标志 */
void lvgl_touch_irq_handler(void)
{
    touch_irq_flag = true;
    touch_irq_count++;
}

/* 触摸释放处理函数 - 在触摸释放后调用 */
void lvgl_touch_release_handler(void)
{
    touch_buf_pressed = false;
    if (touch_indev) {
        lv_indev_enable(touch_indev, false);
    }
}

/* 主循环中处理触摸中断标志 - 读取 I2C 并缓存数据 */
void lvgl_touch_process(void)
{
    if (!touch_irq_flag && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET) {
        touch_irq_flag = true;
        touch_irq_count++;
    }
    if (!touch_irq_flag) return;
    touch_irq_flag = false;
    
    if (!ft_diag_done) {
        ft_diag_done = true;
        
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        sw_delay_ms(30);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        sw_delay_ms(160);
        
        i2c_safe_read(0xA3, (uint8_t*)&ft_chip_id, 1);
        i2c_safe_read(0xA8, (uint8_t*)&ft_vend_id, 1);
        i2c_safe_read(0xA5, (uint8_t*)&ft_pwr_mode, 1);
        i2c_safe_read(0x80, (uint8_t*)&ft_threshold, 1);
        i2c_safe_read(0xA4, (uint8_t*)&ft_int_mode, 1);
        i2c_safe_write(0xA5, 0x00);
        i2c_safe_write(0x80, 0x05);
        i2c_safe_write(0xA4, 0x00);
        i2c_safe_write(0x00, 0x01);
        sw_delay_ms(50);
        
        i2c_safe_read(0xA5, (uint8_t*)&ft_pwr_mode, 1);
        i2c_safe_read(0x80, (uint8_t*)&ft_threshold, 1);
    }
    
    uint8_t raw[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    if (!i2c_safe_read(0x00, raw, 4)) return;
    
    ft_raw[0] = raw[0];
    ft_raw[1] = raw[1];
    ft_raw[2] = raw[2];
    ft_raw[3] = raw[3];
    
    uint8_t touch_points = raw[2] & 0x0F;
    if (touch_points > 0 && touch_points < 0x0F) {
        uint8_t xy[4] = {0};
        if (i2c_safe_read(0x03, xy, 4)) {
            touch_buf_x = ((xy[0] & 0x0F) << 8) | xy[1];
            touch_buf_y = ((xy[2] & 0x0F) << 8) | xy[3];
            touch_buf_pressed = true;
            touch_read_ok_count++;
            if (touch_indev) {
                lv_indev_enable(touch_indev, true);
            }
        }
    } else {
        touch_buf_pressed = false;
    }

    infinitime_detect_swipe(touch_buf_x, touch_buf_y, touch_buf_pressed);
}

void lvgl_tick_handler(uint32_t tick)
{
    lv_tick_inc(tick);
}

/* 外部变量，从 infinitime_adapter.c 获取帧计数 */
extern uint32_t get_ui_frame_count(void);
extern uint32_t get_max_handler_time(void);

/* LVGL异步任务：更新调试信息 */
static void debug_update_task(lv_task_t * task)
{
    if (!debug_enabled || !dbg_label) return;
    
    static char buf[128];
    uint32_t current_tick = HAL_GetTick();
    uint32_t systick = get_systick_count();
    uint32_t main_loop = get_main_loop_count();
    uint32_t dwt_cnt = lvgl_get_dwt_counter();
    uint32_t icsr = SCB->ICSR;
    uint32_t shpr3 = SCB->SHP[2];
    uint32_t hclk = HAL_RCC_GetHCLKFreq();
    uint32_t basepri = __get_BASEPRI();
    
    snprintf(buf, sizeof(buf), "T:%lu SY:%lu ML:%lu\nDW:%lu ICSR:0x%lX\nSHPR3:0x%lX HCLK:%lu BP:%lu", 
             (unsigned long)current_tick,
             (unsigned long)systick,
             (unsigned long)main_loop,
             (unsigned long)dwt_cnt,
             (unsigned long)icsr,
             (unsigned long)shpr3,
             (unsigned long)hclk,
             (unsigned long)basepri);
    
    lv_label_set_text(dbg_label, buf);
}

/* 创建调试更新任务 */
static lv_task_t *debug_task = NULL;

void lvgl_debug_draw(void)
{
    /* 创建异步任务（只创建一次） */
    if (!debug_task && dbg_label) {
        debug_task = lv_task_create(debug_update_task, 100, LV_TASK_PRIO_LOW, NULL);
    }
}

void lvgl_debug_set_enabled(bool en)
{
    debug_enabled = en;
}