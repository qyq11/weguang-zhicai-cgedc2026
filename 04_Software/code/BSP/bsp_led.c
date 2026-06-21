#include "bsp_led.h"

/* 四个用户 LED 连接在 GPIOC 的 9~12 引脚。 */
#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_CLK        RCU_GPIOC

#define LED1_PIN            GPIO_PIN_9
#define LED2_PIN            GPIO_PIN_10
#define LED3_PIN            GPIO_PIN_11
#define LED4_PIN            GPIO_PIN_12

#define LED_ALL_PIN         (LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN)

/* 将逻辑 LED 编号转换为实际 GPIO 引脚。 */
static uint32_t bsp_led_get_pin(led_id_t led)
{
    switch(led)
    {
        case LED1:
            return LED1_PIN;

        case LED2:
            return LED2_PIN;

        case LED3:
            return LED3_PIN;

        case LED4:
            return LED4_PIN;

        default:
            return 0U;
    }
}

/* 初始化 LED 对应的 GPIO 时钟、模式和输出类型。 */
void bsp_led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLK);

    gpio_mode_set(LED_GPIO_PORT,
                  GPIO_MODE_OUTPUT,
                  GPIO_PUPD_NONE,
                  LED_ALL_PIN);

    gpio_output_options_set(LED_GPIO_PORT,
                            GPIO_OTYPE_PP,
                            GPIO_OSPEED_60MHZ,
                            LED_ALL_PIN);

    bsp_led_all_off();
}

/* 点亮指定 LED。 */
void bsp_led_on(led_id_t led)
{
    uint32_t pin = bsp_led_get_pin(led);

    if(pin != 0U)
    {
        gpio_bit_set(LED_GPIO_PORT, pin);
    }
}

/* 熄灭指定 LED。 */
void bsp_led_off(led_id_t led)
{
    uint32_t pin = bsp_led_get_pin(led);

    if(pin != 0U)
    {
        gpio_bit_reset(LED_GPIO_PORT, pin);
    }
}

/* 翻转指定 LED 当前状态。 */
void bsp_led_toggle(led_id_t led)
{
    uint32_t pin = bsp_led_get_pin(led);

    if(pin != 0U)
    {
        gpio_bit_toggle(LED_GPIO_PORT, pin);
    }
}

/* 点亮全部 LED。 */
void bsp_led_all_on(void)
{
    gpio_bit_set(LED_GPIO_PORT, LED_ALL_PIN);
}

/* 熄灭全部 LED。 */
void bsp_led_all_off(void)
{
    gpio_bit_reset(LED_GPIO_PORT, LED_ALL_PIN);
}
