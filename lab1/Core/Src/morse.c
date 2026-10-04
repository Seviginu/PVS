#include "morse.h"
#include "button.h"
#include "led.h"

#define MAX_SIGNALS 8
#define LONG_PRESS_MS 200u
#define INPUT_PAUSE_MS 800u
#define FEEDBACK_MS 100u
#define DOT_MS 200u
#define DASH_MS 600u
#define GAP_MS 200u

enum { INPUT, SEND_ON, SEND_GAP };

static int state = INPUT;
static bool signals[MAX_SIGNALS]; /* false — точка, true — тире */
static unsigned count;
static unsigned position;
static bool holding;
static bool feedback;
static uint32_t pressed_at;
static uint32_t released_at;
static uint32_t feedback_at;
static uint32_t switched_at;

void morse_update(uint32_t now)
{
    int event = button_update(now);

    if (state == INPUT) {
        if (feedback && (uint32_t)(now - feedback_at) >= FEEDBACK_MS) {
            led_set(0);
            feedback = false;
        }
        if (event == BUTTON_PRESSED) {
            holding = true;
            pressed_at = now;
        }
        if (event == BUTTON_RELEASED) {
            released_at = now;
            if (holding && count < MAX_SIGNALS) {
                bool dash = (uint32_t)(now - pressed_at) >= LONG_PRESS_MS;
                signals[count++] = dash;
                led_set(dash ? LED_RED : LED_YELLOW);
                feedback = true;
                feedback_at = now;
            }
            holding = false;
        }
        if (count && !button_down() &&
            (uint32_t)(now - released_at) >= INPUT_PAUSE_MS) {
            position = 0;
            state = SEND_ON;
            switched_at = now;
            feedback = false;
            led_set(LED_GREEN);
        }
        return;
    }

    /* Во время передачи кнопка опрашивается, но новые сигналы не записываются. */
    if (state == SEND_ON) {
        uint32_t duration = signals[position] ? DASH_MS : DOT_MS;
        if ((uint32_t)(now - switched_at) >= duration) {
            led_set(0);
            switched_at = now;
            position++;
            if (position == count) {
                count = 0;
                holding = false;
                state = INPUT;
            } else {
                state = SEND_GAP;
            }
        }
    } else if ((uint32_t)(now - switched_at) >= GAP_MS) {
        led_set(LED_GREEN);
        switched_at = now;
        state = SEND_ON;
    }
}
