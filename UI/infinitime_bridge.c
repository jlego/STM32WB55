#include "infinitime_bridge.h"
#include <lvgl/lvgl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* InfiniTime 主题颜色 */
#define COLOR_BG          LV_COLOR_MAKE(0x5d, 0x69, 0x7e)
#define COLOR_BG_ALT      LV_COLOR_MAKE(0x38, 0x38, 0x38)
#define COLOR_BG_DARK     LV_COLOR_MAKE(0x18, 0x18, 0x18)
#define COLOR_HIGHLIGHT   LV_COLOR_MAKE(0x0, 0xb0, 0x0)

/* 外部回调函数 - 在 infinitime_adapter.c 中定义 */
extern void app_tile_cb(lv_obj_t *btnm, lv_event_t event);

lv_obj_t* infinitime_create_tile_screen(lv_obj_t *parent, int page_num, int total_pages, const char **icons, int num_icons) {
    /* 调试打印 */
    printf("Tile screen page %d, icons: %d\n", page_num, num_icons);
    for (int i = 0; i < num_icons; i++) {
        printf("  Icon %d: [%02x %02x %02x]\n", i, 
               (unsigned char)icons[i][0], 
               (unsigned char)icons[i][1], 
               (unsigned char)icons[i][2]);
    }
    
    /* 创建页面容器 - 不直接清理 parent */
    lv_obj_t *page = lv_obj_create(parent, NULL);
    lv_obj_set_size(page, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_local_bg_color(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_all(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    
    /* 调试打印：页面容器创建成功 */
    printf("  Page created: %p\n", (void*)page);
    
    /* 顶部时间显示（占位） */
    lv_obj_t *label_time = lv_label_create(page, NULL);
    lv_label_set_text(label_time, "00:00");
    lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label_time, page, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    
    int btn_width = 80;
    int btn_height = 80;
    int gap_x = 20;
    int gap_y = 20;
    int total_width = 2 * btn_width + 1 * gap_x;
    int start_x = (LV_HOR_RES_MAX - total_width) / 2;
    int start_y = 60;
    
    printf("  Button layout: start_x=%d, start_y=%d, btn_w=%d, btn_h=%d\n", 
           start_x, start_y, btn_width, btn_height);
    
    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        int x = start_x + col * (btn_width + gap_x);
        int y = start_y + row * (btn_height + gap_y);
        
        if (i < num_icons && icons[i] != NULL) {
            /* 创建按钮容器 */
            lv_obj_t *btn = lv_btn_create(page, NULL);
            lv_obj_set_size(btn, btn_width, btn_height);
            lv_obj_set_pos(btn, x, y);
            lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 15);
            lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, COLOR_BG_ALT);
            lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
            lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, COLOR_BG);
            lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_OPA_COVER);
            lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
            
            printf("  Button %d: pos(%d,%d) icon=[%02x%02x%02x]\n", i, x, y,
                   (unsigned char)icons[i][0],
                   (unsigned char)icons[i][1],
                   (unsigned char)icons[i][2]);
            
            /* 创建图标标签 */
            lv_obj_t *label = lv_label_create(btn, NULL);
            lv_label_set_text(label, icons[i]);
            lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
            lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_icons);
            lv_obj_align(label, btn, LV_ALIGN_CENTER, 0, 0);
            
            /* 设置按钮点击事件 */
            lv_obj_set_user_data(btn, (void*)(intptr_t)i);
            lv_obj_set_event_cb(btn, app_tile_cb);
        }
    }
    
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
    
    /* 返回页面容器，而不是按钮矩阵 */
    return page;
}