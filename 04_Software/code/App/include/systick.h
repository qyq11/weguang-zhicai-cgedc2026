#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* 初始化 SysTick 为 1ms 中断。 */
void systick_config(void);

/* 毫秒级阻塞延时。 */
void delay_1ms(uint32_t count);

/* SysTick 中断中调用，用于递减延时计数。 */
void delay_decrement(void);

#endif /* SYSTICK_H */
