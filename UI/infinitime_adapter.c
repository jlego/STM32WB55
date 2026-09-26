#include "infinitime_adapter.h"
#include "lvgl_stm32.h"
#include "st7789.h"
#include "ft3168_touch.h"
#include "lvgl/lvgl.h"

/* 全局触摸状态 */
static it_touch_info_t current_touch = {0};

/* 初始化InfiniTime UI系统 */
void infinitime_ui_init(void)
{
    /* 初始化LVGL */
    lvgl_init();
    
    /* 创建主屏幕 */
    lv_obj_t *scr = lv_scr_act();
    
    /* 创建状态栏 */
    lv_obj_t *status_bar = lv_obj_create(scr, NULL);
    lv_obj_set_size(status_bar, LV_HOR_RES_MAX, 30);
    lv_obj_align(status_bar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_style_local_bg_color(status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x20, 0x20, 0x20));
    
    /* 时间标签 */
    lv_obj_t *time_label = lv_label_create(status_bar, NULL);
    lv_label_set_text(time_label, "12:00");
    lv_obj_align(time_label, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);
    lv_obj_set_style_local_text_color(time_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 电池图标占位 */
    lv_obj_t *battery_label = lv_label_create(status_bar, NULL);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(battery_label, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_obj_set_style_local_text_color(battery_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 主内容区域 */
    lv_obj_t *content = lv_obj_create(scr, status_bar);
    lv_obj_set_size(content, LV_HOR_RES_MAX, LV_VER_RES_MAX - 60);
    lv_obj_align(content, NULL, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_local_bg_color(content, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    
    /* 欢迎文字 */
    lv_obj_t *welcome = lv_label_create(content, NULL);
    lv_label_set_text(welcome, "InfiniTime UI\nSTM32WB55");
    lv_obj_align(welcome, NULL, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_local_text_color(welcome, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(welcome, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    
    /* 底部导航栏 */
    lv_obj_t *nav_bar = lv_obj_create(scr, status_bar);
    lv_obj_set_size(nav_bar, LV_HOR_RES_MAX, 30);
    lv_obj_align(nav_bar, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_local_bg_color(nav_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x20, 0x20, 0x20));
    
    /* 导航按钮 */
    lv_obj_t *btn_home = lv_btn_create(nav_bar, NULL);
    lv_obj_set_size(btn_home, 40, 24);
    lv_obj_align(btn_home, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);
    lv_obj_t *btn_home_label = lv_label_create(btn_home, NULL);
    lv_label_set_text(btn_home_label, LV_SYMBOL_HOME);
    
    lv_obj_t *btn_back = lv_btn_create(nav_bar, btn_home);
    lv_obj_align(btn_back, NULL, LV_ALIGN_IN_LEFT_MID, 70, 0);
    lv_obj_t *btn_back_label = lv_label_create(btn_back, NULL);
    lv_label_set_text(btn_back_label, LV_SYMBOL_LEFT);
    
    lv_obj_t *btn_menu = lv_btn_create(nav_bar, btn_home);
    lv_obj_align(btn_menu, NULL, LV_ALIGN_IN_RIGHT_MID, -20, 0);
    lv_obj_t *btn_menu_label = lv_label_create(btn_menu, NULL);
    lv_label_set_text(btn_menu_label, LV_SYMBOL_LIST);
}

/* 处理UI任务 */
void infinitime_ui_task(void)
{
    lv_task_handler();
}

/* 获取触摸信息 */
it_touch_info_t infinitime_get_touch_info(void)
{
    return current_touch;
}

/* 设置当前显示的屏幕 */
void infinitime_set_screen(const char* screen_name)
{
    /* TODO: 实现屏幕切换逻辑 */
    (void)screen_name;
}

/* 更新屏幕内容 */
void infinitime_update_screen(void)
{
    lv_task_handler();
}