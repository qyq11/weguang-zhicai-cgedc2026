#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "gd32h73x_75x.h"

/* 板载 LED 的逻辑编号。 */
typedef enum
{
    LED1 = 0,
    LED2,
    LED3,
    LED4
} led_id_t;

/* 初始化 LED GPIO。 */
void bsp_led_init(void);

/* 单个 LED 控制接口。 */
void bsp_led_on(led_id_t led);
void bsp_led_off(led_id_t led);
void bsp_led_toggle(led_id_t led);

/* 所有 LED 同时控制接口。 */
void bsp_led_all_on(void);
void bsp_led_all_off(void);

#endif
