#include "bsp_sai.h"

/* 根据采样率选择 MCLK 分频值。当前工程按 16kHz 调试链路配置。 */
static uint32_t sai_mclk_div_select(uint32_t sample_rate)
{
    /*
     * 音频 Codec 常用 MCLK = Fs * 256。当前 16kHz 对应目标 MCLK 约 4.096MHz。
     * 如果实测 LRCK/BCLK/MCLK 不准，需要结合实际 PLL 时钟树重新校准分频。
     */
    (void)sample_rate;
    return SAI_MCLKDIV_12;
}

/* 初始化 SAI0 相关 GPIO 引脚为复用功能。 */
void bsp_sai0_gpio_init(void)
{
    rcu_periph_clock_enable(BSP_AUDIO_SAI_GPIO_RCU);

    gpio_af_set(BSP_AUDIO_SAI_GPIO_PORT, BSP_AUDIO_SAI_GPIO_AF,
                BSP_AUDIO_SAI_MCLK_PIN | BSP_AUDIO_SAI_SD_ADC_PIN | BSP_AUDIO_SAI_FS_PIN |
                    BSP_AUDIO_SAI_SCK_PIN | BSP_AUDIO_SAI_SD_DAC_PIN);
    gpio_mode_set(BSP_AUDIO_SAI_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  BSP_AUDIO_SAI_MCLK_PIN | BSP_AUDIO_SAI_SD_ADC_PIN | BSP_AUDIO_SAI_FS_PIN |
                      BSP_AUDIO_SAI_SCK_PIN | BSP_AUDIO_SAI_SD_DAC_PIN);
    gpio_output_options_set(BSP_AUDIO_SAI_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            BSP_AUDIO_SAI_MCLK_PIN | BSP_AUDIO_SAI_SD_ADC_PIN | BSP_AUDIO_SAI_FS_PIN |
                                BSP_AUDIO_SAI_SCK_PIN | BSP_AUDIO_SAI_SD_DAC_PIN);
}

/* 初始化 SAI0：Block0 发送，Block1 接收，二者共享音频时钟。 */
void bsp_sai0_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels)
{
    sai_parameter_struct sai_cfg;
    sai_frame_parameter_struct frame_init;
    sai_slot_parameter_struct slot_init;
    uint32_t data_width;

    (void)channels;

    rcu_periph_clock_enable(BSP_AUDIO_SAI_RCU);
    rcu_sai_clock_config(IDX_SAI0, RCU_SAISRC_PLL0Q);
    bsp_sai0_gpio_init();

    sai_deinit(BSP_AUDIO_SAI);

    data_width = (bit_width == 16U) ? SAI_DATAWIDTH_16BIT : SAI_DATAWIDTH_32BIT;

    /*
     * Block0：主发送，PE6/SD0 -> ES8311 DAC。
     * Block1：同步从接收，PE3/SD1 <- ES7210 ADC。
     */
    sai_struct_para_init(&sai_cfg);
    sai_cfg.operating_mode = SAI_MASTER_TRANSMITTER;
    sai_cfg.protocol = SAI_PROTOCOL_POLYMORPHIC;
    sai_cfg.data_width = data_width;
    sai_cfg.shift_dir = SAI_SHIFT_MSB;
    sai_cfg.sample_edge = SAI_SAMPEDGE_FALLING;
    sai_cfg.sync_mode = SAI_SYNCMODE_ASYNC;
    sai_cfg.output_drive = SAI_OUTPUT_NOW;
    sai_cfg.clk_div_bypass = SAI_CLKDIV_BYPASS_OFF;
    sai_cfg.mclk_div = sai_mclk_div_select(sample_rate);
    sai_cfg.mclk_oversampling = SAI_MCLK_OVERSAMP_256;
    sai_cfg.mclk_enable = SAI_MCLK_ENABLE;
    sai_cfg.fifo_threshold = SAI_FIFOTH_HALF;
    sai_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK, &sai_cfg);

    sai_struct_para_init(&sai_cfg);
    sai_cfg.operating_mode = SAI_SLAVE_RECEIVER;
    sai_cfg.protocol = SAI_PROTOCOL_POLYMORPHIC;
    sai_cfg.data_width = data_width;
    sai_cfg.shift_dir = SAI_SHIFT_MSB;
    sai_cfg.sample_edge = SAI_SAMPEDGE_FALLING;
    sai_cfg.sync_mode = SAI_SYNCMODE_OTHERBLOCK;
    sai_cfg.output_drive = SAI_OUTPUT_WITH_SAIEN;
    sai_cfg.clk_div_bypass = SAI_CLKDIV_BYPASS_OFF;
    sai_cfg.mclk_div = SAI_MCLKDIV_1;
    sai_cfg.mclk_oversampling = SAI_MCLK_OVERSAMP_256;
    sai_cfg.mclk_enable = SAI_MCLK_DISABLE;
    sai_cfg.fifo_threshold = SAI_FIFOTH_HALF;
    sai_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK, &sai_cfg);

    sai_frame_struct_para_init(&frame_init);
    frame_init.frame_width = 32U;           /* 双声道帧：2 * 16bit */
    frame_init.frame_sync_width = 16U;      /* LRCK 半帧宽度 */
    frame_init.frame_sync_function = SAI_FS_FUNC_START_CHANNEL;
    frame_init.frame_sync_polarity = SAI_FS_POLARITY_LOW;
    frame_init.frame_sync_offset = SAI_FS_OFFSET_BEGINNING;
    sai_frame_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK, &frame_init);
    sai_frame_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK, &frame_init);

    sai_slot_struct_para_init(&slot_init);
    slot_init.slot_number = 2U;
    slot_init.slot_width = SAI_SLOT_WIDTH_16BIT;
    slot_init.data_offset = 0U;
    slot_init.slot_active = SAI_SLOT_ACTIVE_0 | SAI_SLOT_ACTIVE_1;
    sai_slot_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK, &slot_init);
    sai_slot_init(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK, &slot_init);

    sai_fifo_flush(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
    sai_fifo_flush(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
}

/* 启动 SAI DMA 请求并使能 RX/TX Block。 */
void bsp_sai0_start(void)
{
    sai_dma_enable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
    sai_dma_enable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
    sai_enable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
    sai_enable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
}

/* 停止 SAI RX/TX Block 并关闭 DMA 请求。 */
void bsp_sai0_stop(void)
{
    sai_disable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
    sai_disable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
    sai_dma_disable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
    sai_dma_disable(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
}
