#ifndef ES8311_H
#define ES8311_H

#include <stdint.h>

/* ES8311 常见 7-bit 地址，最终以 I2C 扫描结果为准。 */
#define ES8311_I2C_ADDR_DEFAULT      0x18U

/* ES8311 初始化返回值。 */
typedef enum {
    ES8311_OK = 0,
    ES8311_ERR_I2C = -1,
    ES8311_ERR_NO_REG_TABLE = -2
} es8311_status_t;

/* ES8311 初始化和地址配置接口。 */
int es8311_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels);
void es8311_set_i2c_addr(uint8_t addr_7bit);
uint8_t es8311_get_i2c_addr(void);

#endif /* ES8311_H */
