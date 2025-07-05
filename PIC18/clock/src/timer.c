#include "timer.h"
#include <stddef.h>
#include <htc.h>

static signed char timer0_tune;
static signed char timer0_bias   = 0;
static char timer0_active_callbacks = 0;
static char timer1_active_callbacks = 0;
static timer_callback timer0_callbacks[TIMER0_CALLBACK_MAX] = TIMER0_CALLBACK_INIT;
static timer_callback timer1_callbacks[TIMER1_CALLBACK_MAX] = TIMER1_CALLBACK_INIT;

void timer0_init(signed char fine_tune) {
	timer0_tune = fine_tune;  // Minor timer drift tune
	T0CON = TMR0_PRESCALER;   // 16-bit mode, internal clock, prescaler 1:256
    TMR0H = TMR0H_500MS;      // High byte preload
    TMR0L = TMR0L_500MS_FAST; // Low byte preload
    TMR0IF = 0;               // Clear interrupt flag
    TMR0ON = 1;               // Turn on Timer0
	TMR0IE = 1;			      // Enable timer 0 interrupts
}

void timer1_init(void) {
	T1CON = TMR1_PRESCALER; // 8-bit mode, internal clock, prescaler 1:8
    TMR1H = TMR1H_5MS;      // High byte preload
    TMR1L = TMR1L_5MS;      // Low byte preload
    TMR1IF = 0;             // Clear interrupt flag
    TMR1ON = 1;             // Turn on Timer1
	TMR1IE = 1;			    // Enable timer 1 interrupts
}

void timer_dummy_callback(void) {}

void timer0_adjust_set(signed char fine_tune) {
	timer0_tune = fine_tune;
}

signed char timer0_adjust_get(void) {
	return timer0_tune;
}

char timer0_register_callback(timer_callback callback) {
	if (timer0_active_callbacks >= TIMER0_CALLBACK_MAX) {
		return 0;
	}
	timer0_callbacks[timer0_active_callbacks++] = callback;
	return timer0_active_callbacks;
}

char timer1_register_callback(timer_callback callback) {
	if (timer1_active_callbacks >= TIMER1_CALLBACK_MAX) {
		return 0;
	}
	timer1_callbacks[timer1_active_callbacks++] = callback;
	return timer1_active_callbacks;
}

void timer0_handler(void) {
	if (!TMR0IF) {
		return;
	}

	TMR0IF = 0;
	TMR0H = TMR0H_500MS;

	// Adjustment
	if (timer0_bias < 0) {
		TMR0L = TMR0L_500MS_SLOW;
		timer0_bias++;
	} else {
		TMR0L = TMR0L_500MS_FAST;
		timer0_bias--;
	}
	if (timer0_bias == 0) {
		timer0_bias += timer0_tune;
	}
	
	#if TIMER0_CALLBACK_MAX > 1
	for (char i = 0; i < TIMER0_CALLBACK_MAX; i++) {
		timer0_callbacks[i]();
	}
	#else
	timer0_callbacks[0]();
	#endif	
}


void timer1_handler(void) {
	if (!TMR1IF) {
		return;
	}
	TMR1IF = 0;

	TMR1H = TMR1H_5MS;
	TMR1L = TMR1L_5MS;

	#if TIMER1_CALLBACK_MAX > 1
	for (char i = 0; i < TIMER1_CALLBACK_MAX; i++) {
		timer1_callbacks[i]();
	}
	#else
	timer1_callbacks[i]();
	#endif
}