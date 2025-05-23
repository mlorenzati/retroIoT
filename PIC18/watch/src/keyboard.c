#include "keyboard.h"
#include "timer.h"
#include <stdbool.h>

const char *key_index = NULL;
key_status_t keys_stats[KEY_INDEX_MAX];
keyboard_callback keys_callback;
bool key_changed = false;
bool keys_changed = false;
char repeat_rate_5ms;
char repeat_rate_acel;

void keyboard_handler(void) {
    // Called periodically by a fast IRQ timer
    if (key_index == NULL) {
        return;
    }

    // Read input pin, aligned to the scan index
    bool value = !KEYBOARD_PIN_VALUE; // Pressed is LOW
    key_status_t *stat = &keys_stats[*key_index];

    // Update count with hysteresis
    if (value) {
        if (stat->count < KEYBOARD_THRESHOLD) {
            stat->count++;
        }
    } else {
        if (stat->count > 0) {
            stat->count--;
        }
    }

    // Detect change if count reaches stable limits
    if (value != stat->pressed) {
        if (value && stat->count == KEYBOARD_THRESHOLD) {
            stat->pressed = value;
            key_changed = true;

            // Prepare the repeat key
            stat->repeat_countdown = repeat_rate_5ms;
        } else if (!value && stat->count == 0) {
            stat->pressed = value;
            key_changed = true;
            stat->repeat_countdown = 0;
        }
    } else if (stat->repeat_countdown > 0) {
        if (--(stat->repeat_countdown) == 0) {
            // Key still pressed repeat countdown, pending decrease time
            stat->repeating = true;
            key_changed = true;
            stat->repeat_countdown = repeat_rate_5ms; 
        }
    }

    // At end of scan cycle, propagate changes
    if (key_changed && (*key_index == KEY_INDEX_MAX - 1)) {
        key_changed = false;
        keys_changed = true;
    }
}

key_value get_key_from(keyboard_status_t keys, char key_idx) {
    keyboard_status_union_t keys_union;
	keys_union.keys = keys;
    return  (key_value) ((keys_union.raw >> (2 * key_idx)) & 0b00000011);
}

keyboard_status_t last_keyboard_status;
keyboard_status_t get_last_keyboard_status(void) {
    return last_keyboard_status;
}

void keyboard_init(const char *key_idx, char _repeat_rate_5ms, char _repeat_rate_acel, keyboard_callback callback) {
    // Configure I/O Ports
    KEYBOARD_PIN_DIRECTION = 1;

    // Define shared display index
    key_index = key_idx;

    // Register on key event callback
    keys_callback = callback;

    // Initialize keys register
    key_status_t keystatus = { 0, 0, 0, 0};

    for (int idx = 0; idx < KEY_INDEX_MAX; idx++) {
        keys_stats[idx] = keystatus;
    }

    // Set keyboard rates
    repeat_rate_5ms = _repeat_rate_5ms;
    repeat_rate_acel = _repeat_rate_acel;

    // Register interrupt handler
    timer0_register_callback(keyboard_handler);
}

void keyboard_event_loop(void) {
    if (keys_changed) {
        keys_changed = false;
        // There are key changes to be processed}
        keyboard_status_union_t new_keyboard_status;
        keyboard_status_union_t old_keyboard_status;
        old_keyboard_status.keys = last_keyboard_status;
        new_keyboard_status.raw = 0;

        for (int idx = 0; idx < KEY_INDEX_MAX; idx++) {
            key_status_t *key_stats = &(keys_stats[idx]);
            key_value old_value = (old_keyboard_status.raw >> (2 * idx)) & 0x3;
            key_value value = key_stats->repeating ? 
                key_repeat : 
                key_stats->pressed ? 
                    key_pressed :
                    (old_value == key_pressed || old_value == key_repeat) ? key_released : key_inactive;
            
            unsigned int key_stats_raw = value << (2 * idx);
            new_keyboard_status.raw |= key_stats_raw;
        }
        last_keyboard_status = new_keyboard_status.keys;
        
        // Call to key event subscriptor
        keys_callback(last_keyboard_status);
    }
}