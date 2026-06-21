#ifndef BSP_AUDIO_DMA_H
#define BSP_AUDIO_DMA_H

#include "gd32h73x_75x.h"
#include <stdint.h>

/* 单帧音频样本数；DMA 使用前半/后半缓冲区轮流传输。 */
#define AUDIO_FRAME_SAMPLES        320U
#define AUDIO_CHANNELS             2U
#define AUDIO_DMA_BUFFER_SAMPLES   (AUDIO_FRAME_SAMPLES * AUDIO_CHANNELS * 2U)

/* RX DMA：SAI0 Block1 接收 ADC 数据到内存。 */
#define AUDIO_DMA_RX_PERIPH        DMA0
#define AUDIO_DMA_RX_CHANNEL       DMA_CH0
#define AUDIO_DMA_RX_IRQ           DMA0_Channel0_IRQn
#define AUDIO_DMA_RX_REQUEST       DMA_REQUEST_SAI0_B1

/* TX DMA：内存数据发送到 SAI0 Block0，用于 DAC 输出。 */
#define AUDIO_DMA_TX_PERIPH        DMA0
#define AUDIO_DMA_TX_CHANNEL       DMA_CH1
#define AUDIO_DMA_TX_IRQ           DMA0_Channel1_IRQn
#define AUDIO_DMA_TX_REQUEST       DMA_REQUEST_SAI0_B0

/* DMA 中断和调试观察用的共享状态。 */
extern volatile uint8_t audio_frame_ready;
extern volatile uint8_t audio_frame_index;
extern int16_t audio_rx_buffer[AUDIO_DMA_BUFFER_SAMPLES];
extern int16_t audio_tx_buffer[AUDIO_DMA_BUFFER_SAMPLES];

/* 音频 DMA 初始化和启停接口。 */
void bsp_audio_dma_init(void);
void bsp_audio_dma_start(void);
void bsp_audio_dma_stop(void);

/* 初始化辅助接口：填充 TX 缓冲区并维护 M7 D-Cache 一致性。 */
void bsp_audio_dma_fill_tx_sine_1khz(void);
void bsp_audio_dma_cache_invalidate_rx(void);
void bsp_audio_dma_cache_clean_tx(void);

#endif /* BSP_AUDIO_DMA_H */
