#include "clock.h"
#include "timer.h"
#include <stddef.h>
#include <stdbool.h>

// By default 24 hs format
bool clock_format_24_12_h    = true;
bool clock_half_second_cnt   = false;
bool clock_second_event      = false;
bool clock_half_second_event = false;
char clock_hours, clock_minutes, clock_seconds;
clock_callback hms_callback = NULL;
clock_alarm_callback alarm_callback = NULL;

void clock_handler(void) {
    clock_half_second_cnt = !clock_half_second_cnt;
    if (clock_half_second_cnt) {
        clock_half_second_event = true;
    } else {
        clock_second_event = true;
    }
}

void clock_init(bool _clock_format_24_12_h, clock_callback _hms_callback, clock_alarm_callback _alarm_callback) {
    // Set Hour format
    clock_format_24_12_h = _clock_format_24_12_h;

    // Initialize clock registers
    clock_hours = 0;
    clock_minutes = 0;
    clock_seconds = 0;
    clock_half_second_cnt = false;

    // Register callbacks
    hms_callback = _hms_callback;
    alarm_callback = _alarm_callback;

    // Register interrupt handler
    timer0_register_callback(clock_handler);
}

void clock_event_loop(void) {
    if (clock_half_second_event) {
        hms_callback(clock_hours, clock_minutes, clock_seconds, clock_half_second_event);
        clock_half_second_event = false;
        return;
    } else if (!clock_second_event) {
        return;
    }

    // Once event is triggered ensure total propagation prior alarm validation
    clock_second_event = false;
    do {
        if (++clock_seconds < CLOCK_SECOND_CNT) {
            break;
        }
        clock_seconds = 0;
        if (++clock_minutes < CLOCK_MINUTE_CNT) {
            break;
        }
        clock_minutes = 0;
        if (++clock_hours < CLOCK_HOUR_CNT) {
            break;
        }
        clock_hours = 0;
    } while (false);
    
    // On updated h:m:s 
    if (hms_callback) {
        hms_callback(clock_hours, clock_minutes, clock_seconds, clock_half_second_event);
    }

    //Check alarm logic (TBD)
}

void clock_get_hms(char *hour, char *minute, char *seconds) {
    *hour = clock_hours;
    *minute = clock_minutes;
    *seconds = clock_seconds;
}

void clock_set_hms(char hour, char minute, char seconds) {
    char mod_seconds = seconds % 60;
    clock_seconds = mod_seconds;
    minute += (seconds / 60);
    char mod_minute = minute % 60;
    clock_minutes = mod_minute;
    hour += (minute / 60);
    clock_hours = hour % 24;
}