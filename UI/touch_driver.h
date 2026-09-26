#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* CST816S触摸芯片寄存器地址 */
#define CST816S_REG_GESTURE     0x01
#define CST816S_REG_TOUCH_NUM   0x02
#define CST816S_REG_X_HIGH      0x03
#define CST816S_REG_X_LOW       0x04
#define CST816S_REG_Y_HIGH      0x05
#define CST816S_REG_Y_LOW       0x06
#define CST816S_REG_CHIP_ID     0xA7
#define CST816S_REG_VENDOR_ID   0xA8
#define CST816S_REG_FW_VERSION  0xA9

/* 触摸手势定义 */
typedef enum {
    TOUCH_GESTURE_NONE = 0x00,
    TOUCH_GESTURE_SLIDE_DOWN = 0x01,
    TOUCH_GESTURE_SLIDE_UP = 0x02,
    TOUCH_GESTURE_SLIDE_LEFT = 0x03,
    TOUCH_GESTURE_SLIDE_RIGHT = 0x04,
    TOUCH_GESTURE_SINGLE_TAP = 0x05,
    TOUCH_GESTURE_DOUBLE_TAP = 0x0B,
    TOUCH_GESTURE_LONG_PRESS = 0x0C
} touch_gesture_t;

/* 触摸信息结构 */
typedef struct {
    uint16_t x;
    uint16_t y;
    touch_gesture_t gesture;
    bool is_touching;
    bool is_valid;
} touch_info_t;

/* 初始化触摸驱动 */
bool touch_init(void);

/* 获取触摸信息 */
touch_info_t touch_get_info(void);

/* 触摸休眠 */
void touch_sleep(void);

/* 触摸唤醒 */
void touch_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_DRIVER_H */