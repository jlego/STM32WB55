#include "infinitime_bridge.h"
#include <lvgl/lvgl.h>
#include <stdlib.h>
#include <string.h>

/* InfiniTime 主题颜色 */
#define COLOR_BG          LV_COLOR_MAKE(0x5d, 0x69, 0x7e)
#define COLOR_BG_ALT      LV_COLOR_MAKE(0x38, 0x38, 0x38)
#define COLOR_BG_DARK     LV_COLOR_MAKE(0x18, 0x18, 0x18)
#define COLOR_HIGHLIGHT   LV_COLOR_MAKE(0x0, 0xb0, 0x0)

/* 外部回调函数 - 在 infinitime_adapter.c 中定义 */
extern void app_tile_cb(lv_obj_t *btnm, lv_event_t event);

lv_obj_t* infinitime_create_tile_screen(lv_obj_t *parent, int page_num, int total_pages, const char **icons, int num_icons) {
    /* 创建页面容器 - 不直接清理 parent */
    lv_obj_t *page = lv_obj_create(parent, NULL);
    lv_obj_set_size(page, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_style_local_bg_color(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_all(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    
    /* 顶部时间显示（占位） */
    lv_obj_t *label_time = lv_label_create(page, NULL);
    lv_label_set_text(label_time, "00:00");
    lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label_time, page, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    
    /* 为 btnm_map 分配动态内存，确保每页独立且生命周期足够长 */
    const char **btnm_map = (const char **)malloc(8 * sizeof(const char *));
    int btn_index = 0;
    
    for (int i = 0; i < 6; i++) {
        if (i == 3) {
            btnm_map[btn_index++] = "\n";
        }
        if (i < num_icons && icons[i] != NULL) {
            btnm_map[btn_index++] = icons[i];
        } else {
            btnm_map[btn_index++] = " ";
        }
    }
    btnm_map[btn_index] = "";
    
    /* 创建按钮矩阵 - 参考 InfiniTime Tile.cpp */
    lv_obj_t *btnm1 = lv_btnmatrix_create(page, NULL);
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_obj_set_size(btnm1, LV_HOR_RES_MAX - 16, LV_VER_RES_MAX - 60);
    lv_obj_align(btnm1, page, LV_ALIGN_CENTER, 0, 10);
    
    /* 设置样式 - 完全参考 InfiniTime */
    lv_obj_set_style_local_radius(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, COLOR_BG_ALT);
    lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_PRESSED, COLOR_BG);
    lv_obj_set_style_local_pad_all(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_inner(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);
    
    /* 设置按钮点击触发 */
    for (int i = 0; i < 6; i++) {
        lv_btnmatrix_set_btn_ctrl(btnm1, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
    }
    
    /* 使用外部回调函数 */
    lv_obj_set_event_cb(btnm1, app_tile_cb);
    
    /* 页码指示器 - 底部居中 */
    for (int i = 0; i < total_pages; i++) {
        lv_obj_t *dot = lv_obj_create(page, NULL);
        lv_obj_set_size(dot, 8, 8);
        int total_width = total_pages * 12;
        int start_x = (LV_HOR_RES_MAX - total_width) / 2;
        lv_obj_set_pos(dot, start_x + i * 12, LV_VER_RES_MAX - 20);
        lv_obj_set_style_local_bg_color(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 
                                        i == page_num ? LV_COLOR_WHITE : LV_COLOR_GRAY);
        lv_obj_set_style_local_radius(dot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    }
    
    return btnm1;
}