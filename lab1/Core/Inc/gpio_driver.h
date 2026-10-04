#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "stm32f4xx.h"

enum { GPIO_INPUT, GPIO_OUTPUT, GPIO_ALTERNATE, GPIO_ANALOG };
enum { GPIO_NO_PULL, GPIO_PULL_UP, GPIO_PULL_DOWN };
enum { GPIO_PUSH_PULL, GPIO_OPEN_DRAIN };
enum { GPIO_LOW_SPEED, GPIO_MEDIUM_SPEED, GPIO_DRIVER_HIGH_SPEED, GPIO_VERY_HIGH_SPEED };

typedef struct {
    uint32_t mode;
    uint32_t pull;
    uint32_t type;
    uint32_t speed;
    uint32_t alternate;
} GpioConfig;

/* pins и values — маски; для одного контакта передаём 1u << номер. */
void gpio_init(GPIO_TypeDef *port, uint16_t pins, GpioConfig config, uint16_t values);
GpioConfig gpio_get_config(GPIO_TypeDef *port, unsigned pin);
uint16_t gpio_read(GPIO_TypeDef *port, uint16_t pins);
void gpio_write(GPIO_TypeDef *port, uint16_t pins, uint16_t values);

#endif
