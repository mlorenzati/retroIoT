
#include <stddef.h>
#include "menu.h"
#include "display.h"
#include "beep.h"
#include "keyboard.h"
#include "clock.h"

// Globals
char menu_display_key_index   = 0;
menu_state menu_prev_state   = MENU_STATE_NONE;
menu_state menu_current_state = MENU_STATE_NONE;
menu_event menu_current_event = MENU_EVENT_NONE;

// Clock value update handler
void menu_on_updated_hms(char clock_hours, char clock_minutes, char clock_seconds, bool half_second) {
    if (menu_current_state != MENU_STATE_CLOCK) {
        return;
    }

	if (half_second) {
		half_second = false;
		// Skip redraw on half second
		display_update_segment(SEG_DP, true, 1);
		display_update_segment(SEG_DP, true, 3);
		return;
	}
	// Redraw
	display_number_2_7_seg(clock_seconds, 4, 2);
	display_number_2_7_seg(clock_minutes, 2, 2);
	display_number_2_7_seg(clock_hours, 0, 2);
}

void menu_on_alarm_playblack_completed(void) {
    menu_state_set(MENU_STATE_CLOCK);
    menu_event_trigger(MENU_EVENT_GENERIC);
}

// On Alarm triggered handler
void menu_on_alarm_triggered(char alarm_index) {
    if (menu_current_state != MENU_STATE_CLOCK && menu_current_state != MENU_STATE_ALARM) {
        return;
    }
    menu_event_trigger(MENU_EVENT_ALARM_TRIGGERED);
    menu_state_set(MENU_STATE_ALARM);
}

// Keyboard pressed handler
void menu_on_keyboard_pressed(keyboard_status_t keys) {
    if (keys.key1 == key_released) {
	}
    if (keys.key2 == key_released) {
	}
	if (keys.key3 == key_released) {
	}
	if (keys.key4 == key_released) {
	}
	if (keys.key5 == key_released) {
	}
	if (keys.key6 == key_released) {
	}
}

// Menu Initialization
void menu_init(menu_state mode) {
    // Setup initial mode
    menu_current_state = mode;

    // Display Init, shares menu_display_key_index as multiplexor index
	display_init(&menu_display_key_index);

    // Keyboard Init
	keyboard_init(&menu_display_key_index, 28 /* 28 * 30ms = 840ms for repeat*/, 14 /* 13 * 30ms for repeat fast*/ , menu_on_keyboard_pressed);

    // Beeper Init
    beep_init(15);

    // 24 hs clock initialization
	clock_init(true /*24 hs mode*/, menu_on_updated_hms, menu_on_alarm_triggered);

    // TEST: Set an alarm in 1 minute
    clock_set_alarm(-1, 0, 1);
}

void menu_state_set(menu_state state) {
    menu_prev_state = menu_current_state;
    menu_current_state = state;
}

void menu_event_trigger(menu_event event) {
    menu_current_event = event;
}

void menu_on_startup_completed(char animation_action) {
    menu_state_set(MENU_STATE_CLOCK);
    menu_event_trigger(MENU_EVENT_GENERIC);
}

void _menu_event_loop(void) {
    if (menu_current_event == MENU_EVENT_NONE) {
        return;
    }

    menu_event current_event = menu_current_event;
    menu_current_event = MENU_EVENT_NONE;

    switch (menu_current_state) {
        case MENU_STATE_STARTUP:
            if (current_event == MENU_EVENT_GENERIC) {
                display_scrolling_text("Welcome Home!... ho.. ho.. hoo...", 6, 0, 2, false, false, menu_on_startup_completed);
            }
            break;
        case MENU_STATE_CLOCK:
            if (current_event == MENU_EVENT_GENERIC) {
                char hour, minute, seconds;
                clock_get_hms(&hour, &minute, &seconds);
                menu_on_updated_hms(hour, minute, seconds, false);
            }
            break;
        case MENU_STATE_ALARM_CONFIG:
            break;
        case MENU_STATE_ALARM:
            if (current_event == MENU_EVENT_ALARM_TRIGGERED) {
                const char play[2] = {0b10101010, 0b10101010};
                beep_play(play, 16, 10, menu_on_alarm_playblack_completed);
                display_text("WakeUp", 0, 6, false);
            }
            break;
        case MENU_STATE_NONE:
        default:
            break;
    }
}

// Main event loop
void menu_event_loop(void) {
    while (true) {
        _menu_event_loop();
        keyboard_event_loop();
        display_event_loop();
        clock_event_loop();
        beep_event_loop();
    }
}