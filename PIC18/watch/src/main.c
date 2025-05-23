#include <stddef.h>
#include "config.h"
#include "timer.h"
#include "display.h"
#include "keyboard.h"

void interrupt isr(void){
	timer0_handler();
}

char display_key_index = 0;
// Keyboard pressed handler
void on_keyboard_pressed(keyboard_status_t keys) {
    for (char key_idx = 0; key_idx < KEY_INDEX_MAX; key_idx++) {
        key_value key = get_key_from(keys, key_idx);
        if (key == key_pressed) {
            display_update_index(DISPLAY_NUM_0, key_idx);
        } else if (key == key_released) {
            display_update_index(DISPLAY_NUM_1, key_idx);
        } else if (key == key_repeat) {
            display_update_index(DISPLAY_NUM_2, key_idx);
        }
    }
}

void peripheral_init(void) {
	// Timer 0 Init
	timer0_init();

	// Display Init, shares display_key_index as multiplexor index
	display_init(&display_key_index);

	// Keyboard Init, requires help to force function to avoid being removed by optimization
	FORCE_INCLUDE(on_keyboard_pressed);
	keyboard_init(&display_key_index, 200, 80, on_keyboard_pressed);

	// Enable peripheral and global interrupts
	PEIE = 1;
	GIE = 1;
}

void main(void) {
	peripheral_init();
	const unsigned char msg[DISPLAY_INDEX_MAX] = { 
		DISPLAY_CHR_H, DISPLAY_CHR_E, DISPLAY_CHR_L, DISPLAY_CHR_L, DISPLAY_CHR_O, DISPLAY_CHR_EXCLAMATION
	};
	display_update_all(msg, false);
    while (1) {
        keyboard_event_loop();
    }
}
