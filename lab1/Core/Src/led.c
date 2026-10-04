#include "led.h"
#include "gpio_driver.h"

#define LED_PINS ((1u << 13) | (1u << 14) | (1u << 15))

void led_init(void)
{
    GpioConfig config = {
        .mode = GPIO_OUTPUT,
        .type = GPIO_PUSH_PULL,
        .speed = GPIO_LOW_SPEED,
        .pull = GPIO_NO_PULL
    };
    gpio_init(GPIOD, LED_PINS, config, 0);
}

void led_set(unsigned leds)
{
    gpio_write(GPIOD, LED_PINS, (leds & 7u) << 13);
}
