#include "gd32h73x_75x_it.h"
#include "main.h"
#include "systick.h"

/* NMI 异常处理：当前工程进入死循环，便于调试定位。 */
void NMI_Handler(void)
{
    while(1) {
    }
}

/* HardFault 异常处理。 */
void HardFault_Handler(void)
{
    while(1) {
    }
}

/* MemManage 异常处理。 */
void MemManage_Handler(void)
{
    while(1) {
    }
}

/* BusFault 异常处理。 */
void BusFault_Handler(void)
{
    while(1) {
    }
}

/* UsageFault 异常处理。 */
void UsageFault_Handler(void)
{
    while(1) {
    }
}

/* DebugMon 异常处理。 */
void DebugMon_Handler(void)
{
    while(1) {
    }
}

/* SVC 异常处理。 */
void SVC_Handler(void)
{
    while(1) {
    }
}

/* PendSV 异常处理。 */
void PendSV_Handler(void)
{
    while(1) {
    }
}

/* FPU 异常处理。 */
void FPU_IRQHandler(void)
{
    while(1) {
    }
}

/* SysTick 中断：保留模板心跳入口，并维护 delay_1ms() 计数。 */
void SysTick_Handler(void)
{
    led_spark();
    delay_decrement();
}
