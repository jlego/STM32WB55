#include "infinitime_adapter.h"
#include "infinitime_bridge.h"
#ifndef LVGL_SIMULATOR
#include "lvgl_stm32.h"
#include "st7789.h"
#include "ft3168_touch.h"
#endif
#include "lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========== InfiniTime 主题颜色 (对应 Colors.h) ========== */
#define IT_COLOR_DEEP_ORANGE   LV_COLOR_MAKE(0xff, 0x40, 0x0)
#define IT_COLOR_ORANGE        LV_COLOR_MAKE(0xff, 0xb0, 0x0)
#define IT_COLOR_GREEN         LV_COLOR_MAKE(0x0, 0xb0, 0x0)
#define IT_COLOR_BLUE          LV_COLOR_MAKE(0x0, 0x50, 0xff)
#define IT_COLOR_LIGHT_GRAY    LV_COLOR_MAKE(0xb0, 0xb0, 0xb0)
#define IT_COLOR_GRAY          LV_COLOR_MAKE(0x50, 0x50, 0x50)
#define IT_COLOR_BG            LV_COLOR_MAKE(0x5d, 0x69, 0x7e)
#define IT_COLOR_BG_ALT        LV_COLOR_MAKE(0x38, 0x38, 0x38)
#define IT_COLOR_BG_DARK       LV_COLOR_MAKE(0x18, 0x18, 0x18)
#define IT_COLOR_HIGHLIGHT     IT_COLOR_GREEN
#define IT_COLOR_HEART_RED     LV_COLOR_MAKE(0xCE, 0x1B, 0x1B)
#define IT_COLOR_STEP_CYAN     LV_COLOR_MAKE(0x00, 0xFF, 0xE7)

/* ========== InfiniTime符号定义 (对应Symbols.h) ========== */
#define SYM_BATTERY_HALF       "\xEF\x89\x82"
#define SYM_HEART_BEAT         "\xEF\x88\x9E"
#define SYM_BLUETOOTH          "\xEF\x8A\x94"
#define SYM_SHOE               "\xEF\x95\x8B"
#define SYM_CLOCK              "\xEF\x80\x97"
#define SYM_BELL               "\xEF\x83\xB3"
#define SYM_INFO               "\xEF\x84\xA9"
#define SYM_LIST               "\xEF\x80\xBA"
#define SYM_SUN                "\xEF\x86\x85"
#define SYM_MUSIC              "\xEF\x80\x81"
#define SYM_STOPWATCH          "\xEF\x8B\xB2"
#define SYM_PAINTBRUSH         "\xEF\x87\xBC"
#define SYM_PADDLE             "\xEF\x91\x9D"
#define SYM_DICE               "\xEF\x94\xA2"
#define SYM_CALCULATOR         "\xEF\x87\xAC"
#define SYM_HOME               "\xEF\x80\x95"
#define SYM_SLEEP              "\xEE\xBD\x84"
#define SYM_SETTINGS           "\xEE\xA2\xB8"
#define SYM_BRIGHTNESS_LOW     "\xEE\x8E\xAA"
#define SYM_BRIGHTNESS_MEDIUM  "\xEE\x8E\xAB"
#define SYM_BRIGHTNESS_HIGH    "\xEE\x8E\xAC"
#define SYM_FLASHLIGHT         "\xEF\x80\x8B"
#define SYM_PHONE              "\xEF\x82\x95"
#define SYM_WEATHER            "\xEF\x83\x82"
#define SYM_METRONOME          "\xEF\x95\xA9"
#define SYM_NAVIGATION         "\xEF\x96\xa0"
#define SYM_PLAY               "\xEF\x81\x8B"
#define SYM_STEP_FORWARD       "\xEF\x81\x91"
#define SYM_STEP_BACKWARD      "\xEF\x81\x88"
#define SYM_PAUSE              "\xEF\x81\x8C"
#define SYM_LAPS_FLAG          "\xEF\x80\xA4"
#define SYM_EYE                "\xEF\x81\xAE"

/* ========== 全局状态 ========== */
static it_screen_t current_screen = IT_SCREEN_CLOCK;
static it_touch_info_t current_touch = {0};
static uint8_t sim_hour = 12, sim_minute = 0, sim_second = 0;
static uint8_t sim_battery = 80;
static uint32_t sim_steps = 0;
static uint8_t sim_heart_rate = 0;

/* ========== 时钟表盘UI对象 ========== */
static struct {
    lv_obj_t *label_time;
    lv_obj_t *label_date;
    lv_obj_t *label_heart;
    lv_obj_t *label_heart_val;
    lv_obj_t *label_steps;
    lv_obj_t *label_step_icon;
    lv_obj_t *label_bell;
    lv_obj_t *label_weather;
    lv_obj_t *label_temp;
    lv_task_t *task_refresh;
} clock_ui = {0};

/* ========== 应用列表(Launcher)UI对象 ========== */
static struct {
    lv_obj_t *tileview;
    lv_obj_t *btnm[2];  /* 两页 btnmatrix */
    int current_page;
} launcher_ui = {0};

/* ========== 快捷设置UI对象 ========== */
static struct {
    lv_obj_t *container;
    lv_obj_t *label_brightness;
    lv_obj_t *label_bluetooth;
    lv_obj_t *label_dnd;
    lv_obj_t *btn_brightness;
    lv_obj_t *btn_bluetooth;
    lv_obj_t *btn_dnd;
} quick_settings_ui = {0};

/* ========== 通知UI对象 ========== */
static struct {
    lv_obj_t *container;
    lv_obj_t *label_title;
} notifications_ui = {0};

/* ========== 秒表UI对象 ========== */
static struct {
    lv_obj_t *label_time;
    lv_obj_t *label_msec;
    lv_obj_t *label_lap;
    lv_obj_t *label_play;
    lv_obj_t *btn_start;
    lv_obj_t *btn_lap;
    lv_obj_t *btn_reset;
    uint32_t elapsed_ms;
    bool running;
} stopwatch_ui = {0};

/* ========== 设置变量 ========== */
static bool is_dimmed = false;
static uint32_t last_activity_time = 0;

/* ========== 前向声明 ========== */
static void switch_screen(it_screen_t screen);
static void create_clock_screen(void);
static void create_launcher_screen(void);
static void create_quick_settings_screen(void);
static void create_notifications_screen(void);
static void create_stopwatch_screen(void);
static void create_placeholder_screen(it_screen_t screen);
static void clock_refresh(lv_task_t *task);

/* ========== 滑动手势检测 ========== */
static lv_coord_t swipe_start_x = 0;
static lv_coord_t swipe_start_y = 0;
static bool swipe_tracking = false;
static bool swipe_consumed = false;
#define SWIPE_THRESHOLD 40

void infinitime_detect_swipe(lv_coord_t x, lv_coord_t y, bool pressed)
{
    if (pressed && !swipe_tracking) {
        swipe_start_x = x;
        swipe_start_y = y;
        swipe_tracking = true;
        swipe_consumed = false;
        return;
    }

    if (pressed && swipe_tracking && !swipe_consumed) {
        lv_coord_t dx = x - swipe_start_x;
        lv_coord_t dy = y - swipe_start_y;

        if (dx > SWIPE_THRESHOLD && dx > abs(dy)) {
            swipe_consumed = true;
            if (current_screen == IT_SCREEN_CLOCK) {
                switch_screen(IT_SCREEN_NOTIFICATIONS);
            }
        } else if (dx < -SWIPE_THRESHOLD && abs(dx) > abs(dy)) {
            swipe_consumed = true;
            if (current_screen != IT_SCREEN_CLOCK) {
                switch_screen(IT_SCREEN_CLOCK);
            }
        } else if (dy > SWIPE_THRESHOLD && dy > abs(dx)) {
            swipe_consumed = true;
            if (current_screen == IT_SCREEN_CLOCK) {
                switch_screen(IT_SCREEN_QUICK_SETTINGS);
            }
        } else if (dy < -SWIPE_THRESHOLD && abs(dy) > abs(dx)) {
            swipe_consumed = true;
            if (current_screen == IT_SCREEN_CLOCK) {
                switch_screen(IT_SCREEN_LAUNCHER);
            }
        }
    }

    if (!pressed && swipe_tracking) {
        swipe_tracking = false;
        swipe_consumed = false;
    }
}

/* ========== LVGL回调函数 ========== */
static void clock_refresh(lv_task_t *task) {
    (void)task;
    
    if (current_screen != IT_SCREEN_CLOCK) return;
    if (!clock_ui.label_time || !clock_ui.label_date || !clock_ui.label_steps) return;
    
    /* 更新时间 */
    char time_buf[16];
    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", sim_hour, sim_minute);
    lv_label_set_text(clock_ui.label_time, time_buf);
    
    /* 更新日期 */
    lv_label_set_text(clock_ui.label_date, "Sun 1 Jan 2025");
    
    /* 更新心率 */
    if (sim_heart_rate > 0) {
        char hr_buf[8];
        snprintf(hr_buf, sizeof(hr_buf), "%d", sim_heart_rate);
        lv_label_set_text(clock_ui.label_heart_val, hr_buf);
        lv_obj_set_style_local_text_color(clock_ui.label_heart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 
                                          lv_color_hex(0xCE1B1B));
    } else {
        lv_label_set_text(clock_ui.label_heart_val, "");
        lv_obj_set_style_local_text_color(clock_ui.label_heart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 
                                          lv_color_hex(0x1B1B1B));
    }
    
    /* 更新步数 */
    char steps_buf[16];
    snprintf(steps_buf, sizeof(steps_buf), "%lu", (unsigned long)sim_steps);
    lv_label_set_text(clock_ui.label_steps, steps_buf);
    
    /* 更新通知图标 */
    lv_label_set_text(clock_ui.label_bell, "");
}

/* 秒表回调 */
static void stopwatch_btn_cb(lv_obj_t *btn, lv_event_t event) {
    if (event != LV_EVENT_CLICKED) return;
    
    if (btn == stopwatch_ui.btn_start) {
        stopwatch_ui.running = !stopwatch_ui.running;
        if (stopwatch_ui.running) {
            lv_label_set_text(stopwatch_ui.label_play, SYM_PAUSE);
            lv_obj_set_state(stopwatch_ui.btn_lap, LV_STATE_DEFAULT);
        } else {
            lv_label_set_text(stopwatch_ui.label_play, SYM_PLAY);
            lv_obj_set_state(stopwatch_ui.btn_lap, LV_STATE_DISABLED);
        }
    } else if (btn == stopwatch_ui.btn_lap) {
        if (stopwatch_ui.running) {
            /* 计次 */
            uint32_t ms = stopwatch_ui.elapsed_ms % 1000;
            uint32_t s = (stopwatch_ui.elapsed_ms / 1000) % 60;
            uint32_t m = stopwatch_ui.elapsed_ms / 60000;
            char lap_buf[32];
            snprintf(lap_buf, sizeof(lap_buf), "Lap: %02lu:%02lu.%02lu", (unsigned long)m, (unsigned long)s, (unsigned long)(ms / 10));
            lv_label_set_text(stopwatch_ui.label_lap, lap_buf);
        } else {
            /* 停止并重置 */
            stopwatch_ui.elapsed_ms = 0;
            lv_label_set_text(stopwatch_ui.label_time, "00:00");
            lv_label_set_text(stopwatch_ui.label_msec, "00");
            lv_label_set_text(stopwatch_ui.label_lap, "");
            lv_label_set_text(stopwatch_ui.label_play, SYM_PLAY);
        }
    }
}

static void stopwatch_task_cb(lv_task_t *task) {
    (void)task;
    if (current_screen != IT_SCREEN_STOPWATCH) return;
    if (!stopwatch_ui.running) return;
    
    stopwatch_ui.elapsed_ms += 10;
    uint32_t ms = stopwatch_ui.elapsed_ms % 1000;
    uint32_t s = (stopwatch_ui.elapsed_ms / 1000) % 60;
    uint32_t m = stopwatch_ui.elapsed_ms / 60000;
    
    /* 更新主时间显示 */
    char time_buf[16];
    snprintf(time_buf, sizeof(time_buf), "%02lu:%02lu", (unsigned long)m, (unsigned long)s);
    lv_label_set_text(stopwatch_ui.label_time, time_buf);
    
    /* 更新百分秒 */
    char msec_buf[8];
    snprintf(msec_buf, sizeof(msec_buf), "%02lu", (unsigned long)(ms / 10));
    lv_label_set_text(stopwatch_ui.label_msec, msec_buf);
}

/* 应用图标点击回调 (btnmatrix 事件) */
void app_tile_cb(lv_obj_t *btnm, lv_event_t event) {
    if (event != LV_EVENT_VALUE_CHANGED) return;
    
    uint32_t btn_id = lv_btnmatrix_get_active_btn(btnm);
    
    /* 根据当前页和按钮 ID 确定应用 */
    int app_index = -1;
    if (launcher_ui.current_page == 0) {
        app_index = btn_id;  /* 0-5 */
    } else if (launcher_ui.current_page == 1) {
        app_index = btn_id + 6;  /* 6-11 */
    }
    
    if (app_index < 0 || app_index >= 12) return;
    
    switch (app_index) {
        case 0: switch_screen(IT_SCREEN_NOTIFICATIONS); break;
        case 1: switch_screen(IT_SCREEN_STOPWATCH); break;
        case 2: switch_screen(IT_SCREEN_TIMER); break;
        case 3: switch_screen(IT_SCREEN_MUSIC); break;
        case 4: switch_screen(IT_SCREEN_NAVIGATION); break;
        case 5: switch_screen(IT_SCREEN_METRONOME); break;
        case 6: switch_screen(IT_SCREEN_WEATHER); break;
        case 7: switch_screen(IT_SCREEN_BATTERY_INFO); break;
        case 8: switch_screen(IT_SCREEN_SYSTEM_INFO); break;
        case 9: switch_screen(IT_SCREEN_FLASHLIGHT); break;
        case 10: switch_screen(IT_SCREEN_PADDLE); break;
        case 11: switch_screen(IT_SCREEN_DICE); break;
        default: break;
    }
}

/* 快捷设置按钮回调 */
static void qs_btn_cb(lv_obj_t *btn, lv_event_t event) {
    if (event != LV_EVENT_CLICKED) return;
    
    if (btn == quick_settings_ui.btn_brightness) {
        static int level = 0;
        level = (level + 1) % 3;
        const char *syms[] = {SYM_BRIGHTNESS_LOW, SYM_BRIGHTNESS_MEDIUM, SYM_BRIGHTNESS_HIGH};
#ifndef LVGL_SIMULATOR
        uint8_t vals[] = {10, 50, 100};
#endif
        lv_label_set_text(quick_settings_ui.label_brightness, syms[level]);
#ifndef LVGL_SIMULATOR
        ST7789_SetBacklight(vals[level]);
#endif
    } else if (btn == quick_settings_ui.btn_bluetooth) {
        static bool bt_on = true;
        bt_on = !bt_on;
        lv_label_set_text(quick_settings_ui.label_bluetooth, bt_on ? SYM_BLUETOOTH : "");
    } else if (btn == quick_settings_ui.btn_dnd) {
        static bool dnd_on = false;
        dnd_on = !dnd_on;
        lv_label_set_text(quick_settings_ui.label_dnd, dnd_on ? SYM_BELL : "");
    }
}

/* ========== 创建时钟表盘 (WatchFaceDigital) - 完全参考 InfiniTime ========== */
static void create_clock_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 通知图标 - 左上角 (对应 InfiniTime notificationIcon) */
    clock_ui.label_bell = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_bell, "");
    lv_obj_set_style_local_text_color(clock_ui.label_bell, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
    lv_obj_align(clock_ui.label_bell, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    
    /* 时间 - 右侧中央 (对应 InfiniTime label_time, 使用 jetbrains_mono_extrabold_compressed) */
    clock_ui.label_time = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_time, "12:00");
    lv_obj_set_style_local_text_font(clock_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
    lv_obj_set_style_local_text_color(clock_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(clock_ui.label_time, scr, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    
    /* 日期 - 中央偏下 (对应 InfiniTime label_date) */
    clock_ui.label_date = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_date, "Sun 1 Jan 2025");
    lv_obj_align(clock_ui.label_date, scr, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_local_text_color(clock_ui.label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_LIGHT_GRAY);
    
    /* 心率图标 - 左下角 (对应 InfiniTime heartbeatIcon) */
    clock_ui.label_heart = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_heart, SYM_HEART_BEAT);
    lv_obj_set_style_local_text_color(clock_ui.label_heart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_HEART_RED);
    lv_obj_align(clock_ui.label_heart, scr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    
    /* 心率值 (对应 InfiniTime heartbeatValue) */
    clock_ui.label_heart_val = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_heart_val, "");
    lv_obj_set_style_local_text_color(clock_ui.label_heart_val, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_HEART_RED);
    lv_obj_align(clock_ui.label_heart_val, clock_ui.label_heart, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    
    /* 步数值 - 右下角 (对应 InfiniTime stepValue) */
    clock_ui.label_steps = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_steps, "0");
    lv_obj_set_style_local_text_color(clock_ui.label_steps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_STEP_CYAN);
    lv_obj_align(clock_ui.label_steps, scr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    
    /* 步数图标 (对应 InfiniTime stepIcon) */
    clock_ui.label_step_icon = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_step_icon, SYM_SHOE);
    lv_obj_set_style_local_text_color(clock_ui.label_step_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_STEP_CYAN);
    lv_obj_align(clock_ui.label_step_icon, clock_ui.label_steps, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    
    /* 天气图标和温度 (占位，对应 InfiniTime weatherIcon 和 temperature) */
    clock_ui.label_weather = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_weather, "");
    lv_obj_set_style_local_text_color(clock_ui.label_weather, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    lv_obj_align(clock_ui.label_weather, scr, LV_ALIGN_IN_TOP_MID, -20, 50);
    
    clock_ui.label_temp = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_temp, "");
    lv_obj_set_style_local_text_color(clock_ui.label_temp, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    lv_obj_align(clock_ui.label_temp, scr, LV_ALIGN_IN_TOP_MID, 20, 50);
    
    /* 创建定时刷新任务 */
    clock_ui.task_refresh = lv_task_create(clock_refresh, 1000, LV_TASK_PRIO_MID, NULL);
}

/* ========== 创建应用列表 (Launcher/Tile) - InfiniTime 风格 ========== */
static void create_launcher_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 应用图标 - FontAwesome 符号 */
    const char *icons_page1[] = {
        SYM_BELL,          /* Notifications */
        SYM_STOPWATCH,     /* Stopwatch */
        SYM_CLOCK,         /* Timer */
        SYM_MUSIC,         /* Music */
        SYM_NAVIGATION,    /* Navigation */
        SYM_METRONOME,     /* Metronome */
    };
    
    const char *icons_page2[] = {
        SYM_WEATHER,       /* Weather */
        SYM_BATTERY_HALF,  /* Battery Info */
        SYM_INFO,          /* System Info */
        SYM_FLASHLIGHT,    /* Flashlight */
        SYM_PADDLE,        /* Paddle */
        SYM_DICE,          /* Dice */
    };
    
    /* 创建两页 Tile 屏幕 */
    launcher_ui.btnm[0] = infinitime_create_tile_screen(scr, 0, 2, icons_page1, 6);
    launcher_ui.btnm[1] = infinitime_create_tile_screen(scr, 1, 2, icons_page2, 6);
    
    /* 默认显示第一页 */
    launcher_ui.current_page = 0;
    lv_obj_set_hidden(launcher_ui.btnm[1], true);
}

/* ========== 创建快捷设置 (QuickSettings) ========== */
static void create_quick_settings_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 标题 */
    lv_obj_t *title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Quick Settings");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    
    /* 亮度按钮 */
    quick_settings_ui.btn_brightness = lv_btn_create(scr, NULL);
    lv_obj_set_size(quick_settings_ui.btn_brightness, 80, 80);
    lv_obj_align(quick_settings_ui.btn_brightness, NULL, LV_ALIGN_CENTER, -70, -40);
    lv_obj_set_style_local_bg_color(quick_settings_ui.btn_brightness, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x222222));
    lv_obj_set_event_cb(quick_settings_ui.btn_brightness, qs_btn_cb);
    
    quick_settings_ui.label_brightness = lv_label_create(quick_settings_ui.btn_brightness, NULL);
    lv_label_set_text(quick_settings_ui.label_brightness, SYM_BRIGHTNESS_HIGH);
    lv_obj_set_style_local_text_color(quick_settings_ui.label_brightness, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    lv_obj_t *lbl = lv_label_create(scr, NULL);
    lv_label_set_text(lbl, "Brightness");
    lv_obj_align(lbl, quick_settings_ui.btn_brightness, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    
    /* 蓝牙按钮 */
    quick_settings_ui.btn_bluetooth = lv_btn_create(scr, quick_settings_ui.btn_brightness);
    lv_obj_align(quick_settings_ui.btn_bluetooth, NULL, LV_ALIGN_CENTER, 70, -40);
    lv_obj_set_event_cb(quick_settings_ui.btn_bluetooth, qs_btn_cb);
    
    quick_settings_ui.label_bluetooth = lv_label_create(quick_settings_ui.btn_bluetooth, NULL);
    lv_label_set_text(quick_settings_ui.label_bluetooth, SYM_BLUETOOTH);
    lv_obj_set_style_local_text_color(quick_settings_ui.label_bluetooth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4488FF));
    
    lbl = lv_label_create(scr, NULL);
    lv_label_set_text(lbl, "Bluetooth");
    lv_obj_align(lbl, quick_settings_ui.btn_bluetooth, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    
    /* 勿扰按钮 */
    quick_settings_ui.btn_dnd = lv_btn_create(scr, quick_settings_ui.btn_brightness);
    lv_obj_align(quick_settings_ui.btn_dnd, NULL, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_event_cb(quick_settings_ui.btn_dnd, qs_btn_cb);
    
    quick_settings_ui.label_dnd = lv_label_create(quick_settings_ui.btn_dnd, NULL);
    lv_label_set_text(quick_settings_ui.label_dnd, "");
    lv_obj_set_style_local_text_color(quick_settings_ui.label_dnd, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    lbl = lv_label_create(scr, NULL);
    lv_label_set_text(lbl, "DND");
    lv_obj_align(lbl, quick_settings_ui.btn_dnd, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
}

/* ========== 创建通知屏幕 ========== */
static void create_notifications_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 标题 */
    notifications_ui.label_title = lv_label_create(scr, NULL);
    lv_label_set_text(notifications_ui.label_title, "Notifications");
    lv_obj_align(notifications_ui.label_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_style_local_text_color(notifications_ui.label_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(notifications_ui.label_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    
    /* 通知列表 */
    lv_obj_t *list = lv_list_create(scr, NULL);
    lv_obj_set_size(list, LV_HOR_RES_MAX - 20, LV_VER_RES_MAX - 60);
    lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_local_bg_color(list, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(list, LV_LIST_PART_BG, LV_STATE_DEFAULT, 0);
    
    lv_list_add_btn(list, SYM_BELL, "No notifications");
}

/* ========== 创建秒表屏幕 (StopWatch) - 完全参考 InfiniTime ========== */
static void create_stopwatch_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 时间显示 - 顶部中央，使用 jetbrains_mono_76 大字体 */
    stopwatch_ui.label_time = lv_label_create(scr, NULL);
    lv_label_set_text(stopwatch_ui.label_time, "00:00");
    lv_obj_set_style_local_text_font(stopwatch_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
    lv_obj_set_style_local_text_color(stopwatch_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_width(stopwatch_ui.label_time, LV_HOR_RES_MAX);
    lv_label_set_align(stopwatch_ui.label_time, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(stopwatch_ui.label_time, scr, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    /* 百分秒 - 时间下方 */
    lv_obj_t *msec_label = lv_label_create(scr, NULL);
    stopwatch_ui.label_msec = msec_label;
    lv_label_set_text_static(msec_label, "00");
    lv_obj_set_style_local_text_color(msec_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_LIGHT_GRAY);
    lv_obj_align(msec_label, stopwatch_ui.label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, -2);
    
    /* 计次显示区域 - 底部中央上方 */
    stopwatch_ui.label_lap = lv_label_create(scr, NULL);
    lv_label_set_text_static(stopwatch_ui.label_lap, "");
    lv_obj_set_style_local_text_color(stopwatch_ui.label_lap, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_LIGHT_GRAY);
    lv_label_set_long_mode(stopwatch_ui.label_lap, LV_LABEL_LONG_BREAK);
    lv_label_set_align(stopwatch_ui.label_lap, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(stopwatch_ui.label_lap, LV_HOR_RES_MAX);
    lv_obj_align(stopwatch_ui.label_lap, scr, LV_ALIGN_IN_BOTTOM_MID, 0, -82);
    
    /* 按钮尺寸 - 参考 InfiniTime (btnWidth=115, btnHeight=80) */
    static const uint8_t btn_width = 115;
    static const uint8_t btn_height = 80;
    
    /* 开始/暂停按钮 - 右下角 */
    stopwatch_ui.btn_start = lv_btn_create(scr, NULL);
    lv_obj_set_size(stopwatch_ui.btn_start, btn_width, btn_height);
    lv_obj_align(stopwatch_ui.btn_start, scr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_local_bg_color(stopwatch_ui.btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_BG_ALT);
    lv_obj_set_style_local_bg_color(stopwatch_ui.btn_start, LV_BTN_PART_MAIN, LV_STATE_PRESSED, IT_COLOR_BG);
    lv_obj_set_style_local_radius(stopwatch_ui.btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_event_cb(stopwatch_ui.btn_start, stopwatch_btn_cb);
    
    lv_obj_t *txt_play = lv_label_create(stopwatch_ui.btn_start, NULL);
    stopwatch_ui.label_play = txt_play;
    lv_label_set_text(txt_play, SYM_PLAY);
    lv_obj_set_style_local_text_color(txt_play, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 计次/停止按钮 - 左下角 */
    stopwatch_ui.btn_lap = lv_btn_create(scr, NULL);
    lv_obj_set_size(stopwatch_ui.btn_lap, btn_width, btn_height);
    lv_obj_align(stopwatch_ui.btn_lap, scr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_local_bg_color(stopwatch_ui.btn_lap, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, IT_COLOR_BG_ALT);
    lv_obj_set_style_local_bg_color(stopwatch_ui.btn_lap, LV_BTN_PART_MAIN, LV_STATE_PRESSED, IT_COLOR_BG);
    lv_obj_set_style_local_radius(stopwatch_ui.btn_lap, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_state(stopwatch_ui.btn_lap, LV_STATE_DISABLED);  /* 初始禁用 */
    lv_obj_set_event_cb(stopwatch_ui.btn_lap, stopwatch_btn_cb);
    
    lv_obj_t *txt_lap = lv_label_create(stopwatch_ui.btn_lap, NULL);
    lv_label_set_text(txt_lap, SYM_LAPS_FLAG);
    lv_obj_set_style_local_text_color(txt_lap, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 重置按钮不需要，InfiniTime 没有重置按钮 */
    stopwatch_ui.btn_reset = NULL;
    
    stopwatch_ui.elapsed_ms = 0;
    stopwatch_ui.running = false;
    
    /* 创建秒表更新任务 */
    lv_task_create(stopwatch_task_cb, 10, LV_TASK_PRIO_MID, NULL);
}

/* ========== 屏幕切换 ========== */
static void switch_screen(it_screen_t screen) {
    /* 清理之前的任务 */
    if (clock_ui.task_refresh) {
        lv_task_del(clock_ui.task_refresh);
        clock_ui.task_refresh = NULL;
    }
    
    /* 清零 UI 结构体，防止访问已销毁的对象 */
    memset(&clock_ui, 0, sizeof(clock_ui));
    memset(&launcher_ui, 0, sizeof(launcher_ui));
    memset(&quick_settings_ui, 0, sizeof(quick_settings_ui));
    memset(&notifications_ui, 0, sizeof(notifications_ui));
    memset(&stopwatch_ui, 0, sizeof(stopwatch_ui));
    
    current_screen = screen;
#ifndef LVGL_SIMULATOR
    last_activity_time = HAL_GetTick();
#else
    last_activity_time = 0;
#endif
    
    switch (screen) {
        case IT_SCREEN_CLOCK:
            create_clock_screen();
            break;
        case IT_SCREEN_LAUNCHER:
            create_launcher_screen();
            break;
        case IT_SCREEN_QUICK_SETTINGS:
            create_quick_settings_screen();
            break;
        case IT_SCREEN_NOTIFICATIONS:
            create_notifications_screen();
            break;
        case IT_SCREEN_STOPWATCH:
            create_stopwatch_screen();
            break;
        case IT_SCREEN_TIMER:
        case IT_SCREEN_MUSIC:
        case IT_SCREEN_WEATHER:
        case IT_SCREEN_BATTERY_INFO:
        case IT_SCREEN_SYSTEM_INFO:
        case IT_SCREEN_FLASHLIGHT:
        case IT_SCREEN_METRONOME:
        case IT_SCREEN_PADDLE:
        case IT_SCREEN_DICE:
        case IT_SCREEN_CALCULATOR:
        default:
            /* 对于未实现的屏幕，显示占位 */
            create_placeholder_screen(screen);
            break;
    }
}

/* 返回按钮回调 */
static void back_btn_cb(lv_obj_t *btn, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) switch_screen(IT_SCREEN_CLOCK);
}

/* ========== 占位屏幕 ========== */
static void create_placeholder_screen(it_screen_t screen) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    const char *names[] = {
        "Clock", "Launcher", "Notifications", "Quick Settings", "Settings",
        "Stop Watch", "Timer", "Music", "Weather", "Battery",
        "System Info", "Flashlight", "Metronome", "Paddle", "Dice", "Calculator"
    };
    
    lv_obj_t *title = lv_label_create(scr, NULL);
    lv_label_set_text(title, (screen < IT_SCREEN_COUNT) ? names[screen] : "Unknown");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    
    lv_obj_t *msg = lv_label_create(scr, NULL);
    lv_label_set_text(msg, "Coming Soon");
    lv_obj_align(msg, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_text_color(msg, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    
    /* 返回按钮 */
    lv_obj_t *btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(btn, 120, 40);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333));
    lv_obj_set_event_cb(btn, back_btn_cb);
    
    lv_obj_t *lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
}

/* ========== 公共API ========== */

/* 硬件相关初始化（仅STM32） */
void infinitime_ui_init_hw(void)
{
#ifndef LVGL_SIMULATOR
    ft3168_touch_init();
    
    lvgl_init();
    
    ST7789_FillScreen(COLOR_BLACK);
    
    /* 使用ST7789_Delay，不依赖SysTick */
    ST7789_Delay(100);
#endif
}

/* UI 初始化入口（模拟器或STM32共用） */
void infinitime_ui_init(void)
{
    /* 硬件初始化 */
    infinitime_ui_init_hw();
    
    /* 创建默认屏幕 - 临时改为 Launcher 调试 */
    create_launcher_screen();
    //create_clock_screen();
    
#ifndef LVGL_SIMULATOR
    last_activity_time = HAL_GetTick();
#else
    last_activity_time = 0;
#endif
}

/* 调试用全局变量 */
static uint32_t g_ui_task_count = 0;
static uint32_t g_frame_count = 0;
static uint32_t g_max_handler_time = 0;
static volatile uint32_t g_heartbeat = 0; /* 心跳计数，用于确认系统是否存活 */
static uint32_t g_main_loop_count = 0; /* 主循环计数器 */
volatile uint32_t g_debug_stage = 0; /* 调试：记录最后执行到的阶段 */

void infinitime_ui_task(void)
{
#ifndef LVGL_SIMULATOR
    static uint32_t last_tick = 0;
    static uint32_t last_systick_check = 0;
    static uint32_t systick_stuck_count = 0;
    static uint32_t systick_recover_attempts = 0;
    
    g_ui_task_count++;
    g_main_loop_count++;
    g_frame_count++;
    g_heartbeat++; /* 心跳递增 */
    g_debug_stage = 1; /* 阶段1：进入UI任务 */
    
    /* Increment DWT counter on each UI task call - this counter increments even if SysTick stops */
    lvgl_increment_dwt_counter();
    
    /* 检测 SysTick 是否卡住并自动恢复 */
    uint32_t current_systick = get_systick_count();
    if (current_systick == last_systick_check) {
        systick_stuck_count++;
        /* 如果 SysTick 超过 50ms 没有递增，尝试重新启用 */
        if (systick_stuck_count >= 50) {
            systick_recover_attempts++;
            /* 强制重新启用 SysTick 中断 */
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
            /* 也确保计数器使能 */
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
            systick_stuck_count = 0;
        }
    } else {
        systick_stuck_count = 0;
        last_systick_check = current_systick;
    }
    
    uint32_t current_tick = HAL_GetTick();
    
    /* 调试：在 lv_task_handler 前后标记 */
    g_debug_stage = 4; /* 阶段4：准备调用lv_task_handler */
    
    lvgl_touch_process();
    
    uint32_t before_handler = HAL_GetTick();
    uint32_t next_run = lv_task_handler();
    g_debug_stage = 5; /* 阶段5：lv_task_handler完成 */
    uint32_t after_handler = HAL_GetTick();
    uint32_t handler_time = after_handler - before_handler;
    
    if (handler_time > g_max_handler_time) {
        g_max_handler_time = handler_time;
    }
    
    /* 禁用调试图层 */
    /* if (g_ui_task_count == 1) {
        lvgl_debug_set_enabled(true);
        lvgl_debug_draw();
    } */
    
    /* 不再每次都调用lvgl_debug_draw()，让LVGL自然刷新 */
    
    /* 检测屏幕超时 - 模拟InfiniTime的自动休眠 */
    // uint32_t inactive_time = lv_disp_get_inactive_time(NULL);
    // if (inactive_time > 30000 && !is_dimmed) {
    //     is_dimmed = true;
    //     ST7789_SetBacklight(5);
    // } else if (inactive_time < 30000 && is_dimmed) {
    //     is_dimmed = false;
    //     ST7789_SetBacklight(10);
    // }
    
    /* 暂时禁用触摸释放处理 - 调试用 */
    // if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET) {
    //     lvgl_touch_release_handler();
    // }
#else
    /* 模拟器：简单调用 lv_task_handler */
    g_ui_task_count++;
    g_frame_count++;
    g_heartbeat++;
    lv_task_handler();
#endif
}

uint32_t get_ui_frame_count(void)
{
    return g_frame_count;
}

uint32_t get_max_handler_time(void)
{
    return g_max_handler_time;
}

uint32_t get_heartbeat(void)
{
    return g_heartbeat;
}

uint32_t get_main_loop_count(void)
{
    return g_main_loop_count;
}

uint32_t get_debug_stage(void)
{
    return g_debug_stage;
}

void set_debug_stage(uint32_t stage)
{
    g_debug_stage = stage;
}

it_touch_info_t infinitime_get_touch_info(void)
{
    return current_touch;
}

void infinitime_set_screen(it_screen_t screen)
{
    switch_screen(screen);
}

it_screen_t infinitime_get_current_screen(void)
{
    return current_screen;
}

void infinitime_update_screen(void)
{
    lv_task_handler();
}

void infinitime_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    sim_hour = hour;
    sim_minute = minute;
    sim_second = second;
}

void infinitime_set_battery(uint8_t percent)
{
    sim_battery = percent;
}

void infinitime_set_steps(uint32_t steps)
{
    sim_steps = steps;
}

void infinitime_set_heart_rate(uint8_t bpm)
{
    sim_heart_rate = bpm;
}