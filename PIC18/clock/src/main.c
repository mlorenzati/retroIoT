#include "config.h"
#include "timer.h"
#include "menu.h"

void interrupt isr(void){
	timer0_handler();
	timer1_handler();
}

void bootstrap_init(void) {
	// General: ADCs disabled, all RA are digital
	ADCON0 = 0;
	ADCON1 = 0x07;	

	// Timer 0 & Timer 1 Init 
	timer0_init(1);
	timer1_init();

	// Menu system init (internally deals with display, keyboard, beeper and seconds clock)
	menu_init(MENU_MODE_STARTUP);

	// Enable peripheral and global interrupts
	PEIE = 1;
	GIE = 1;
}

void main(void) {
	bootstrap_init();
    while (1) {
		menu_event_loop();
    }
}
