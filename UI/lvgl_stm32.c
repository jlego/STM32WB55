#include "lvgl_stm32.h"
#include "st7789.h"
#include "ft3168_touch.h"
#include "stm32wbxx_hal.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

static lv_disp_buf_t disp_buf;
static lv_color_t buf1[LV_HOR_RES_MAX * 100];
static lv_color_t buf2[LV_HOR_RES_MAX * 50];

/* 显示刷新回调函数 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    uint16_t width = area->x2 - area->x1 + 1;
    uint16_t height = area->y2 - area->y1 + 1;
    uint16_t *buf = (uint16_t *)color_p;
    
    /* 设置显示窗口 */
    ST7789_SetAddressWindow(area->x1, area->y1, width, height);
    
    /* 开始写入数据 */
    lcd_dc_set();
    lcd_cs_clr();
    
    /* 发送像素数据 */
    for (int i = 0; i < width * height; i++) {
        uint16_t color = buf[i];
        /* 交换字节顺序，因为LVGL使用RGB565但字节序可能不同 */
        uint8_t buf_spi[2] = { (uint8_t)((color >> 8) & 0xFF), (uint8_t)(color & 0xFF) };
        HAL_SPI_Transmit(&hspi1, buf_spi, 2, HAL_MAX_DELAY);
    }
    
    lcd_cs_set();
    
    /* 通知LVGL刷新完成 */
    lv_disp_flush_ready(disp_drv);
}

/* 触摸读取回调函数 */
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    ft3168_touch_info_t touch = ft3168_touch_get_info();
    
    if (touch.is_touching && touch.points[0].is_valid) {
        data->point.x = touch.points[0].x;
        data->point.y = touch.points[0].y;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void lvgl_init(void)
{
    /* 初始化LVGL库 */
    lv_init();
    
    /* 初始化显示缓冲区 */
    lv_disp_buf_init(&disp_buf, buf1, buf2, LV_HOR_RES_MAX * 10);
    
    /* 注册显示驱动 */
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    
    /* 注册触摸驱动 */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void lvgl_tick_handler(uint32_t tick)
{
    lv_tick_inc(tick);
}

void lvgl_flush_display(void)
{
    lv_task_handler();
}

void lvgl_touch_handler(void)
{
    /* 触摸处理在lv_task_handler中自动调用 */
}