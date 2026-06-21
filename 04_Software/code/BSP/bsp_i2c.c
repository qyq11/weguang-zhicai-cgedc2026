#include "bsp_i2c.h"

static void i2c_bus_recover(void);

/* 等待指定 I2C 标志达到目标状态，超时或 NACK 时返回错误。 */
static bsp_i2c_status_t i2c_wait_flag(uint32_t flag, FlagStatus status)
{
    uint32_t timeout = BSP_I2C_TIMEOUT;

    while(i2c_flag_get(BSP_AUDIO_I2C, flag) != status) {
        if(SET == i2c_flag_get(BSP_AUDIO_I2C, I2C_FLAG_NACK)) {
            i2c_flag_clear(BSP_AUDIO_I2C, I2C_FLAG_NACK);
            return BSP_I2C_ERR_NACK;
        }
        if(timeout-- == 0U) {
            return BSP_I2C_ERR_TIMEOUT;
        }
    }

    return BSP_I2C_OK;
}

/* 清除 STOP 检测标志。 */
static void i2c_clear_stop_flag(void)
{
    if(SET == i2c_flag_get(BSP_AUDIO_I2C, I2C_FLAG_STPDET)) {
        i2c_flag_clear(BSP_AUDIO_I2C, I2C_FLAG_STPDET);
    }
}

/* 初始化音频 I2C 总线 GPIO、时序和主机模式。 */
void bsp_audio_i2c_init(void)
{
    rcu_periph_clock_enable(BSP_AUDIO_I2C_GPIO_RCU);
    rcu_periph_clock_enable(BSP_AUDIO_I2C_RCU);

    /*
     * I2C 必须使用开漏输出。内部上拉便于调试，实际硬件建议外接
     * 2.2k~4.7k 上拉到 3.3V，波形会更稳定。
     */
    gpio_af_set(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_GPIO_AF,
                BSP_AUDIO_I2C_SCL_PIN | BSP_AUDIO_I2C_SDA_PIN);
    gpio_mode_set(BSP_AUDIO_I2C_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,
                  BSP_AUDIO_I2C_SCL_PIN | BSP_AUDIO_I2C_SDA_PIN);
    gpio_output_options_set(BSP_AUDIO_I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ,
                            BSP_AUDIO_I2C_SCL_PIN | BSP_AUDIO_I2C_SDA_PIN);

    i2c_bus_recover();

    i2c_deinit(BSP_AUDIO_I2C);

    /*
     * 这里使用偏保守的低速调试时序。若实测 SCL 偏差较大，
     * 需要按 APB 时钟和示波器结果重新计算 timing 参数。
     */
    i2c_timing_config(BSP_AUDIO_I2C, 0x01, 0x07, 0x00);
    i2c_master_clock_config(BSP_AUDIO_I2C, 0x2D, 0x87);
    i2c_address_config(BSP_AUDIO_I2C, 0x72, I2C_ADDFORMAT_7BITS);
    i2c_enable(BSP_AUDIO_I2C);
}

/* 阻塞写入一段数据到 7-bit I2C 从机地址。 */
bsp_i2c_status_t bsp_i2c_write(uint8_t dev_addr_7bit, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    if((data == 0) || (len == 0U)) {
        return BSP_I2C_ERR_BUS;
    }

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_I2CBSY, RESET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }

    i2c_master_addressing(BSP_AUDIO_I2C, (uint32_t)(dev_addr_7bit << 1), I2C_MASTER_TRANSMIT);
    i2c_transfer_byte_number_config(BSP_AUDIO_I2C, (uint8_t)len);
    i2c_automatic_end_enable(BSP_AUDIO_I2C);
    I2C_STAT(BSP_AUDIO_I2C) |= I2C_STAT_TBE;
    i2c_start_on_bus(BSP_AUDIO_I2C);

    for(i = 0; i < len; i++) {
        bsp_i2c_status_t ret = i2c_wait_flag(I2C_FLAG_TI, SET);
        if(BSP_I2C_OK != ret) {
            i2c_stop_on_bus(BSP_AUDIO_I2C);
            return ret;
        }
        i2c_data_transmit(BSP_AUDIO_I2C, data[i]);
    }

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_STPDET, SET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }
    i2c_clear_stop_flag();

    return BSP_I2C_OK;
}

/* 阻塞读取一段数据。 */
bsp_i2c_status_t bsp_i2c_read(uint8_t dev_addr_7bit, uint8_t *data, uint16_t len)
{
    uint16_t i;

    if((data == 0) || (len == 0U)) {
        return BSP_I2C_ERR_BUS;
    }

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_I2CBSY, RESET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }

    i2c_master_addressing(BSP_AUDIO_I2C, (uint32_t)(dev_addr_7bit << 1), I2C_MASTER_RECEIVE);
    i2c_transfer_byte_number_config(BSP_AUDIO_I2C, (uint8_t)len);
    i2c_automatic_end_enable(BSP_AUDIO_I2C);
    i2c_start_on_bus(BSP_AUDIO_I2C);

    for(i = 0; i < len; i++) {
        bsp_i2c_status_t ret = i2c_wait_flag(I2C_FLAG_RBNE, SET);
        if(BSP_I2C_OK != ret) {
            i2c_stop_on_bus(BSP_AUDIO_I2C);
            return ret;
        }
        data[i] = (uint8_t)i2c_data_receive(BSP_AUDIO_I2C);
    }

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_STPDET, SET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }
    i2c_clear_stop_flag();

    return BSP_I2C_OK;
}

/* 写 8-bit 寄存器：先发送寄存器地址，再发送寄存器值。 */
bsp_i2c_status_t bsp_i2c_write_reg8(uint8_t dev_addr_7bit, uint8_t reg, uint8_t value)
{
    uint8_t buf[2];

    buf[0] = reg;
    buf[1] = value;

    return bsp_i2c_write(dev_addr_7bit, buf, 2U);
}

/* 读 8-bit 寄存器：写寄存器地址后，用 repeated START 读回 1 字节。 */
bsp_i2c_status_t bsp_i2c_read_reg8(uint8_t dev_addr_7bit, uint8_t reg, uint8_t *value)
{
    bsp_i2c_status_t ret;

    if(value == 0) {
        return BSP_I2C_ERR_BUS;
    }

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_I2CBSY, RESET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }

    i2c_master_addressing(BSP_AUDIO_I2C, (uint32_t)(dev_addr_7bit << 1), I2C_MASTER_TRANSMIT);
    i2c_transfer_byte_number_config(BSP_AUDIO_I2C, 1U);
    i2c_automatic_end_disable(BSP_AUDIO_I2C);
    I2C_STAT(BSP_AUDIO_I2C) |= I2C_STAT_TBE;
    i2c_start_on_bus(BSP_AUDIO_I2C);

    ret = i2c_wait_flag(I2C_FLAG_TI, SET);
    if(BSP_I2C_OK != ret) {
        i2c_stop_on_bus(BSP_AUDIO_I2C);
        return ret;
    }
    i2c_data_transmit(BSP_AUDIO_I2C, reg);

    ret = i2c_wait_flag(I2C_FLAG_TC, SET);
    if(BSP_I2C_OK != ret) {
        i2c_stop_on_bus(BSP_AUDIO_I2C);
        return ret;
    }

    i2c_master_addressing(BSP_AUDIO_I2C, (uint32_t)(dev_addr_7bit << 1), I2C_MASTER_RECEIVE);
    i2c_transfer_byte_number_config(BSP_AUDIO_I2C, 1U);
    i2c_automatic_end_enable(BSP_AUDIO_I2C);
    i2c_start_on_bus(BSP_AUDIO_I2C);

    ret = i2c_wait_flag(I2C_FLAG_RBNE, SET);
    if(BSP_I2C_OK != ret) {
        i2c_stop_on_bus(BSP_AUDIO_I2C);
        return ret;
    }
    *value = (uint8_t)i2c_data_receive(BSP_AUDIO_I2C);

    if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_STPDET, SET)) {
        return BSP_I2C_ERR_TIMEOUT;
    }
    i2c_clear_stop_flag();

    return BSP_I2C_OK;
}

/* 扫描 7-bit I2C 地址，只探测 ACK，不写 Codec 寄存器。 */
uint8_t bsp_i2c_scan(uint8_t *addr_list, uint8_t max_count)
{
    uint8_t addr;
    uint8_t found = 0U;

    if(addr_list == 0) {
        return 0U;
    }

    for(addr = 0x03U; addr < 0x78U; addr++) {
        uint32_t timeout;
        uint8_t ack = 0U;

        if(BSP_I2C_OK != i2c_wait_flag(I2C_FLAG_I2CBSY, RESET)) {
            continue;
        }

        i2c_master_addressing(BSP_AUDIO_I2C, (uint32_t)(addr << 1), I2C_MASTER_TRANSMIT);
        i2c_transfer_byte_number_config(BSP_AUDIO_I2C, 0U);
        i2c_automatic_end_enable(BSP_AUDIO_I2C);
        i2c_start_on_bus(BSP_AUDIO_I2C);

        timeout = BSP_I2C_TIMEOUT;
        while(timeout-- > 0U) {
            if(SET == i2c_flag_get(BSP_AUDIO_I2C, I2C_FLAG_NACK)) {
                i2c_flag_clear(BSP_AUDIO_I2C, I2C_FLAG_NACK);
                break;
            }
            if(SET == i2c_flag_get(BSP_AUDIO_I2C, I2C_FLAG_STPDET)) {
                ack = 1U;
                break;
            }
        }
        i2c_clear_stop_flag();

        if(ack != 0U) {
            if(found < max_count) {
                addr_list[found] = addr;
            }
            found++;
        }
    }

    return found;
}

/* 调试复位卡在 I2C 事务中间时，手动产生 9 个 SCL 释放 SDA。 */
static void i2c_bus_recover(void)
{
    uint8_t i;

    gpio_mode_set(BSP_AUDIO_I2C_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP,
                  BSP_AUDIO_I2C_SCL_PIN | BSP_AUDIO_I2C_SDA_PIN);
    gpio_output_options_set(BSP_AUDIO_I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_60MHZ,
                            BSP_AUDIO_I2C_SCL_PIN | BSP_AUDIO_I2C_SDA_PIN);
    gpio_bit_set(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SDA_PIN | BSP_AUDIO_I2C_SCL_PIN);

    for(i = 0U; i < 9U; i++) {
        gpio_bit_reset(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SCL_PIN);
        gpio_bit_set(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SCL_PIN);
    }

    gpio_bit_reset(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SDA_PIN);
    gpio_bit_set(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SCL_PIN);
    gpio_bit_set(BSP_AUDIO_I2C_GPIO_PORT, BSP_AUDIO_I2C_SDA_PIN);
}
