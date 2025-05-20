#include "timer.h"
#include <stddef.h>
#include <htc.h>

void timer0_init(void) {
	T0CON = 0b00000011; // 16-bit mode, internal clock, prescaler 1:16
    TMR0H = TMRH_10MS;  // High byte preload
    TMR0L = TMRL_10MS;  // Low byte preload
    TMR0IF = 0;         // Clear interrupt flag
    TMR0ON = 1;         // Turn on Timer0
	TMR0IE = 1;			// Enable timer 0 interrupts
}

static char timer0_active_callbacks = 0;
static timer0_callback timer0_callbacks[TIMER0_CALLBACK_MAX] = TIMER0_CALLBACK_INIT;

int  timer0_register_callback(timer0_callback callback) {
	if (timer0_active_callbacks >= TIMER0_CALLBACK_MAX) {
		return 0;
	}
	timer0_callbacks[timer0_active_callbacks++] = callback;
	return timer0_active_callbacks;
}

void timer0_handler(void) {
	if(TMR0IE && TMR0IF){
		// 10ms interrupt
		TMR0IF = 0;
		TMR0H = TMRH_10MS;
        TMR0L = TMRL_10MS;
		
		for (char i = 0; i < TIMER0_CALLBACK_MAX; i++) {
			if (timer0_callbacks[i]) {
				timer0_callbacks[i]();
			}
		}
	}
}