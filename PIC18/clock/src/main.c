#include <stddef.h>
#include "config.h"
#include "timer.h"
#include "display.h"
#include "keyboard.h"
#include "clock.h"

void interrupt isr(void){
	timer0_handler();
	timer1_handler();
}

char display_key_index = 0;
// Keyboard pressed handler
void on_keyboard_pressed(keyboard_status_t keys) {
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

bool hold_hms_clock = true;
void on_updated_hms(char clock_hours, char clock_minutes, char clock_seconds, bool half_second) {
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

void on_alarm_triggered(char alarm_index) {
}

void peripheral_init(void) {
	// General: ADCs disabled, all RA are digital
	ADCON0 = 0;
	ADCON1 = 0x07;	

	// Timer 0 & Timer 1 Init 
	timer0_init(1);
	timer1_init();

	// Display Init, shares display_key_index as multiplexor index
	display_init(&display_key_index);

	// 24 hs clock initialization
	on_alarm_triggered(0); // Force linkeage and inclusion of method
	clock_init(true /*24 hs mode*/, on_updated_hms, on_alarm_triggered);

	// Keyboard Init
	keyboard_init(&display_key_index, 28 /* 28 * 30ms = 840ms for repeat*/, 14 /* 13 * 30ms for repeat fast*/ , on_keyboard_pressed);

	// Enable peripheral and global interrupts
	PEIE = 1;
	GIE = 1;
}

void on_last_animation_completed(char animation_action) {
	hold_hms_clock = false;
}

void on_first_animation_completed(char animation_action) {
	if (animation_action == DISPLAY_ANIMATE_TEXT_FWD) {
		display_scrolling_text("...you rock!...you know...", 0, 6, 8, true, false, on_last_animation_completed);	
	}
}

void main(void) {
	peripheral_init();
	display_scrolling_text("Hello Almendra!", 6, 0, 8, false, false, on_first_animation_completed);
    while (1) {
        keyboard_event_loop();
		clock_event_loop();
		display_event_loop();
    }
}
