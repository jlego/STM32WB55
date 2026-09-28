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

/* 屏幕类型 - 对应InfiniTime的Apps枚举 */
typedef enum {
    IT_SCREEN_CLOCK = 0,
    IT_SCREEN_LAUNCHER,
    IT_SCREEN_NOTIFICATIONS,
    IT_SCREEN_QUICK_SETTINGS,
    IT_SCREEN_SETTINGS,
    IT_SCREEN_STOPWATCH,
    IT_SCREEN_TIMER,
    IT_SCREEN_MUSIC,
    IT_SCREEN_WEATHER,
    IT_SCREEN_BATTERY_INFO,
    IT_SCREEN_SYSTEM_INFO,
    IT_SCREEN_FLASHLIGHT,
    IT_SCREEN_METRONOME,
    IT_SCREEN_PADDLE,
    IT_SCREEN_DICE,
    IT_SCREEN_CALCULATOR,
    IT_SCREEN_NAVIGATION,
    IT_SCREEN_COUNT
} it_screen_t;

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
void infinitime_set_screen(it_screen_t screen);

/* 获取当前屏幕 */
it_screen_t infinitime_get_current_screen(void);

/* 更新屏幕内容 */
void infinitime_update_screen(void);

/* 模拟时间更新 */
void infinitime_set_time(uint8_t hour, uint8_t minute, uint8_t second);

/* 模拟电池电量更新 (0-100) */
void infinitime_set_battery(uint8_t percent);

/* 模拟步数更新 */
void infinitime_set_steps(uint32_t steps);

/* 模拟心率更新 */
void infinitime_set_heart_rate(uint8_t bpm);

/* 获取调试信息 */
uint32_t get_main_loop_count(void);
uint32_t get_debug_stage(void);

/* 调试阶段变量 - 供lvgl_stm32.c直接访问 */
extern volatile uint32_t g_debug_stage;

#ifdef __cplusplus
}
#endif

#endif /* INFINITIME_ADAPTER_H */