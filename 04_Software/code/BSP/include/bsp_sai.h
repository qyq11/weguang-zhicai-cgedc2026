#ifndef BSP_SAI_H
#define BSP_SAI_H

#include "gd32h73x_75x.h"
#include <stdint.h>

/* 音频链路统一使用的采样率、位宽和声道数。 */
#define BSP_AUDIO_SAMPLE_RATE       16000U
#define BSP_AUDIO_BIT_WIDTH         16U
#define BSP_AUDIO_CHANNELS          2U

/* SAI0 外设和 GPIO 映射。 */
#define BSP_AUDIO_SAI               SAI0
#define BSP_AUDIO_SAI_RCU           RCU_SAI0
#define BSP_AUDIO_SAI_GPIO_RCU      RCU_GPIOE
#define BSP_AUDIO_SAI_GPIO_PORT     GPIOE
#define BSP_AUDIO_SAI_GPIO_AF       GPIO_AF_6

/* SAI0 引脚必须配置为复用功能。 */
#define BSP_AUDIO_SAI_MCLK_PIN      GPIO_PIN_2  /* PE2 -> SAI0_MCLK0 */
#define BSP_AUDIO_SAI_SD_ADC_PIN    GPIO_PIN_3  /* PE3 -> SAI0_SD1，ADC 输入 */
#define BSP_AUDIO_SAI_FS_PIN        GPIO_PIN_4  /* PE4 -> SAI0_FS0 */
#define BSP_AUDIO_SAI_SCK_PIN       GPIO_PIN_5  /* PE5 -> SAI0_SCK0 */
#define BSP_AUDIO_SAI_SD_DAC_PIN    GPIO_PIN_6  /* PE6 -> SAI0_SD0，DAC 输出 */

#define BSP_AUDIO_SAI_TX_BLOCK      SAI_BLOCK0
#define BSP_AUDIO_SAI_RX_BLOCK      SAI_BLOCK1

/* SAI GPIO、外设参数初始化和启停接口。 */
void bsp_sai0_gpio_init(void);
void bsp_sai0_init(uint32_t sample_rate, uint8_t bit_width, uint8_t channels);
void bsp_sai0_start(void);
void bsp_sai0_stop(void);

#endif /* BSP_SAI_H */
