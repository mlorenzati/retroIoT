
#include "menu.h"
#include "display.h"
#include "beep.h"
#include "keyboard.h"
#include "clock.h"

// Globals
char menu_display_key_index = 0;
bool hold_hms_clock = true;

// Clock value update handler
void menu_on_updated_hms(char clock_hours, char clock_minutes, char clock_seconds, bool half_second) {
	if (hold_hms_clock) {
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

// On Alarm triggered handler
void menu_on_alarm_triggered(char alarm_index) {
}

// Keyboard pressed handler
void menu_on_keyboard_pressed(keyboard_status_t keys) {
	char hour;
	char minute;
	char seconds;
	clock_get_hms(&hour, &minute, &seconds);

    if (keys.key1 == key_released) {
		hour += 10;
	}
	if (keys.key2 == key_released) {
		hour += 1;
	}
	if (keys.key3 == key_released) {
		minute += 10;
	}
	if (keys.key4 == key_released) {
		minute += 1;
	}
	if (keys.key5 == key_released) {
		seconds += 10;
	}
	if (keys.key6 == key_released) {
		seconds += 1;
	}

	clock_set_hms(hour, minute, seconds);
}

void menu_on_playblack_completed(void) {

}

void on_last_animation_completed(char animation_action) {
	hold_hms_clock = false;
    const char play[2] = {0b10101010, 0b10101010};
    beep_play(play, 16, 4, menu_on_playblack_completed);
}

void on_first_animation_completed(char animation_action) {
	if (animation_action == DISPLAY_ANIMATE_TEXT_FWD) {
		display_scrolling_text("...you rock!...you know...", 0, 6, 8, true, false, on_last_animation_completed);	
	}
}

// Menu Initialization
void menu_init(char mode) {
    // Display Init, shares menu_display_key_index as multiplexor index
	display_init(&menu_display_key_index);

    // Keyboard Init
	keyboard_init(&menu_display_key_index, 28 /* 28 * 30ms = 840ms for repeat*/, 14 /* 13 * 30ms for repeat fast*/ , menu_on_keyboard_pressed);

    // Beeper Init
    beep_init(5);

    // 24 hs clock initialization
	menu_on_alarm_triggered(0); // Force linkeage and inclusion of method
    menu_on_playblack_completed(); // Force linkeage and inclusion of method
	clock_init(true /*24 hs mode*/, menu_on_updated_hms, menu_on_alarm_triggered);

    // For testing
    display_scrolling_text("Hello Almendra!", 6, 0, 8, false, false, on_first_animation_completed);
}

void _menu_event_loop(void) {
    // TBD Menu workflow
}

// Main event loop
void menu_event_loop(void) {
    _menu_event_loop();
    keyboard_event_loop();
    display_event_loop();
    clock_event_loop();
    beep_event_loop();
}