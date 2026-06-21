#ifndef GD32H73X_75X_IT_H
#define GD32H73X_75X_IT_H

#include "gd32h73x_75x.h"

/* Cortex-M 异常和外设中断服务函数声明。 */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void FPU_IRQHandler(void);
void SysTick_Handler(void);

#endif /* GD32H73X_75X_IT_H */
