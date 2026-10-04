#include "gpio_driver.h"

void gpio_write(GPIO_TypeDef *port, uint16_t pins, uint16_t values)
{
    port->BSRR = (values & pins) | ((uint32_t)(pins & ~values) << 16);
}

uint16_t gpio_read(GPIO_TypeDef *port, uint16_t pins)
{
    return port->IDR & pins;
}

void gpio_init(GPIO_TypeDef *port, uint16_t pins, GpioConfig config, uint16_t values)
{
    /* GPIOA..GPIOI расположены с шагом 0x400; биты тактирования идут подряд. */
    unsigned index = ((uintptr_t)port - GPIOA_BASE) / 0x400u;
    RCC->AHB1ENR |= 1u << index;
    (void)RCC->AHB1ENR;

    uint32_t mask = 0;
    uint32_t disconnect = 0;
    for (unsigned pin = 0; pin < 16; pin++) {
        if (pins & (1u << pin)) {
            mask |= 3u << (2 * pin);
            unsigned old_mode = (port->MODER >> (2 * pin)) & 3u;
            if (old_mode != GPIO_OUTPUT || config.mode != GPIO_OUTPUT) {
                disconnect |= 3u << (2 * pin);
            }
        }
    }

    /* При выход -> выход сохраняем управление линией.
       При смене режима отключаем старый источник, ODR задаём до включения выхода. */
    port->MODER &= ~disconnect;
    gpio_write(port, pins, values);

    uint32_t modes = 0;
    for (unsigned pin = 0; pin < 16; pin++) {
        if (!(pins & (1u << pin))) {
            continue;
        }
        unsigned shift = 2 * pin;
        unsigned af_shift = 4 * (pin % 8);
        port->OSPEEDR = (port->OSPEEDR & ~(3u << shift)) | (config.speed << shift);
        port->PUPDR = (port->PUPDR & ~(3u << shift)) | (config.pull << shift);
        port->OTYPER = (port->OTYPER & ~(1u << pin)) | (config.type << pin);
        port->AFR[pin / 8] = (port->AFR[pin / 8] & ~(15u << af_shift))
                          | (config.alternate << af_shift);
        modes |= config.mode << shift;
    }
    port->MODER = (port->MODER & ~mask) | modes;
}

GpioConfig gpio_get_config(GPIO_TypeDef *port, unsigned pin)
{
    GpioConfig config = {
        .mode = (port->MODER >> (2 * pin)) & 3u,
        .pull = (port->PUPDR >> (2 * pin)) & 3u,
        .type = (port->OTYPER >> pin) & 1u,
        .speed = (port->OSPEEDR >> (2 * pin)) & 3u,
        .alternate = (port->AFR[pin / 8] >> (4 * (pin % 8))) & 15u
    };
    return config;
}
