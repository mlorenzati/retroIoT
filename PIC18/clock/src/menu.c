
#include <stddef.h>
#include <stdlib.h>
#include "menu.h"
#include "display.h"
#include "beep.h"
#include "keyboard.h"
#include "clock.h"
#include "timer.h"

// Globals
char  menu_display_key_index   = 0;
menu_state menu_prev_state   = MENU_STATE_NONE;
menu_state menu_current_state = MENU_STATE_NONE;
menu_event menu_current_event = MENU_EVENT_NONE;
menu_ev_loop_callback menu_event_loop_tasks[] = {
    clock_event_loop,       /* Maximum priority to check, low presence in a second */
    keyboard_event_loop,    /* Medium priority to respond to a keyboard action */
    menu_event_loop,        /* Medium to low priority */
    beep_event_loop,        /* Low priority */
    display_event_loop      /* Most intensive app, reduced priority */
};

const char menu_freq_mask[] = { (0), (0x2 - 1), (0x2 - 1), (0x4 - 1), (0x8 - 1) };
char menu_alarm_config_n = 0;
char menu_display_selection_index = 0;

// Tunes
const char blip_fast[2] = {0b10101010, 0b10101010};
const char sustained_short[2] = {0b01111111, 0b01111111};

// Clock value update handler
void menu_on_updated_hms(char clock_hours, char clock_minutes, char clock_seconds, bool half_second) {
    if (menu_current_state == MENU_STATE_CLOCK_CONFIG && display_get_animation_status() == DISPLAY_ANIMATE_NONE) {
        display_number_2_7_seg(clock_seconds, 4, 2);
        display_update_segment(SEG_DP, true, menu_display_selection_index);
        return;
    } else if (menu_current_state != MENU_STATE_CLOCK) {
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
	display_number_2_7_seg(clock_hours,   0, 2);
    display_number_2_7_seg(clock_minutes, 2, 2);
    display_number_2_7_seg(clock_seconds, 4, 2);
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
        menu_event_trigger(MENU_EVENT_NEXT_MENU);
	}
    if (keys.key2 == key_released) {
        menu_event_trigger(MENU_EVENT_LEFT);
	}
	if (keys.key3 == key_released) {
        menu_event_trigger(MENU_EVENT_RIGHT);
	}
	if (keys.key4 == key_released || keys.key4 == key_repeat) {
        menu_event_trigger(MENU_EVENT_UP);
	}
	if (keys.key5 == key_released || keys.key5 == key_repeat) {
        menu_event_trigger(MENU_EVENT_DOWN);
	}
	if (keys.key6 == key_released) {
        menu_event_trigger(MENU_EVENT_KEY_OK);
	}
}

// Menu Initialization
void menu_init(menu_state mode) {
    // Setup initial mode
    menu_current_state = mode;

    // Display Init, shares menu_display_key_index as multiplexor index
	display_init(&menu_display_key_index);

    // Keyboard Init
	keyboard_init(&menu_display_key_index, 28 /* 28 * 30ms = 840ms for repeat*/, 14 /* 13 * 30ms for repeat fast*/, menu_on_keyboard_pressed);

    // Beeper Init
    beep_init(15);

    // 24 hs clock initialization
	clock_init(true /*24 hs mode*/, menu_on_updated_hms, menu_on_alarm_triggered);
}

void menu_state_set(menu_state state) {
    menu_prev_state = menu_current_state;
    menu_current_state = state;
}

void menu_event_trigger(menu_event event) {
    menu_current_event = event;
}

void menu_on_clock_show(char animation_action) {
    display_update_action(DISPLAY_ANIMATE_NONE);
    menu_state_set(MENU_STATE_CLOCK);
    menu_event_trigger(MENU_EVENT_GENERIC);
}

void menu_on_alarm_cfg_message_completed(char animation_action) {
    char hour, minute;
    bool enabled;
    clock_get_alarm(menu_alarm_config_n, &hour, &minute, &enabled);
    if (hour >= CLOCK_HOUR_MAX || minute >= CLOCK_MINUTE_MAX) {
        hour = 0;
        minute = 0;
        clock_update_alarm(menu_alarm_config_n, hour, minute, enabled);
    }
    display_number_2_7_seg(hour,   0, 2);
    display_number_2_7_seg(minute, 2, 2);
    display_text(enabled ? "on" : "of", 4, 2, false);
    display_update_segment(SEG_DP, true, menu_display_selection_index);
}

void menu_on_clock_cfg_message_completed(char animation_action) {
    char hour, minute, seconds;
    clock_get_hms(&hour, &minute, &seconds);
	display_number_2_7_seg(hour,   0, 2);
    display_number_2_7_seg(minute, 2, 2);
    display_number_2_7_seg(seconds, 4, 2);
    display_update_segment(SEG_DP, true, menu_display_selection_index);
}

void menu_on_clock_tune_completed(char animation_action) {
    signed char tune = timer0_adjust_get();

    char tune_abs = abs(tune);
    if (tune_abs > 99) {
        display_text("er", 2, 2, false);
    } else {
        display_number_2_7_seg(tune_abs, 2, 2);
    }

    display_update_segment(DISPLAY_CHR_MINUS, tune < 0, 1);
}

void menu_set_hms(menu_event event, char *hour, char *minute, char *second) {
    char hour_math = 0;
    char min_math  = 0;
    char sec_math  = 0;
    switch (menu_display_selection_index) {
        case 0: hour_math = 10; break;
        case 1: hour_math = 1; break;
        case 2: min_math = 10; break;
        case 3: min_math = 1; break;
        case 4: case 5: sec_math = 1; break;
    };
    *hour   = (event == MENU_EVENT_UP) ? *hour   + hour_math : *hour   - hour_math;
    *minute = (event == MENU_EVENT_UP) ? *minute + min_math  : *minute - min_math;
    *second = (event == MENU_EVENT_UP) ? *second + sec_math  : *second - sec_math;
    if (*hour >= CLOCK_HOUR_MAX) {
        *hour = (event == MENU_EVENT_UP) ? 0 : CLOCK_HOUR_MAX - 1;
    }
    if (*minute >= CLOCK_MINUTE_MAX) {
        *minute = (event == MENU_EVENT_UP) ? 0 : CLOCK_MINUTE_MAX - 1;
    }
    if (*second >= CLOCK_SECOND_MAX) {
        *second = (event == MENU_EVENT_UP) ? 0 : CLOCK_SECOND_MAX - 1;
    }
}

void menu_event_loop(void) {
    if (menu_current_event == MENU_EVENT_NONE) {
        return;
    }

    menu_event current_event = menu_current_event;
    menu_current_event = MENU_EVENT_NONE;

    switch (menu_current_state) {
        case MENU_STATE_STARTUP:
            if (current_event == MENU_EVENT_GENERIC) {
                display_scrolling_text("What happened?", 6, 0, 1, false, false, menu_on_clock_show);
                beep_play(sustained_short, 16, 1, NULL);
            }
            break;
        case MENU_STATE_CLOCK:
            if (current_event == MENU_EVENT_GENERIC) {
                char hour, minute, seconds;
                clock_get_hms(&hour, &minute, &seconds);
                menu_on_updated_hms(hour, minute, seconds, false);
            } else if (current_event == MENU_EVENT_NEXT_MENU) {
                menu_state_set(MENU_STATE_ALARM_CONFIG);
                menu_event_trigger(MENU_EVENT_GENERIC);
            }
            break;
        case MENU_STATE_ALARM_CONFIG:
            if (current_event == MENU_EVENT_GENERIC) {
                const char *configs[CLOCK_ALARM_MAX] = {"Alarm 1", "Alarm 2", "Alarm 3", "Alarm 4"};
                display_scrolling_text(configs[menu_alarm_config_n], 6, 0, 1, false, false, menu_on_alarm_cfg_message_completed);
            } else if (current_event == MENU_EVENT_NEXT_MENU) {
                menu_event_trigger(MENU_EVENT_GENERIC);
                if (++menu_alarm_config_n >= CLOCK_ALARM_MAX) {
                    menu_alarm_config_n = 0;
                    menu_state_set(MENU_STATE_CLOCK_CONFIG);
                }
            } else if (current_event == MENU_EVENT_LEFT || current_event == MENU_EVENT_RIGHT) {
                display_update_segment(SEG_DP, false, menu_display_selection_index);
                if (current_event == MENU_EVENT_LEFT) {
                    if (--menu_display_selection_index >= 5) {
                        menu_display_selection_index = 4;
                    }
                } else {
                    if (++menu_display_selection_index >= 5) {
                        menu_display_selection_index = 0;
                    }
                }
                display_update_segment(SEG_DP, true, menu_display_selection_index);
            } else if (current_event == MENU_EVENT_UP || current_event == MENU_EVENT_DOWN) {
                char hour, minute, second;
                bool enabled;
                clock_get_alarm(menu_alarm_config_n, &hour, &minute, &enabled);
                if (menu_display_selection_index == 4) {
                    enabled = !enabled;
                } else {
                    menu_set_hms(current_event, &hour, &minute, &second);
                }
                clock_update_alarm(menu_alarm_config_n, hour, minute, enabled);
                menu_on_alarm_cfg_message_completed(DISPLAY_ANIMATE_NONE);
            }
            break;
        case MENU_STATE_ALARM:
            if (current_event == MENU_EVENT_ALARM_TRIGGERED) {
                beep_play(blip_fast, 16, 10, menu_on_alarm_playblack_completed);
                display_text("WakeUp", 0, 6, false);
            }
            break;
        case MENU_STATE_CLOCK_CONFIG:
            if (current_event == MENU_EVENT_GENERIC) {
                display_scrolling_text("Clock config", 6, 0, 1, false, false, menu_on_clock_cfg_message_completed);
            } else if (current_event == MENU_EVENT_NEXT_MENU) {
                menu_event_trigger(MENU_EVENT_GENERIC);
                menu_state_set(MENU_STATE_CLOCK_TUNE);
            } else if (current_event == MENU_EVENT_LEFT || current_event == MENU_EVENT_RIGHT) {
                display_update_segment(SEG_DP, false, menu_display_selection_index);
                if (current_event == MENU_EVENT_LEFT) {
                    if (--menu_display_selection_index >= 5) {
                        menu_display_selection_index = 4;
                    }
                } else {
                    if (++menu_display_selection_index >= 5) {
                        menu_display_selection_index = 0;
                    }
                }
                display_update_segment(SEG_DP, true, menu_display_selection_index);
            } else if (current_event == MENU_EVENT_UP || current_event == MENU_EVENT_DOWN) {
                char hour, minute, second;
                clock_get_hms( &hour, &minute, &second);
                menu_set_hms(current_event, &hour, &minute, &second);
                clock_set_hms(hour, minute, second);
                menu_on_clock_cfg_message_completed(DISPLAY_ANIMATE_NONE);
            }
            break;
        case MENU_STATE_CLOCK_TUNE:
            if (current_event == MENU_EVENT_GENERIC) {
                display_scrolling_text("Clock Tune", 6, 0, 1, false, false, menu_on_clock_tune_completed);
            } else if (current_event == MENU_EVENT_NEXT_MENU) {
                if (display_get_animation_status() == DISPLAY_ANIMATE_TEXT_FWD) {
                    menu_on_clock_show(DISPLAY_ANIMATE_NONE);
                } else {
                    display_scrolling_text("Clock Time", 6, 0, 1, false, false, menu_on_clock_show);
                }
            } else if (current_event == MENU_EVENT_UP || current_event == MENU_EVENT_DOWN) {
                signed char tune = timer0_adjust_get();
                tune = current_event == MENU_EVENT_UP ? tune + 1 : tune - 1;
                if (abs(tune) < 99) {
                    timer0_adjust_set(tune);
                    menu_on_clock_tune_completed(DISPLAY_ANIMATE_NONE);
                }
            }
            break;
        case MENU_STATE_NONE:
        default:
            break;
    }
}

// Main event loop
void menu_runner(void) {
    char cnt = 0;
    while (true) {
        cnt++;
        for (char i = 0; i < MENU_CALLBACK_MAX; i++) {
            if ((cnt & menu_freq_mask[i]) == 0) {
                menu_event_loop_tasks[i]();
            }
        }
    }
}