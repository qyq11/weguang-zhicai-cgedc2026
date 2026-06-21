#include "es7210.h"
#include "bsp_i2c.h"
#include <stdio.h>

/* 当前使用的 ES7210 7-bit I2C 地址。 */
static uint8_t s_es7210_addr = ES7210_I2C_ADDR_DEFAULT;

/* 设置 ES7210 I2C 地址，供不同硬件焊接配置使用。 */
void es7210_set_i2c_addr(uint8_t addr_7bit)
{
    s_es7210_addr = addr_7bit;
}

/* 获取当前 ES7210 I2C 地址。 */
uint8_t es7210_get_i2c_addr(void)
{
    return s_es7210_addr;
}

/* ES7210 初始化占位函数：保留初始化入口，不加入运行期业务。 */
int es7210_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels)
{
    /*
     * TODO：需要根据 ES7210 数据手册或官方例程补充真实寄存器表。
     * 补表前不要凭空写寄存器，先确认地址、MCLK/LRCK/BCLK 比例、
     * 音频格式、通道使能、PGA/数字增益和上电时序。
     */
    (void)sample_rate;
    (void)bit_width;
    (void)channels;

    printf("\r\nES7210 init TODO: need datasheet/register table, addr=0x%02X", s_es7210_addr);
    return ES7210_ERR_NO_REG_TABLE;
}
