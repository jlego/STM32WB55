#include "ft3168_touch.h"
#include "stm32wbxx_hal.h"
#include "core_cm4.h"  // 用于访问DWT寄存器

extern I2C_HandleTypeDef hi2c1;

/* 软件延迟函数 - 使用DWT周期计数器，不依赖SysTick */
static void sw_delay_ms(uint32_t ms)
{
    /* 启用DWT周期计数器 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    /* STM32WB55 SYSCLK=64MHz, 1ms ≈ 64000 cycles */
    uint32_t cycles_per_ms = 64000;
    uint32_t target_cycles = ms * cycles_per_ms;
    
    /* 读取当前周期计数 */
    uint32_t start = DWT->CYCCNT;
    
    /* 等待直到达到目标周期数 */
    while ((DWT->CYCCNT - start) < target_cycles) {
        /* 软件超时：防止DWT溢出或其他问题 */
        if ((DWT->CYCCNT - start) > 100000000) break; /* 约1.5秒超时 */
    }
}

/* FT3168 I2C地址 (7-bit) */
#define FT3168_I2C_ADDR         0x38

/* 寄存器定义 - 与FT6X36/FT6236兼容 */
#define FT3168_REG_DEVICE_MODE  0x00
#define FT3168_REG_GESTURE      0x01
#define FT3168_REG_STATUS       0x02
#define FT3168_REG_TOUCH1_XH    0x03
#define FT3168_REG_TOUCH1_XL    0x04
#define FT3168_REG_TOUCH1_YH    0x05
#define FT3168_REG_TOUCH1_YL    0x06
#define FT3168_REG_TOUCH2_XH    0x09
#define FT3168_REG_TOUCH2_XL    0x0A
#define FT3168_REG_TOUCH2_YH    0x0B
#define FT3168_REG_TOUCH2_YL    0x0C
#define FT3168_REG_THRESHOLD    0x80
#define FT3168_REG_PERIOD_ACTIVE 0x88
#define FT3168_REG_FIRM_VERS    0xA6
#define FT3168_REG_CHIP_ID      0xA3
#define FT3168_REG_VENDOR_ID    0xA8

/* GPIO引脚定义 */
#define FT3168_INT_PIN      GPIO_PIN_3
#define FT3168_INT_PORT     GPIOA

#define FT3168_RST_PIN      GPIO_PIN_2
#define FT3168_RST_PORT     GPIOA

/* 芯片ID */
static uint8_t chip_id = 0;
static uint8_t firmware_id = 0;
static bool touch_initialized = false;

/* 诊断信息 */
static uint8_t diag_vend_id = 0;
static uint8_t diag_chip_id = 0;
static uint8_t diag_fail_step = 0;
static HAL_StatusTypeDef diag_last_i2c_status = HAL_OK;
static uint8_t diag_i2c_scan[8] = {0};
static uint8_t diag_gpio_probe = 0;

uint8_t ft3168_diag_get_fail_step(void) { return diag_fail_step; }
uint8_t ft3168_diag_get_vend_id(void) { return diag_vend_id; }
uint8_t ft3168_diag_get_chip_id(void) { return diag_chip_id; }
HAL_StatusTypeDef ft3168_diag_get_last_i2c_status(void) { return diag_last_i2c_status; }
const uint8_t * ft3168_diag_get_i2c_scan(void) { return diag_i2c_scan; }
uint8_t ft3168_diag_get_gpio_probe(void) { return diag_gpio_probe; }

static void gpio_i2c_scl_high(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_6;
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &g);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

static void gpio_i2c_scl_low(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

static void gpio_i2c_sda_high(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_7;
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &g);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
}

static void gpio_i2c_sda_low(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}

static uint8_t gpio_i2c_sda_read(void)
{
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
}

static void gpio_i2c_delay(void)
{
    for (volatile int i = 0; i < 200; i++) __NOP();
}

static void gpio_i2c_start(void)
{
    gpio_i2c_sda_high();
    gpio_i2c_scl_high();
    gpio_i2c_delay();
    gpio_i2c_sda_low();
    gpio_i2c_delay();
    gpio_i2c_scl_low();
    gpio_i2c_delay();
}

static void gpio_i2c_stop(void)
{
    gpio_i2c_sda_low();
    gpio_i2c_delay();
    gpio_i2c_scl_high();
    gpio_i2c_delay();
    gpio_i2c_sda_high();
    gpio_i2c_delay();
}

static void gpio_i2c_send_byte(uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        if (byte & 0x80) gpio_i2c_sda_high();
        else gpio_i2c_sda_low();
        byte <<= 1;
        gpio_i2c_delay();
        gpio_i2c_scl_high();
        gpio_i2c_delay();
        gpio_i2c_scl_low();
        gpio_i2c_delay();
    }
}

static uint8_t gpio_i2c_wait_ack(void)
{
    gpio_i2c_sda_high();
    gpio_i2c_delay();
    gpio_i2c_scl_high();
    gpio_i2c_delay();
    uint8_t ack = gpio_i2c_sda_read();
    gpio_i2c_scl_low();
    gpio_i2c_delay();
    return ack;
}

void ft3168_diag_gpio_probe(void)
{
    diag_gpio_probe = 0;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    gpio_i2c_start();
    gpio_i2c_send_byte(0x70);   // 0x38 << 1, write mode
    uint8_t ack = gpio_i2c_wait_ack();
    gpio_i2c_stop();
    
    diag_gpio_probe = ack ? 0 : 1;
    
    {
        GPIO_InitTypeDef g = {0};
        g.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        g.Mode = GPIO_MODE_AF_OD;
        g.Pull = GPIO_PULLUP;
        g.Speed = GPIO_SPEED_FREQ_HIGH;
        g.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &g);
    }
}

void ft3168_diag_i2c_scan(void)
{
    for (int i = 0; i < 8; i++) diag_i2c_scan[i] = 0;
    
    extern I2C_HandleTypeDef hi2c1;
    for (uint8_t addr = 1; addr < 127; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, (addr << 1), 3, 10) == HAL_OK) {
            diag_i2c_scan[addr / 16] |= (1 << (addr % 16));
        }
    }
}

/* I2C读取函数 - 通过硬件I2C1 */
static bool ft3168_i2c_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    diag_last_i2c_status = HAL_I2C_Mem_Read(&hi2c1, (FT3168_I2C_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    return (diag_last_i2c_status == HAL_OK);
}

/* I2C写入函数 - 通过硬件I2C1 */
static bool ft3168_i2c_write(uint8_t reg, uint8_t *data, uint16_t len)
{
    diag_last_i2c_status = HAL_I2C_Mem_Write(&hi2c1, (FT3168_I2C_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    return (diag_last_i2c_status == HAL_OK);
}

bool ft3168_diag_direct_read(uint8_t reg, uint8_t *data, uint16_t len)
{
    return ft3168_i2c_read(reg, data, len);
}

/* 硬件复位 - 使用软件延迟，不依赖SysTick */
static void ft3168_hard_reset(void)
{
    HAL_GPIO_WritePin(FT3168_RST_PORT, FT3168_RST_PIN, GPIO_PIN_SET);
    sw_delay_ms(10);
    HAL_GPIO_WritePin(FT3168_RST_PORT, FT3168_RST_PIN, GPIO_PIN_RESET);
    sw_delay_ms(30);
    HAL_GPIO_WritePin(FT3168_RST_PORT, FT3168_RST_PIN, GPIO_PIN_SET);
    sw_delay_ms(160);
}

/* 初始化FT3168触摸驱动 */
bool ft3168_touch_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    diag_fail_step = 0;
    diag_last_i2c_status = HAL_OK;
    
    HAL_I2C_DeInit(&hi2c1);
    HAL_I2C_Init(&hi2c1);
    HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);
    HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0);
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置复位引脚 */
    GPIO_InitStruct.Pin = FT3168_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT3168_RST_PORT, &GPIO_InitStruct);
    
    /* 配置中断引脚 */
    GPIO_InitStruct.Pin = FT3168_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(FT3168_INT_PORT, &GPIO_InitStruct);
    
    /* 硬件复位 */
    ft3168_hard_reset();
    
    sw_delay_ms(20);
    
    {
        uint8_t active_mode = 0x01;
        ft3168_i2c_write(0x00, &active_mode, 1);
    }
    sw_delay_ms(5);
    
    ft3168_i2c_read(0xA8, &diag_vend_id, 1);
    ft3168_i2c_read(0xA3, &chip_id, 1);
    ft3168_i2c_read(0xA6, &firmware_id, 1);
    
    {
        uint8_t threshold = 40;
        ft3168_i2c_write(0x80, &threshold, 1);
    }
    
    {
        uint8_t report_rate = 0x14;
        ft3168_i2c_write(0x88, &report_rate, 1);
    }
    
    {
        uint8_t mode_check = 0;
        ft3168_i2c_read(0x00, &mode_check, 1);
        if (mode_check != 0x01) {
            uint8_t active = 0x01;
            ft3168_i2c_write(0x00, &active, 1);
        }
    }
    
    touch_initialized = true;
    diag_fail_step = 0;
    return true;
}

/* 获取触摸信息 */
ft3168_touch_info_t ft3168_touch_get_info(void)
{
    ft3168_touch_info_t info = {0};
    uint8_t buf[16] = {0};
    
    if (!touch_initialized) {
        return info;
    }
    
    /* 从REG 0x00开始读取16字节: mode, gesture, status, touch1_xy, ..., touch2_xy */
    if (!ft3168_i2c_read(FT3168_REG_DEVICE_MODE, buf, 16)) {
        return info;
    }
    
    /* 获取触摸点数 (REG 0x02) */
    info.touch_points = buf[2] & 0x0F;
    if (info.touch_points == 0 || info.touch_points == 0x0F) {
        info.touch_points = 0;
        return info;
    }
    if (info.touch_points > 2) {
        info.touch_points = 2;
    }
    
    /* 获取手势 (REG 0x01) */
    switch (buf[1]) {
    case 0x10: info.gesture = FT3168_GESTURE_MOVE_UP;    break;
    case 0x14: info.gesture = FT3168_GESTURE_MOVE_RIGHT;  break;
    case 0x18: info.gesture = FT3168_GESTURE_MOVE_DOWN;   break;
    case 0x1C: info.gesture = FT3168_GESTURE_MOVE_LEFT;   break;
    case 0x48: info.gesture = FT3168_GESTURE_ZOOM_IN;     break;
    case 0x49: info.gesture = FT3168_GESTURE_ZOOM_OUT;    break;
    default:   info.gesture = FT3168_GESTURE_NONE;         break;
    }
    
    /* 解析第一个触摸点 (REG 0x03~0x06) */
    if (info.touch_points >= 1) {
        info.points[0].event = (buf[3] >> 6) & 0x03;
        info.points[0].id = (buf[3] >> 4) & 0x0F;
        info.points[0].x = ((buf[3] & 0x0F) << 8) | buf[4];
        info.points[0].y = ((buf[5] & 0x0F) << 8) | buf[6];
        info.points[0].is_valid = (info.points[0].x < 240 && info.points[0].y < 280);
    }
    
    /* 解析第二个触摸点 (REG 0x09~0x0C) */
    if (info.touch_points >= 2) {
        info.points[1].event = (buf[9] >> 6) & 0x03;
        info.points[1].id = (buf[9] >> 4) & 0x0F;
        info.points[1].x = ((buf[9] & 0x0F) << 8) | buf[10];
        info.points[1].y = ((buf[11] & 0x0F) << 8) | buf[12];
        info.points[1].is_valid = (info.points[1].x < 240 && info.points[1].y < 280);
    }
    
    /* 判断是否有触摸 */
    info.is_touching = (info.touch_points > 0);
    
    return info;
}

/* 读取芯片ID */
uint8_t ft3168_touch_get_chip_id(void)
{
    return chip_id;
}

/* 读取固件版本 */
uint8_t ft3168_touch_get_firmware_id(void)
{
    return firmware_id;
}

/* 进入工厂模式 */
bool ft3168_touch_enter_factory_mode(void)
{
    uint8_t mode = 0x40;
    return ft3168_i2c_write(FT3168_REG_DEVICE_MODE, &mode, 1);
}

/* 退出工厂模式 */
bool ft3168_touch_exit_factory_mode(void)
{
    uint8_t mode = 0x00;
    return ft3168_i2c_write(FT3168_REG_DEVICE_MODE, &mode, 1);
}

/* 设置中断模式 */
void ft3168_touch_set_interrupt_mode(uint8_t mode)
{
    ft3168_i2c_write(0xA4, &mode, 1);
}

/* 触摸休眠 - 深度睡眠模式 (~100uA) */
void ft3168_touch_sleep(void)
{
    uint8_t pmode = 0x03;
    ft3168_i2c_write(0xA5, &pmode, 1);
}

/* 触摸唤醒 - 通过硬件复位退出深度睡眠 */
void ft3168_touch_wakeup(void)
{
    ft3168_hard_reset();
}

/* 中断处理函数 */
void FT3168_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(FT3168_INT_PIN);
}

/* 中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == FT3168_INT_PIN) {
        /* 触摸中断触发，启用 LVGL 触摸输入设备 */
        lvgl_touch_irq_handler();
    }
}