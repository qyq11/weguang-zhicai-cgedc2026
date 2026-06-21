#ifndef ES7210_H
#define ES7210_H

#include <stdint.h>

/* ES7210 常见 7-bit 地址，最终以硬件脚位和 I2C 扫描结果为准。 */
#define ES7210_I2C_ADDR_DEFAULT      0x40U

/* ES7210 初始化返回值。 */
typedef enum {
    ES7210_OK = 0,
    ES7210_ERR_I2C = -1,
    ES7210_ERR_NO_REG_TABLE = -2
} es7210_status_t;

/* ES7210 初始化和地址配置接口。 */
int es7210_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels);
void es7210_set_i2c_addr(uint8_t addr_7bit);
uint8_t es7210_get_i2c_addr(void);

#endif /* ES7210_H */
