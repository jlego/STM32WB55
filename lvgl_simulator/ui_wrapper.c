#include "lvgl/lvgl.h"
#include "infinitime_adapter.h"
#include <stdio.h>

/* 模拟器 UI 初始化入口 */
void infinitime_ui_init(void)
{
    printf("Initializing InfiniTime UI...\n");
    
    /* 默认显示时钟表盘 */
    infinitime_set_screen(IT_SCREEN_CLOCK);
    
    printf("UI initialized. Default screen: Clock\n");
}