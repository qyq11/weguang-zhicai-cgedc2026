#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "gd32h73x_75x.h"
#include <stdint.h>

/* TIMER1 中断累加的毫秒计数，可在 Keil Watch 中观察。 */
extern volatile uint32_t g_timer1_ms_count;

/* while(1) 中按 TIMER1 tick 累加的主循环计数。 */
extern volatile uint32_t g_while_timer_count;

/* 初始化 TIMER1 为 1ms 周期中断。 */
void timer1_1ms_init(void);

/* 读取 TIMER1 毫秒计数。 */
uint32_t timer1_millis_get(void);

#endif /* BSP_TIMER_H */
