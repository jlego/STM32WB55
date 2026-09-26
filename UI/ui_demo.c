#include "ui_demo.h"
#include "lvgl_stm32.h"
#include "st7789.h"

static lv_obj_t *label;
static int counter = 0;

void ui_demo_init(void)
{
    /* 初始化LVGL */
    lvgl_init();
    
    /* 创建一个简单的界面 */
    lv_obj_t *scr = lv_scr_act();
    
    /* 创建标题标签 */
    label = lv_label_create(scr, NULL);
    lv_label_set_text(label, "STM32WB55 UI");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -20);
    
    /* 创建计数器标签 */
    lv_obj_t *counter_label = lv_label_create(scr, NULL);
    lv_label_set_text(counter_label, "Counter: 0");
    lv_obj_align(counter_label, NULL, LV_ALIGN_CENTER, 0, 20);
    
    /* 创建按钮 */
    lv_obj_t *btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(btn, 120, 40);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);
    
    lv_obj_t *btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Click Me");
}

void ui_demo_run(void)
{
    lv_task_handler();
}