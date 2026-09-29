#ifndef INFINITIME_BRIDGE_H
#define INFINITIME_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

/* 应用图标点击回调 - 在 infinitime_adapter.c 中定义 */
void app_tile_cb(lv_obj_t *btnm, lv_event_t event);

/* InfiniTime 风格的 UI 创建函数 */
/* 返回 btnmatrix 对象，用于事件回调 */
lv_obj_t* infinitime_create_tile_screen(lv_obj_t *parent, int page_num, int total_pages, const char **icons, int num_icons);

#ifdef __cplusplus
}
#endif

#endif /* INFINITIME_BRIDGE_H */