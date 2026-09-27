#include "lvgl_stm32.h"
#include "st7789.h"
#include "ft3168_touch.h"
#include "stm32wbxx_hal.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;

static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 100];
static lv_color_t buf2[LV_HOR_RES_MAX * 50];

static lv_indev_drv_t touch_indev_drv;
static lv_indev_t *touch_indev = NULL;
static bool debug_enabled = true;

/* LVGL 调试图层 label（在 system layer 上，始终置顶） */
static lv_obj_t *dbg_label = NULL;
static uint32_t dbg_frame_count = 0;

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
    
    /* 批量发送到SPI */
    ST7789_SetAddressWindow(DEBUG_BG_X, DEBUG_BG_Y, DEBUG_BG_W, DEBUG_BG_H);
    lcd_dc_set();
    lcd_cs_clr();
    HAL_SPI_Transmit(&hspi1, (uint8_t *)render_buf, DEBUG_BG_W * DEBUG_BG_H * 2, 100);
    lcd_cs_set();
}

/* 显示刷新回调函数 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t width = area->x2 - area->x1 + 1;
    uint16_t height = area->y2 - area->y1 + 1;
    uint16_t *buf = (uint16_t *)color_p;
    int32_t total_pixels = width * height;
    
    if (total_pixels <= 0) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    ST7789_SetAddressWindow(area->x1, area->y1, width, height);
    
    lcd_dc_set();
    lcd_cs_clr();
    
    HAL_StatusTypeDef spi_status = HAL_SPI_Transmit(&hspi1, (uint8_t *)buf, total_pixels * 2, 50);
    
    lcd_cs_set();
    
    if (spi_status != HAL_OK) {
        static uint32_t spi_err_count = 0;
        spi_err_count++;
    }
    
    lv_disp_flush_ready(disp_drv);
}

/* 中断驱动的触摸数据缓存 */
static volatile bool touch_irq_flag = false;
static volatile lv_coord_t touch_buf_x = 0;
static volatile lv_coord_t touch_buf_y = 0;
static volatile bool touch_buf_pressed = false;

static bool touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /* 纯中断驱动：只返回缓存数据，不做任何 I2C/GPIO 操作 */
    data->point.x = touch_buf_x;
    data->point.y = touch_buf_y;
    data->state = touch_buf_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    
    return false;
}

void lvgl_init(void)
{
    lv_init();
    
    /* 只使用单缓冲区，避免双缓冲模式下的死循环问题 */
    /* 增大缓冲区到 40 行，减少刷新次数，降低 while(vdb->flushing) 阻塞风险 */
    lv_disp_buf_init(&disp_buf, buf1, NULL, LV_HOR_RES_MAX * 40);
    
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_flush;
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
    lv_obj_set_pos(dbg_label, 25, 0);
    lv_label_set_text(dbg_label, "");
}

/* 触摸中断处理函数 - 在 EXTI 回调中调用，只设标志 */
void lvgl_touch_irq_handler(void)
{
    touch_irq_flag = true;
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
    if (!touch_irq_flag) return;
    touch_irq_flag = false;
    
    ft3168_touch_info_t touch = ft3168_touch_get_info();
    
    if (touch.is_touching && touch.points[0].is_valid) {
        touch_buf_x = touch.points[0].x;
        touch_buf_y = touch.points[0].y;
        touch_buf_pressed = true;
        if (touch_indev) {
            lv_indev_enable(touch_indev, true);
        }
    } else {
        touch_buf_pressed = false;
    }
}

void lvgl_tick_handler(uint32_t tick)
{
    lv_tick_inc(tick);
}

void lvgl_debug_draw(void)
{
    if (!debug_enabled || !dbg_label) return;
    
    dbg_frame_count++;
    
    static char buf[64];
    if (touch_buf_pressed) {
        snprintf(buf, sizeof(buf), "F%lu T:%d,%d", 
                 (unsigned long)dbg_frame_count, (int)touch_buf_x, (int)touch_buf_y);
    } else {
        snprintf(buf, sizeof(buf), "F%lu", (unsigned long)dbg_frame_count);
    }
    lv_label_set_text(dbg_label, buf);
}

void lvgl_debug_set_enabled(bool en)
{
    debug_enabled = en;
}