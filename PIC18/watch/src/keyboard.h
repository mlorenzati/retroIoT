#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <htc.h>
#include <stddef.h>

typedef enum {
    key_inactive = 0,
    key_pressed  = 1,
    key_repeat   = 2,
    key_released = 3
} key_value;

typedef struct key_status {
    unsigned int pressed:1;
    unsigned int repeating:1;
    unsigned int count:6;
    unsigned int repeat_countdown:8;
} key_status_t; 

typedef struct keyboard_status {
    unsigned int key1:2;
    unsigned int key2:2;
    unsigned int key3:2;
    unsigned int key4:2;
    unsigned int key5:2;
    unsigned int key6:2;
    unsigned int unused:4;
} keyboard_status_t;

typedef union {
    keyboard_status_t keys;
    unsigned int raw;
} keyboard_status_union_t;

#define KEYBOARD_PIN_DIRECTION  TRISA0
#define KEYBOARD_PIN_VALUE      RA0
#define KEYBOARD_THRESHOLD 5
#define KEY_INDEX_MAX 6
#define KEY_COUNT_INIT { 0, 0, 0, 0, 0, 0 }

key_value get_key_from(keyboard_status_t keys, char key_idx);

typedef void (*keyboard_callback)(keyboard_status_t);

void keyboard_init(const char *key_idx, char repeat_rate_5ms, char repeat_rate_acel, keyboard_callback callback);
void keyboard_event_loop(void);
keyboard_status_t get_last_keyboard_status(void);

#endif