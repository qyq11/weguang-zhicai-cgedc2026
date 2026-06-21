#include "audio_app.h"
#include "bsp_i2c.h"
#include <stdio.h>

/* 初始化阶段扫描到的 I2C 设备数量。 */
static uint8_t s_i2c_found_count = 0U;

/* 调试观察变量：在 Keil Watch 中查看扫描到的 I2C 地址。 */
volatile uint8_t debug_i2c_addr_list[16];
volatile uint8_t debug_i2c_found_count = 0U;

/* 音频应用层初始化：当前只清空调试计数，不包含运行期业务。 */
void audio_app_init(void)
{
    s_i2c_found_count = 0U;
    debug_i2c_found_count = 0U;
}

/* 初始化阶段扫描 I2C 总线，确认 Codec 等音频器件是否在线。 */
void audio_app_scan_i2c(void)
{
    uint8_t addr_list[16];
    uint8_t i;

    printf("\r\nI2C scan start");
    s_i2c_found_count = bsp_i2c_scan(addr_list, 16U);
    debug_i2c_found_count = s_i2c_found_count;

    if(s_i2c_found_count == 0U) {
        printf("\r\nNo audio codec found");
        return;
    }

    for(i = 0U; (i < s_i2c_found_count) && (i < 16U); i++) {
        debug_i2c_addr_list[i] = addr_list[i];
        printf("\r\nI2C device found: 0x%02X", addr_list[i]);
    }
}
