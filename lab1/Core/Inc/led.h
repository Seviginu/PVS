#ifndef LED_H
#define LED_H

enum { LED_GREEN = 1, LED_YELLOW = 2, LED_RED = 4 };

void led_init(void);
void led_set(unsigned leds);

#endif
