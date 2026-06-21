#include "bsp_usart.h"
#include <stdio.h>

/* 初始化 USART0 的 GPIO、波特率、收发功能。 */
void bsp_usart0_init(void)
{
    rcu_periph_clock_enable(BSP_USART_GPIO_RCU);
    rcu_periph_clock_enable(BSP_USART_RCU);

    gpio_af_set(BSP_USART_GPIO_PORT, BSP_USART_GPIO_AF, BSP_USART_TX_PIN | BSP_USART_RX_PIN);
    gpio_mode_set(BSP_USART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, BSP_USART_TX_PIN | BSP_USART_RX_PIN);
    gpio_output_options_set(BSP_USART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ,
                            BSP_USART_TX_PIN | BSP_USART_RX_PIN);

    usart_deinit(BSP_USART);
    usart_baudrate_set(BSP_USART, BSP_USART_BAUDRATE);
    usart_receive_config(BSP_USART, USART_RECEIVE_ENABLE);
    usart_transmit_config(BSP_USART, USART_TRANSMIT_ENABLE);
    usart_enable(BSP_USART);
}

/* 阻塞发送 1 字节，用作 printf 重定向的最底层接口。 */
void bsp_usart0_send_byte(uint8_t data)
{
    usart_data_transmit(BSP_USART, data);
    while(RESET == usart_flag_get(BSP_USART, USART_FLAG_TBE)) {
    }
}

#if defined(__GNUC__) && !defined(__clang__)
__attribute__((weak)) int __io_putchar(int ch)
{
    bsp_usart0_send_byte((uint8_t)ch);
    return ch;
}
#else
/* Keil/ARMCC 的 printf 最终会调用 fputc。 */
__WEAK int fputc(int ch, FILE *f)
{
    (void)f;
    bsp_usart0_send_byte((uint8_t)ch);
    return ch;
}
#endif
