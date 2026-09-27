#ifndef FT3168_TOUCH_H
#define FT3168_TOUCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* FT3168触摸芯片寄存器地址 */
#define FT3168_REG_DEVICE_MODE  0x00
#define FT3168_REG_GESTURE      0x01
#define FT3168_REG_TD_STATUS    0x02
#define FT3168_REG_TOUCH1_XH    0x03
#define FT3168_REG_TOUCH1_XL    0x04
#define FT3168_REG_TOUCH1_YH    0x05
#define FT3168_REG_TOUCH1_YL    0x06
#define FT3168_REG_TOUCH2_XH    0x09
#define FT3168_REG_TOUCH2_XL    0x0A
#define FT3168_REG_TOUCH2_YH    0x0B
#define FT3168_REG_TOUCH2_YL    0x0C
#define FT3168_REG_THRESHOLD    0x80
#define FT3168_REG_MONITOR_TIME 0x87
#define FT3168_REG_PERIOD_ACTIVE 0x88
#define FT3168_REG_PERIOD_MONITOR 0x89
#define FT3168_REG_CHIP_ID      0xA3
#define FT3168_REG_VENDOR1_ID   0xA8
#define FT3168_REG_ERROR_STATUS 0xA9
#define FT3168_REG_FIRMWARE_ID  0xA6
#define FT3168_REG_POWER_MODE   0xA5
#define FT3168_REG_INT_STATUS   0xA4
#define FT3168_REG_RELEASE_CODE 0xAF
#define FT3168_REG_COUNTRY_CODE 0xAE

/* FT3168 I2C地址 */
#define FT3168_I2C_ADDR         0x38

/* FocalTech芯片ID */
#define FT6206_CHIP_ID          0x06
#define FT3267_CHIP_ID          0x33
#define FT6236_CHIP_ID          0x36
#define FT6236U_CHIP_ID         0x64
#define FT5206U_CHIP_ID         0x64

/* 触摸状态 */
#define FT3168_TOUCH_EVENT_DOWN 0x00
#define FT3168_TOUCH_EVENT_UP   0x01
#define FT3168_TOUCH_EVENT_CONTACT 0x02

/* 手势定义 */
typedef enum {
    FT3168_GESTURE_NONE = 0x00,
    FT3168_GESTURE_MOVE_UP = 0x10,
    FT3168_GESTURE_MOVE_LEFT = 0x14,
    FT3168_GESTURE_MOVE_DOWN = 0x18,
    FT3168_GESTURE_MOVE_RIGHT = 0x1C,
    FT3168_GESTURE_ZOOM_IN = 0x48,
    FT3168_GESTURE_ZOOM_OUT = 0x49
} ft3168_gesture_t;

/* 触摸点信息 */
typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t id;
    uint8_t event;
    bool is_valid;
} ft3168_touch_point_t;

/* 触摸信息 */
typedef struct {
    uint8_t touch_points;
    ft3168_touch_point_t points[2];
    ft3168_gesture_t gesture;
    bool is_touching;
} ft3168_touch_info_t;

/* 初始化FT3168触摸驱动 */
bool ft3168_touch_init(void);

/* 获取触摸信息 */
ft3168_touch_info_t ft3168_touch_get_info(void);

/* 读取芯片ID */
uint8_t ft3168_touch_get_chip_id(void);

/* 读取固件版本 */
uint8_t ft3168_touch_get_firmware_id(void);

/* 进入工厂模式 */
bool ft3168_touch_enter_factory_mode(void);

/* 退出工厂模式 */
bool ft3168_touch_exit_factory_mode(void);

/* 设置中断模式 */
void ft3168_touch_set_interrupt_mode(uint8_t mode);

/* 触摸休眠 */
void ft3168_touch_sleep(void);

/* 触摸唤醒 */
void ft3168_touch_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* FT3168_TOUCH_H */