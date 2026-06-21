#include "bsp_audio_dma.h"
#include "bsp_sai.h"

#if defined(__CC_ARM)
/* ARM Compiler 5 的 32 字节对齐写法。 */
#define DMA_ALIGN_32 __align(32)
#else
/* ARM Compiler 6 / GCC 的 32 字节对齐写法。 */
#define DMA_ALIGN_32 __attribute__((aligned(32)))
#endif

/* RX DMA 半传输/全传输中断置位，用于调试观察当前音频帧状态。 */
volatile uint8_t audio_frame_ready = 0U;
volatile uint8_t audio_frame_index = 0U;

/* M7 D-Cache 按 cache line 维护，DMA 缓冲区按 32 字节对齐更稳妥。 */
DMA_ALIGN_32 int16_t audio_rx_buffer[AUDIO_DMA_BUFFER_SAMPLES];
DMA_ALIGN_32 int16_t audio_tx_buffer[AUDIO_DMA_BUFFER_SAMPLES];

/* 16kHz 采样率下，16 个点循环一次得到 1kHz 正弦测试数据。 */
static const int16_t sine_1khz_16k[16] = {
    0, 6269, 11585, 15136, 16384, 15136, 11585, 6269,
    0, -6269, -11585, -15136, -16384, -15136, -11585, -6269
};

/* 配置 RX DMA 通道：SAI 接收寄存器 -> audio_rx_buffer。 */
static void dma_config_rx(void)
{
    dma_single_data_parameter_struct dma_init;

    dma_deinit(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL);
    dma_single_data_para_struct_init(&dma_init);
    dma_init.request = AUDIO_DMA_RX_REQUEST;
    dma_init.periph_addr = (uint32_t)&SAI_DATA(BSP_AUDIO_SAI, BSP_AUDIO_SAI_RX_BLOCK);
    dma_init.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init.memory0_addr = (uint32_t)audio_rx_buffer;
    dma_init.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_init.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_init.direction = DMA_PERIPH_TO_MEMORY;
    dma_init.number = AUDIO_DMA_BUFFER_SAMPLES;
    dma_init.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, &dma_init);

    dma_interrupt_enable(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_HTF | DMA_INT_FTF | DMA_INT_TAE);
    nvic_irq_enable(AUDIO_DMA_RX_IRQ, 1U, 0U);
}

/* 配置 TX DMA 通道：audio_tx_buffer -> SAI 发送寄存器。 */
static void dma_config_tx(void)
{
    dma_single_data_parameter_struct dma_init;

    dma_deinit(AUDIO_DMA_TX_PERIPH, AUDIO_DMA_TX_CHANNEL);
    dma_single_data_para_struct_init(&dma_init);
    dma_init.request = AUDIO_DMA_TX_REQUEST;
    dma_init.periph_addr = (uint32_t)&SAI_DATA(BSP_AUDIO_SAI, BSP_AUDIO_SAI_TX_BLOCK);
    dma_init.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init.memory0_addr = (uint32_t)audio_tx_buffer;
    dma_init.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_init.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_init.direction = DMA_MEMORY_TO_PERIPH;
    dma_init.number = AUDIO_DMA_BUFFER_SAMPLES;
    dma_init.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(AUDIO_DMA_TX_PERIPH, AUDIO_DMA_TX_CHANNEL, &dma_init);

    nvic_irq_enable(AUDIO_DMA_TX_IRQ, 1U, 1U);
}

/* 初始化 DMA 时钟、缓冲区初值以及 RX/TX 两个 DMA 通道。 */
void bsp_audio_dma_init(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    audio_frame_ready = 0U;
    audio_frame_index = 0U;
    bsp_audio_dma_fill_tx_sine_1khz();
    bsp_audio_dma_cache_clean_tx();

    dma_config_rx();
    dma_config_tx();
}

/* 启动音频 RX/TX DMA 通道。 */
void bsp_audio_dma_start(void)
{
    dma_channel_enable(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL);
    dma_channel_enable(AUDIO_DMA_TX_PERIPH, AUDIO_DMA_TX_CHANNEL);
}

/* 停止音频 RX/TX DMA 通道。 */
void bsp_audio_dma_stop(void)
{
    dma_channel_disable(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL);
    dma_channel_disable(AUDIO_DMA_TX_PERIPH, AUDIO_DMA_TX_CHANNEL);
}

/* 填充 TX 缓冲区，作为音频链路初始化时的默认测试数据。 */
void bsp_audio_dma_fill_tx_sine_1khz(void)
{
    uint32_t i;

    for(i = 0U; i < AUDIO_DMA_BUFFER_SAMPLES; i += 2U) {
        int16_t sample = sine_1khz_16k[(i / 2U) & 0x0FU];
        audio_tx_buffer[i] = sample;
        audio_tx_buffer[i + 1U] = sample;
    }
}

/* RX 缓冲区由 DMA 写入，CPU 读取前需要让 D-Cache 失效。 */
void bsp_audio_dma_cache_invalidate_rx(void)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)audio_rx_buffer, sizeof(audio_rx_buffer));
}

/* TX 缓冲区由 CPU 写入，DMA 读取前需要清理 D-Cache。 */
void bsp_audio_dma_cache_clean_tx(void)
{
    SCB_CleanDCache_by_Addr((uint32_t *)audio_tx_buffer, sizeof(audio_tx_buffer));
}

/* RX DMA 中断：记录当前完成的是前半缓冲区还是后半缓冲区。 */
void DMA0_Channel0_IRQHandler(void)
{
    if(SET == dma_interrupt_flag_get(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_HTF)) {
        dma_interrupt_flag_clear(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_HTF);
        audio_frame_index = 0U;
        audio_frame_ready = 1U;
    }

    if(SET == dma_interrupt_flag_get(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_FTF);
        audio_frame_index = 1U;
        audio_frame_ready = 1U;
    }

    if(SET == dma_interrupt_flag_get(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_TAE)) {
        dma_interrupt_flag_clear(AUDIO_DMA_RX_PERIPH, AUDIO_DMA_RX_CHANNEL, DMA_INT_FLAG_TAE);
    }
}

/* TX DMA 中断：当前只清除中断标志。 */
void DMA0_Channel1_IRQHandler(void)
{
    dma_interrupt_flag_clear(AUDIO_DMA_TX_PERIPH, AUDIO_DMA_TX_CHANNEL,
                             DMA_INT_FLAG_HTF | DMA_INT_FLAG_FTF | DMA_INT_FLAG_TAE);
}
