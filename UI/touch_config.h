#ifndef TOUCH_CONFIG_H
#define TOUCH_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* 触摸芯片选择 */
#define TOUCH_CHIP_FT6X36   0
#define TOUCH_CHIP_CST816S  1
#define TOUCH_CHIP_FT3168   2

/* 默认使用FT6X36 */
#ifndef TOUCH_CHIP_TYPE
#define TOUCH_CHIP_TYPE     TOUCH_CHIP_FT6X36
#endif

/* 根据配置选择触摸芯片 */
#if TOUCH_CHIP_TYPE == TOUCH_CHIP_FT6X36
    #define TOUCH_CHIP_NAME "FT6X36"
#elif TOUCH_CHIP_TYPE == TOUCH_CHIP_CST816S
    #define TOUCH_CHIP_NAME "CST816S"
#elif TOUCH_CHIP_TYPE == TOUCH_CHIP_FT3168
    #define TOUCH_CHIP_NAME "FT3168"
#else
    #error "Unknown touch chip type!"
#endif

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_CONFIG_H */