#include "es8311.h"
#include "bsp_i2c.h"
#include <stdio.h>

/* 当前使用的 ES8311 7-bit I2C 地址。 */
static uint8_t s_es8311_addr = ES8311_I2C_ADDR_DEFAULT;

/* 设置 ES8311 I2C 地址。 */
void es8311_set_i2c_addr(uint8_t addr_7bit)
{
    s_es8311_addr = addr_7bit;
}

/* 获取当前 ES8311 I2C 地址。 */
uint8_t es8311_get_i2c_addr(void)
{
    return s_es8311_addr;
}

/* ES8311 初始化占位函数：保留初始化入口，不加入运行期业务。 */
int es8311_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels)
{
    /*
     * TODO：需要根据 ES8311 数据手册或官方例程补充真实寄存器表。
     * 需要确认地址、时钟比例、I2S 格式、DAC 上电路径、输出路由等。
     */
    (void)sample_rate;
    (void)bit_width;
    (void)channels;

    printf("\r\nES8311 init TODO: need datasheet/register table, addr=0x%02X", s_es8311_addr);
    return ES8311_ERR_NO_REG_TABLE;
}
