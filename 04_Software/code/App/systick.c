#include "gd32h73x_75x.h"
#include "systick.h"

/* delay_1ms() 使用的倒计时变量，由 SysTick 中断递减。 */
volatile static uint32_t delay;

/* 初始化 SysTick，使其每 1ms 进入一次中断。 */
void systick_config(void)
{
    if(SysTick_Config(SystemCoreClock / 1000U)) {
        while(1) {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/* 阻塞延时 count 毫秒。 */
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay) {
    }
}

/* SysTick 中断调用，每 1ms 递减一次延时计数。 */
void delay_decrement(void)
{
    if(0U != delay) {
        delay--;
    }
}
