/*!
    \file    main.c
    \brief   GD32H759 工程入口：保留外设初始化，主循环只执行计数和 LED 流水灯
*/

#include "gd32h73x_75x.h"
#include "systick.h"
#include "bsp_usart.h"
#include "bsp_i2c.h"
#include "bsp_sai.h"
#include "bsp_audio_dma.h"
#include "bsp_led.h"
#include "bsp_timer.h"
#include "audio_app.h"
#include "es7210.h"
#include "es8311.h"
#include <stdio.h>

/* LED 流水灯切换周期，单位：ms。 */
#define LED_FLOW_INTERVAL_MS       200U

void cache_enable(void);
void mpu_config(void);

/* while(1) 原始循环次数，便于在 Keil Watch 中观察主循环是否仍在运行。 */
volatile uint32_t g_while = 0U;

static void board_init(void);
static void audio_link_init(void);
static void led_flow_update(uint32_t now_ms);

int main(void)
{
    uint32_t last_timer_ms;

    board_init();
    audio_link_init();

    last_timer_ms = timer1_millis_get();

    while(1) {
        uint32_t now_ms = timer1_millis_get();

        /*
         * 主循环计数：只要 while(1) 没有被阻塞，该变量就会跟随 TIMER1
         * 的毫秒 tick 增加。若 TIMER1 在跑而该值不变，说明前台循环卡住。
         */
        if(now_ms != last_timer_ms) {
            g_while_timer_count += (now_ms - last_timer_ms);
            last_timer_ms = now_ms;
        }

        /* 唯一保留的业务逻辑：非阻塞 LED1 -> LED2 -> LED3 -> LED4 流水灯。 */
        led_flow_update(now_ms);

        g_while++;
    }
}

/* 板级基础初始化：MPU/Cache、系统节拍、定时器、LED 和调试串口。 */
static void board_init(void)
{
    /*
     * Cortex-M7 建议先配置 MPU，再打开 I-Cache/D-Cache。
     * TIMER1 提供主循环计数和 LED 流水灯使用的 1ms 时间基准。
     */
    mpu_config();
    cache_enable();
    systick_config();
    timer1_1ms_init();
    bsp_led_init();

    bsp_usart0_init();
    printf("\r\nSystem start");
    printf("\r\nUSART init ok");
    printf("\r\nTIMER1 1ms init ok");
    printf("\r\nLED init ok");
}

/*
 * 音频链路初始化：保留 I2C、Codec、SAI、DMA 的初始化框架。
 * 注意：这里不再启动音频识别、提示音等业务，只保留硬件初始化过程。
 */
static void audio_link_init(void)
{
    bsp_audio_i2c_init();
    audio_app_init();
    audio_app_scan_i2c();

    /*
     * Codec 寄存器表仍是占位框架。后续拿到 ES7210/ES8311 数据手册或
     * 官方初始化表后，再在对应驱动里补真实寄存器写入。
     */
    printf("\r\nES7210 init");
    (void)es7210_init(BSP_AUDIO_SAMPLE_RATE, BSP_AUDIO_BIT_WIDTH, BSP_AUDIO_CHANNELS);

    printf("\r\nES8311 init");
    (void)es8311_init(BSP_AUDIO_SAMPLE_RATE, BSP_AUDIO_BIT_WIDTH, BSP_AUDIO_CHANNELS);

    printf("\r\nSAI0 init");
    bsp_sai0_init(BSP_AUDIO_SAMPLE_RATE, BSP_AUDIO_BIT_WIDTH, BSP_AUDIO_CHANNELS);

    printf("\r\nDMA init");
    bsp_audio_dma_init();

    /*
     * 这里仍然保留 SAI/DMA 启动，作为音频外设链路初始化的一部分；
     * 主循环中已经删除音频处理业务。
     */
    bsp_audio_dma_start();
    bsp_sai0_start();

    printf("\r\nAudio link init ok");
}

/* 按固定周期推进 LED 流水灯，函数内部不阻塞主循环。 */
static void led_flow_update(uint32_t now_ms)
{
    static uint32_t last_led_ms = 0U;
    static uint8_t led_index = 0U;
    static const led_id_t led_table[] = {LED1, LED2, LED3, LED4};

    if((now_ms - last_led_ms) < LED_FLOW_INTERVAL_MS) {
        return;
    }

    last_led_ms = now_ms;
    bsp_led_all_off();
    bsp_led_on(led_table[led_index]);

    led_index++;
    if(led_index >= (sizeof(led_table) / sizeof(led_table[0]))) {
        led_index = 0U;
    }
}

/*
 * SysTick 中断模板仍会调用 led_spark()，因此保留该符号。
 * 实际可见 LED 流水灯由 while(1) 中的 led_flow_update() 负责。
 */
void led_spark(void)
{
    static __IO uint32_t tick = 0U;

    tick++;
    if(tick >= 500U) {
        tick = 0U;
    }
}

/* 打开 Cortex-M7 指令 Cache 和数据 Cache，提高程序运行效率。 */
void cache_enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}

/* 配置 MPU 默认保护区，避免 M7 推测访问未映射区域导致总线异常。 */
void mpu_config(void)
{
    mpu_region_init_struct mpu_init_struct;

    mpu_region_struct_para_init(&mpu_init_struct);

    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);

    /*
     * 默认 4GB 空间设置为不可访问，再由特权默认映射放行正常存储区。
     * 如果后续把 DMA buffer 固定到某段 SRAM，可在此追加非 Cache 区域。
     */
    mpu_init_struct.region_base_address = 0x0;
    mpu_init_struct.region_size = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable = 0x87;
    mpu_init_struct.instruction_exec = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);
}
