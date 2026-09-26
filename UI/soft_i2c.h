#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* I2C引脚定义 */
#define SOFT_I2C_SCL_PIN        GPIO_PIN_6
#define SOFT_I2C_SCL_PORT       GPIOB
#define SOFT_I2C_SDA_PIN        GPIO_PIN_7
#define SOFT_I2C_SDA_PORT       GPIOB

/* I2C延时 */
#define SOFT_I2C_DELAY()        for(volatile int i=0; i<10; i++)

/* I2C宏定义 */
#define SOFT_I2C_SCL_HIGH()     HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET)
#define SOFT_I2C_SCL_LOW()      HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET)
#define SOFT_I2C_SDA_HIGH()     HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET)
#define SOFT_I2C_SDA_LOW()      HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET)
#define SOFT_I2C_SDA_READ()     HAL_GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN)

/* 初始化软件I2C */
void soft_i2c_init(void);

/* I2C起始信号 */
void soft_i2c_start(void);

/* I2C停止信号 */
void soft_i2c_stop(void);

/* 写入一个字节 */
bool soft_i2c_write_byte(uint8_t byte);

/* 读取一个字节 */
uint8_t soft_i2c_read_byte(bool ack);

/* 写入数据到寄存器 */
bool soft_i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);

/* 从寄存器读取数据 */
bool soft_i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* SOFT_I2C_H */