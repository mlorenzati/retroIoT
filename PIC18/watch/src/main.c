#include "config.h"
#include "timer.h"
#include "display.h"
#include "keyboard.h"

void interrupt isr(void){
	timer0_handler();
}

char display_key_index = 0;
void peripheral_init() {
	// Timer 0 Init
	timer0_init();

	// Display Init, shares display_key_index as multiplexor index
	display_init(&display_key_index);

	// Keyboard Init
	keyboard_init(&display_key_index);

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
        // main loop
    }
}