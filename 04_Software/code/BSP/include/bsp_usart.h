#ifndef BSP_USART_H
#define BSP_USART_H

#include "gd32h73x_75x.h"
#include <stdint.h>

/* 调试串口：USART0_TX = PF4，USART0_RX = PF5，115200-8-N-1。 */
#define BSP_USART                 USART0
#define BSP_USART_RCU             RCU_USART0
#define BSP_USART_GPIO_RCU        RCU_GPIOF
#define BSP_USART_GPIO_PORT       GPIOF
#define BSP_USART_TX_PIN          GPIO_PIN_4
#define BSP_USART_RX_PIN          GPIO_PIN_5
#define BSP_USART_GPIO_AF         GPIO_AF_7
#define BSP_USART_BAUDRATE        115200U

/* 初始化 USART0 并提供 printf 底层发送接口。 */
void bsp_usart0_init(void);
void bsp_usart0_send_byte(uint8_t data);

#endif /* BSP_USART_H */
