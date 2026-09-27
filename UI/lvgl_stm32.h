#ifndef LVGL_STM32_H
#define LVGL_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

void lvgl_init(void);
void lvgl_tick_handler(uint32_t tick);
void lvgl_debug_draw(void);
void lvgl_debug_set_enabled(bool en);
void lvgl_touch_irq_handler(void);
void lvgl_touch_release_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* LVGL_STM32_H */