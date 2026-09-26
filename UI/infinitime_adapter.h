#ifndef INFINITIME_ADAPTER_H
#define INFINITIME_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* 屏幕分辨率 */
#define IT_SCREEN_WIDTH  240
#define IT_SCREEN_HEIGHT 280

/* 触摸事件类型 */
typedef enum {
    IT_TOUCH_NONE = 0,
    IT_TOUCH_TAP,
    IT_TOUCH_DOUBLE_TAP,
    IT_TOUCH_LONG_PRESS,
    IT_TOUCH_SWIPE_UP,
    IT_TOUCH_SWIPE_DOWN,
    IT_TOUCH_SWIPE_LEFT,
    IT_TOUCH_SWIPE_RIGHT
} it_touch_event_t;

/* 触摸点信息 */
typedef struct {
    uint16_t x;
    uint16_t y;
    bool is_touching;
    it_touch_event_t gesture;
} it_touch_info_t;

/* 初始化InfiniTime UI系统 */
void infinitime_ui_init(void);

/* 处理UI任务（在主循环中调用） */
void infinitime_ui_task(void);

/* 获取触摸信息 */
it_touch_info_t infinitime_get_touch_info(void);

/* 设置当前显示的屏幕 */
void infinitime_set_screen(const char* screen_name);

/* 更新屏幕内容 */
void infinitime_update_screen(void);

#ifdef __cplusplus
}
#endif

#endif /* INFINITIME_ADAPTER_H */