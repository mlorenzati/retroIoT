#include "clock.h"
#include "timer.h"
#include <stddef.h>
#include <stdbool.h>

// By default 24 hs format
bool clock_format_24_12_h    = true;
bool clock_half_second_cnt   = false;
bool clock_second_event      = false;
bool clock_half_second_event = false;
char clock_hours, clock_minute, clock_seconds;
clock_callback hms_callback = NULL;
clock_alarm_callback alarm_callback = NULL;
clock_alarm_data_t alarms[CLOCK_ALARM_MAX];

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
    clock_minute = 0;
    clock_seconds = 0;
    clock_half_second_cnt = false;

    // Register callbacks
    hms_callback = _hms_callback;
    alarm_callback = _alarm_callback;

    // Register interrupt handler
    timer0_register_callback(clock_handler);
}

signed char clock_set_alarm(signed char id, char hour, char minute) {
    if (id < 0) {
        // Look for an available id
        for (int i = 0; i < CLOCK_ALARM_MAX; i++) {
            clock_alarm_data_t *alarm = alarms + i;
            if (alarm->enabled) {
                continue;
            }
            id = (signed char) i;
            break;
        }
    }
    if (id > -1 && id < CLOCK_ALARM_MAX) {
        clock_alarm_data_t *alarm = alarms + id;
        alarm->hour    = hour;
        alarm->minute  = minute;
        alarm->snooze  = 0;
        alarm->enabled = true;
        return id;
    } 

    return -1;
}

signed char clock_unset_alarm(signed char id) {
    if (id < 0 || id >= CLOCK_ALARM_MAX) {
        return -1;
    }
    clock_alarm_data_t *alarm = alarms + (char) id;
    alarm->enabled = false;
    return id;
}

signed char clock_get_alarm(signed char id, char *hour, char *minute, bool *enabled) {
    if (id < 0 || id >= CLOCK_ALARM_MAX) {
        return -1;
    }
    clock_alarm_data_t *alarm = alarms + (char) id;
    *hour    = alarm->hour;
    *minute  = alarm->minute;
    *enabled = alarm->enabled;
    return id;
}

void clock_event_loop(void) {
    if (clock_half_second_event) {
        hms_callback(clock_hours, clock_minute, clock_seconds, clock_half_second_event);
        clock_half_second_event = false;
        return;
    } else if (!clock_second_event) {
        return;
    }

    // Once event is triggered ensure total propagation prior alarm validation
    clock_second_event = false;
    do {
        if (++clock_seconds < CLOCK_SECOND_MAX) {
            break;
        }
        clock_seconds = 0;
        if (++clock_minute < CLOCK_MINUTE_MAX) {
            break;
        }
        clock_minute = 0;
        if (++clock_hours < CLOCK_HOUR_MAX) {
            break;
        }
        clock_hours = 0;
    } while (false);
    
    // On updated h:m:s 
    if (hms_callback) {
        hms_callback(clock_hours, clock_minute, clock_seconds, clock_half_second_event);
    }

    //Check alarms only when seconds zeroed and alarm is connected
    if (!alarm_callback || clock_seconds > 0) {
        return;
    }

    for (int i = 0; i < CLOCK_ALARM_MAX; i++) {
        clock_alarm_data_t *alarm = alarms + i;
        if (!alarm->enabled) {
            continue;
        }
        char hour = alarm->hour;
        char minute = alarm->minute + alarm->snooze;
        if (minute >= CLOCK_MINUTE_MAX) {
            minute -= CLOCK_MINUTE_MAX;
            hour += 1;
        }
        if (hour >= CLOCK_HOUR_MAX) {
            hour -= CLOCK_HOUR_MAX;
        }
        if (hour == clock_hours && minute == clock_minute) {
            // Trigger alarm
            alarm_callback(i);
        }
    }
}

void clock_get_hms(char *hour, char *minute, char *seconds) {
    *hour = clock_hours;
    *minute = clock_minute;
    *seconds = clock_seconds;
}

bool clock_set_hms(char hour, char minute, char seconds) {
    if (hour >= CLOCK_HOUR_MAX || minute >= CLOCK_MINUTE_MAX || seconds >= CLOCK_SECOND_MAX) {
        return false;
    }
    clock_seconds = seconds;
    clock_minute = minute;
    clock_hours = hour;
}