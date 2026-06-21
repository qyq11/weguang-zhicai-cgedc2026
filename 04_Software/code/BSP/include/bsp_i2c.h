#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "gd32h73x_75x.h"
#include <stdint.h>

/* 音频控制总线：PB10 = I2C1_SCL，PB11 = I2C1_SDA。 */
#define BSP_AUDIO_I2C             I2C1
#define BSP_AUDIO_I2C_RCU         RCU_I2C1
#define BSP_AUDIO_I2C_GPIO_RCU    RCU_GPIOB
#define BSP_AUDIO_I2C_GPIO_PORT   GPIOB
#define BSP_AUDIO_I2C_SCL_PIN     GPIO_PIN_10
#define BSP_AUDIO_I2C_SDA_PIN     GPIO_PIN_11
#define BSP_AUDIO_I2C_GPIO_AF     GPIO_AF_3
#define BSP_I2C_TIMEOUT           0x100000U

/* 阻塞式 I2C 辅助函数返回值。 */
typedef enum {
    BSP_I2C_OK = 0,
    BSP_I2C_ERR_TIMEOUT,
    BSP_I2C_ERR_NACK,
    BSP_I2C_ERR_BUS
} bsp_i2c_status_t;

/* I2C 初始化、基础收发、寄存器读写和总线扫描接口。 */
void bsp_audio_i2c_init(void);
bsp_i2c_status_t bsp_i2c_write(uint8_t dev_addr_7bit, const uint8_t *data, uint16_t len);
bsp_i2c_status_t bsp_i2c_read(uint8_t dev_addr_7bit, uint8_t *data, uint16_t len);
bsp_i2c_status_t bsp_i2c_write_reg8(uint8_t dev_addr_7bit, uint8_t reg, uint8_t value);
bsp_i2c_status_t bsp_i2c_read_reg8(uint8_t dev_addr_7bit, uint8_t reg, uint8_t *value);
uint8_t bsp_i2c_scan(uint8_t *addr_list, uint8_t max_count);

#endif /* BSP_I2C_H */
