#ifndef MENU_H
#define MENU_H

typedef enum {
    MENU_STATE_NONE = 0,
    MENU_STATE_STARTUP,
    MENU_STATE_CLOCK,
    MENU_STATE_ALARM_CONFIG,
    MENU_STATE_ALARM,
    MENU_STATE_COUNT
} menu_state;

typedef enum {
    MENU_EVENT_NONE = 0,
    MENU_EVENT_GENERIC,
    MENU_EVENT_ALARM_TRIGGERED,
    MENU_EVENT_KEY_PRESSED,
    MENU_EVENT_COUNT
} menu_event;

void menu_init(menu_state mode);
void menu_state_set(menu_state state);
void menu_event_trigger(menu_event event);
void menu_event_loop(void);
#endif