#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

enum { BUTTON_NONE, BUTTON_PRESSED, BUTTON_RELEASED };

void button_init(void);
int button_update(uint32_t now);
bool button_down(void);

#endif
