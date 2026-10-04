#include "button.h"
#include "gpio_driver.h"

#define BUTTON_PIN (1u << 15)
#define DEBOUNCE_MS 30u

static bool raw;
static bool pressed;
static uint32_t changed_at;

void button_init(void)
{
    GpioConfig config = { .mode = GPIO_INPUT, .pull = GPIO_NO_PULL };
    gpio_init(GPIOC, BUTTON_PIN, config, 0);
    raw = false;
    pressed = false;
    changed_at = 0;
}

int button_update(uint32_t now)
{
    bool value = gpio_read(GPIOC, BUTTON_PIN) == 0;
    if (value != raw) {
        raw = value;
        changed_at = now;
    }
    if (raw != pressed && (uint32_t)(now - changed_at) >= DEBOUNCE_MS) {
        pressed = raw;
        return pressed ? BUTTON_PRESSED : BUTTON_RELEASED;
    }
    return BUTTON_NONE;
}

bool button_down(void)
{
    /* Учитываем и ещё не подтверждённое нажатие: оно прерывает паузу ввода. */
    return pressed || raw;
}
