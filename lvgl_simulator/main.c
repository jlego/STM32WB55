#include "lvgl/lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

/* 屏幕尺寸 - 与项目一致 */
#define DISP_HOR_RES 240
#define DISP_VER_RES 280

/* SDL 窗口和渲染器 */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static uint32_t *fb_buf = NULL;

/* LVGL 显示刷新回调 */
static void disp_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            /* LVGL 使用 RGB565 格式，且 LV_COLOR_16_SWAP=1，字节是交换的 */
            /* 需要先交换字节，再转换到 ARGB8888 */
            uint16_t color = color_p->full;
            /* 交换高低字节 */
            color = ((color >> 8) & 0xFF) | ((color << 8) & 0xFF00);
            uint8_t r = (color >> 11) & 0x1F;
            uint8_t g = (color >> 5) & 0x3F;
            uint8_t b = color & 0x1F;
            /* 扩展到 8 位 */
            r = (r * 255) / 31;
            g = (g * 255) / 63;
            b = (b * 255) / 31;
            /* ABGR8888 格式（SDL_PIXELFORMAT_ABGR8888） */
            fb_buf[y * DISP_HOR_RES + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            color_p++;
        }
    }
    lv_disp_flush_ready(disp_drv);
}

/* 触摸输入处理 */
static bool mouse_pressed = false;
static bool last_mouse_pressed = false;
static int mouse_x = 0, mouse_y = 0;

static bool touchpad_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->state = mouse_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = mouse_x;
    data->point.y = mouse_y;
    return false;
}

/* 鼠标滚轮输入处理 */
static int mouse_wheel_diff = 0;

static bool mousewheel_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->state = mouse_wheel_diff != 0 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->enc_diff = mouse_wheel_diff;
    mouse_wheel_diff = 0;
    return false;
}

/* 外部滑动手势检测函数 */
extern void infinitime_detect_swipe(lv_coord_t x, lv_coord_t y, bool pressed);

/* 处理 SDL 事件 */
static void sdl_events_process(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                /* 清理并退出 */
                free(fb_buf);
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(0);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    mouse_pressed = true;
                    /* 转换坐标：窗口是 2 倍放大，需要除以 2 */
                    mouse_x = event.button.x / 2;
                    mouse_y = event.button.y / 2;
                    /* 确保坐标在屏幕范围内 */
                    if(mouse_x < 0) mouse_x = 0;
                    if(mouse_x >= DISP_HOR_RES) mouse_x = DISP_HOR_RES - 1;
                    if(mouse_y < 0) mouse_y = 0;
                    if(mouse_y >= DISP_VER_RES) mouse_y = DISP_VER_RES - 1;
                    /* 调用滑动手势检测 */
                    infinitime_detect_swipe(mouse_x, mouse_y, true);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    mouse_pressed = false;
                    /* 调用滑动手势检测（释放） */
                    infinitime_detect_swipe(mouse_x, mouse_y, false);
                }
                break;
            case SDL_MOUSEMOTION:
                /* 转换坐标：窗口是 2 倍放大，需要除以 2 */
                mouse_x = event.motion.x / 2;
                mouse_y = event.motion.y / 2;
                /* 确保坐标在屏幕范围内 */
                if(mouse_x < 0) mouse_x = 0;
                if(mouse_x >= DISP_HOR_RES) mouse_x = DISP_HOR_RES - 1;
                if(mouse_y < 0) mouse_y = 0;
                if(mouse_y >= DISP_VER_RES) mouse_y = DISP_VER_RES - 1;
                /* 如果鼠标按下，调用滑动手势检测 */
                if(mouse_pressed) {
                    infinitime_detect_swipe(mouse_x, mouse_y, true);
                }
                break;
            case SDL_MOUSEWHEEL:
                /* 鼠标滚轮 - 用于页面滚动 */
                mouse_wheel_diff += event.wheel.y;
                break;
            case SDL_KEYDOWN:
                /* ESC 键退出 */
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    free(fb_buf);
                    SDL_DestroyTexture(texture);
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    exit(0);
                }
                /* 方向键模拟滑动手势 */
                else if(event.key.keysym.sym == SDLK_UP) {
                    /* 上滑：从时钟页到应用列表 */
                    infinitime_detect_swipe(120, 140, true);
                    infinitime_detect_swipe(120, 40, true);
                    infinitime_detect_swipe(120, 40, false);
                }
                else if(event.key.keysym.sym == SDLK_DOWN) {
                    /* 下滑：从时钟页到快捷设置 */
                    infinitime_detect_swipe(120, 40, true);
                    infinitime_detect_swipe(120, 140, true);
                    infinitime_detect_swipe(120, 140, false);
                }
                else if(event.key.keysym.sym == SDLK_LEFT) {
                    /* 左滑：返回时钟 */
                    infinitime_detect_swipe(120, 140, true);
                    infinitime_detect_swipe(20, 140, true);
                    infinitime_detect_swipe(20, 140, false);
                }
                else if(event.key.keysym.sym == SDLK_RIGHT) {
                    /* 右滑：到通知页 */
                    infinitime_detect_swipe(20, 140, true);
                    infinitime_detect_swipe(120, 140, true);
                    infinitime_detect_swipe(120, 140, false);
                }
                break;
        }
    }
}

/* 外部 UI 初始化函数 */
extern void infinitime_ui_init(void);
extern void infinitime_ui_task(void);

int main(int argc, char **argv)
{
    printf("LVGL Simulator - InfiniTime UI\n");
    printf("Screen: %dx%d\n", DISP_HOR_RES, DISP_VER_RES);
    printf("Mouse Left Click = Touch\n");
    printf("Mouse Drag = Swipe\n");
    printf("Mouse Wheel = Scroll\n");
    printf("Arrow Keys = Swipe (Up/Down/Left/Right)\n");
    printf("ESC = Quit\n\n");

    /* 初始化 SDL */
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }

    /* 创建窗口 */
    window = SDL_CreateWindow("LVGL Simulator - InfiniTime UI",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              DISP_HOR_RES * 2, DISP_VER_RES * 2,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                SDL_TEXTUREACCESS_STATIC, DISP_HOR_RES, DISP_VER_RES);
    if(!texture) {
        fprintf(stderr, "Texture creation failed: %s\n", SDL_GetError());
        return 1;
    }

    /* 分配帧缓冲 */
    fb_buf = (uint32_t *)malloc(DISP_HOR_RES * DISP_VER_RES * sizeof(uint32_t));
    if(!fb_buf) {
        fprintf(stderr, "Framebuffer allocation failed\n");
        return 1;
    }
    memset(fb_buf, 0, DISP_HOR_RES * DISP_VER_RES * sizeof(uint32_t));

    /* 初始化 LVGL */
    lv_init();

    /* 注册显示驱动 */
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf1[DISP_HOR_RES * 10];
    lv_disp_buf_init(&disp_buf, buf1, NULL, DISP_HOR_RES * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* 注册触摸输入驱动 */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read_cb;
    lv_indev_drv_register(&indev_drv);

    /* 注册鼠标滚轮输入驱动 */
    lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = mousewheel_read_cb;
    lv_indev_drv_register(&enc_drv);

    /* 初始化 UI */
    infinitime_ui_init();

    printf("UI initialized successfully!\n");
    printf("Starting main loop...\n\n");

    /* 主循环 - 所有 SDL 事件处理必须在主线程 */
    uint32_t frame_count = 0;
    uint32_t last_fps_time = SDL_GetTicks();
    
    while(1) {
        /* 处理 SDL 事件（必须在主线程） */
        sdl_events_process();

        /* 更新 LVGL tick */
        lv_tick_inc(1);

        /* 调用 LVGL 任务处理 */
        infinitime_ui_task();

        /* 更新 SDL 纹理 */
        SDL_UpdateTexture(texture, NULL, fb_buf, DISP_HOR_RES * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        /* 帧率统计 */
        frame_count++;
        uint32_t now = SDL_GetTicks();
        if(now - last_fps_time >= 1000) {
            printf("FPS: %u\n", frame_count);
            frame_count = 0;
            last_fps_time = now;
        }

        SDL_Delay(16); /* ~60 FPS */
    }

    return 0;
}