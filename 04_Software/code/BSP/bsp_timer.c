#include "bsp_timer.h"

volatile uint32_t g_timer1_ms_count = 0U;
volatile uint32_t g_while_timer_count = 0U;

/* 初始化 TIMER1，使其每 1ms 产生一次更新中断。 */
void timer1_1ms_init(void)
{
    timer_parameter_struct timer_initpara;
    uint32_t timer_clock_hz;
    uint32_t prescaler;

    rcu_periph_clock_enable(RCU_TIMER1);

    /*
     * TIMER1 挂在 APB1。这里先将 APB1 时钟分频到 10kHz，
     * 再用 10 个计数产生一次 update 中断，目标周期约为 1ms。
     */
    timer_clock_hz = rcu_clock_freq_get(CK_APB1);
    prescaler = (timer_clock_hz / 10000U);
    if(prescaler == 0U) {
        prescaler = 1U;
    }
    prescaler -= 1U;
    if(prescaler > 0xFFFFU) {
        prescaler = 0xFFFFU;
    }

    timer_deinit(TIMER1);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler = (uint16_t)prescaler;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 10U - 1U;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0U;
    timer_init(TIMER1, &timer_initpara);

    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    nvic_irq_enable(TIMER1_IRQn, 2U, 0U);

    g_timer1_ms_count = 0U;
    g_while_timer_count = 0U;
    timer_enable(TIMER1);
}

/* 返回当前 TIMER1 毫秒计数。 */
uint32_t timer1_millis_get(void)
{
    return g_timer1_ms_count;
}

/* TIMER1 中断服务函数：清除更新中断标志并累加 1ms tick。 */
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP)) {
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
        g_timer1_ms_count++;
    }
}
