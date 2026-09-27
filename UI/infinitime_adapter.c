#include "infinitime_adapter.h"
#include "lvgl_stm32.h"
#include "st7789.h"
#include "ft3168_touch.h"
#include "lvgl/lvgl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    lv_obj_t **tiles;
    lv_obj_t **labels;
    lv_obj_t **icons;
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
static void create_clock_screen(void);
static void create_launcher_screen(void);
static void create_quick_settings_screen(void);
static void create_notifications_screen(void);
static void create_stopwatch_screen(void);
static void create_placeholder_screen(it_screen_t screen);
static void clock_refresh(lv_task_t *task);
static void switch_screen(it_screen_t screen);

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
        lv_label_set_text(stopwatch_ui.btn_start, stopwatch_ui.running ? SYM_PAUSE : SYM_PLAY);
    } else if (btn == stopwatch_ui.btn_reset) {
        stopwatch_ui.elapsed_ms = 0;
        stopwatch_ui.running = false;
        lv_label_set_text(stopwatch_ui.btn_start, SYM_PLAY);
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
    
    char buf[32];
    snprintf(buf, sizeof(buf), "%02lu:%02lu.%02lu", (unsigned long)m, (unsigned long)s, (unsigned long)(ms / 10));
    lv_label_set_text(stopwatch_ui.label_time, buf);
}

/* 应用图标点击回调 */
static void app_tile_cb(lv_obj_t *btn, lv_event_t event) {
    if (event != LV_EVENT_CLICKED) return;
    
    for (int i = 0; i < 12; i++) {
        if (launcher_ui.tiles && btn == launcher_ui.tiles[i]) {
            switch (i) {
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
            return;
        }
    }
}

/* 快捷设置按钮回调 */
static void qs_btn_cb(lv_obj_t *btn, lv_event_t event) {
    if (event != LV_EVENT_CLICKED) return;
    
    if (btn == quick_settings_ui.btn_brightness) {
        static int level = 0;
        level = (level + 1) % 3;
        const char *syms[] = {SYM_BRIGHTNESS_LOW, SYM_BRIGHTNESS_MEDIUM, SYM_BRIGHTNESS_HIGH};
        uint8_t vals[] = {10, 50, 100};
        lv_label_set_text(quick_settings_ui.label_brightness, syms[level]);
        ST7789_SetBacklight(vals[level]);
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

/* ========== 创建时钟表盘 (WatchFaceDigital) ========== */
static void create_clock_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 时间 - 中央 (使用系统默认字体) */
    clock_ui.label_time = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_time, "12:00");
    lv_obj_set_style_local_text_font(clock_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_48);
    lv_obj_set_style_local_text_color(clock_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_pos(clock_ui.label_time, 60, 80);
    
    /* 日期 - 时间下方 */
    clock_ui.label_date = lv_label_create(scr, NULL);
    lv_label_set_text(clock_ui.label_date, "Sun 1 Jan 2025");
    lv_obj_set_pos(clock_ui.label_date, 50, 160);
    lv_obj_set_style_local_text_color(clock_ui.label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));
    
    /* 心率图标 - 左下 */
    clock_ui.label_heart = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_heart, SYM_HEART_BEAT);
    lv_obj_set_style_local_text_color(clock_ui.label_heart, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
    lv_obj_set_pos(clock_ui.label_heart, 10, 240);
    
    /* 心率值 */
    clock_ui.label_heart_val = lv_label_create(scr, NULL);
    lv_label_set_text_static(clock_ui.label_heart_val, "");
    lv_obj_set_style_local_text_color(clock_ui.label_heart_val, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
    lv_obj_set_pos(clock_ui.label_heart_val, 40, 240);
    
    /* 步数图标 - 右下 */
    clock_ui.label_step_icon = lv_label_create(scr, NULL);
    lv_obj_set_style_local_text_color(clock_ui.label_step_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
    lv_label_set_text_static(clock_ui.label_step_icon, SYM_SHOE);
    lv_obj_set_pos(clock_ui.label_step_icon, 140, 240);
    
    /* 步数值 */
    clock_ui.label_steps = lv_label_create(scr, NULL);
    lv_obj_set_style_local_text_color(clock_ui.label_steps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
    lv_label_set_text_static(clock_ui.label_steps, "0");
    lv_obj_set_pos(clock_ui.label_steps, 170, 240);
    
    /* 创建定时刷新任务 */
    clock_ui.task_refresh = lv_task_create(clock_refresh, 1000, LV_TASK_PRIO_MID, NULL);
}

/* ========== 创建应用列表 (Launcher/Tile) ========== */
static void create_launcher_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 标题 */
    lv_obj_t *title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Applications");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    
    /* 应用列表 */
    const char *app_names[] = {
        "Notifications", "Stop Watch", "Timer", "Music",
        "Navigation", "Metronome", "Weather", "Battery",
        "System Info", "Flashlight", "Paddle", "Dice"
    };
    const char *app_icons[] = {
        SYM_BELL, SYM_STOPWATCH, SYM_CLOCK, SYM_MUSIC,
        SYM_NAVIGATION, SYM_METRONOME, SYM_WEATHER, SYM_BATTERY_HALF,
        SYM_INFO, SYM_FLASHLIGHT, SYM_PADDLE, SYM_DICE
    };
    
    int num_apps = 12;
    launcher_ui.tiles = (lv_obj_t**)malloc(num_apps * sizeof(lv_obj_t*));
    launcher_ui.labels = (lv_obj_t**)malloc(num_apps * sizeof(lv_obj_t*));
    launcher_ui.icons = (lv_obj_t**)malloc(num_apps * sizeof(lv_obj_t*));
    
    lv_obj_t *list = lv_list_create(scr, NULL);
    lv_obj_set_size(list, LV_HOR_RES_MAX - 20, LV_VER_RES_MAX - 60);
    lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 15);
    lv_obj_set_style_local_bg_color(list, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(list, LV_LIST_PART_BG, LV_STATE_DEFAULT, 0);
    
    for (int i = 0; i < num_apps; i++) {
        launcher_ui.icons[i] = lv_list_add_btn(list, app_icons[i], app_names[i]);
        lv_obj_set_style_local_bg_color(launcher_ui.icons[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_border_width(launcher_ui.icons[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_event_cb(launcher_ui.icons[i], app_tile_cb);
    }
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

/* ========== 创建秒表屏幕 ========== */
static void create_stopwatch_screen(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_local_bg_color(scr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_clean(scr);
    
    /* 标题 */
    lv_obj_t *title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Stop Watch");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
    
    /* 时间显示 */
    stopwatch_ui.label_time = lv_label_create(scr, NULL);
    lv_label_set_text(stopwatch_ui.label_time, "00:00.00");
    lv_obj_align(stopwatch_ui.label_time, scr, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_local_text_color(stopwatch_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(stopwatch_ui.label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
    
    /* 开始/暂停按钮 */
    stopwatch_ui.btn_start = lv_btn_create(scr, NULL);
    lv_obj_set_size(stopwatch_ui.btn_start, 60, 60);
    lv_obj_align(stopwatch_ui.btn_start, scr, LV_ALIGN_CENTER, -60, 50);
    lv_obj_set_style_local_bg_color(stopwatch_ui.btn_start, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x222222));
    lv_obj_set_event_cb(stopwatch_ui.btn_start, stopwatch_btn_cb);
    
    lv_obj_t *lbl = lv_label_create(stopwatch_ui.btn_start, NULL);
    lv_label_set_text(lbl, SYM_PLAY);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 计次按钮 */
    stopwatch_ui.btn_lap = lv_btn_create(scr, stopwatch_ui.btn_start);
    lv_obj_align(stopwatch_ui.btn_lap, scr, LV_ALIGN_CENTER, 0, 50);
    
    lbl = lv_label_create(stopwatch_ui.btn_lap, NULL);
    lv_label_set_text(lbl, SYM_LAPS_FLAG);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    /* 重置按钮 */
    stopwatch_ui.btn_reset = lv_btn_create(scr, stopwatch_ui.btn_start);
    lv_obj_align(stopwatch_ui.btn_reset, scr, LV_ALIGN_CENTER, 60, 50);
    
    lbl = lv_label_create(stopwatch_ui.btn_reset, NULL);
    lv_label_set_text(lbl, LV_SYMBOL_REFRESH);
    lv_obj_set_style_local_text_color(lbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    stopwatch_ui.elapsed_ms = 0;
    stopwatch_ui.running = false;
    
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
    last_activity_time = HAL_GetTick();
    
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

void infinitime_ui_init(void)
{
    ft3168_touch_init();
    
    lvgl_init();
    
    ST7789_FillScreen(COLOR_BLACK);
    
    HAL_Delay(100);
    
    create_clock_screen();
    
    last_activity_time = HAL_GetTick();
}

void infinitime_ui_task(void)
{
    /* 先处理触摸中断标志（读 I2C + 缓存数据），再让 LVGL 处理 */
    lvgl_touch_process();
    
    lv_task_handler();
    
    /* 更新调试图层（帧计数 + 触摸坐标），通过 LVGL label 走正常刷新 */
    lvgl_debug_draw();
    
    /* 检测屏幕超时 - 模拟InfiniTime的自动休眠 */
    uint32_t inactive_time = lv_disp_get_inactive_time(NULL);
    if (inactive_time > 30000 && !is_dimmed) {
        is_dimmed = true;
        ST7789_SetBacklight(5);
    } else if (inactive_time < 30000 && is_dimmed) {
        is_dimmed = false;
        ST7789_SetBacklight(10);
    }
    
    /* 触摸释放后禁用触摸输入 */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET) {
        lvgl_touch_release_handler();
    }
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