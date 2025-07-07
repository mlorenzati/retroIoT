#ifndef CLOCK_H
#define CLOCK_H
#include <stdbool.h>

#define CLOCK_SECOND_MAX    60
#define CLOCK_MINUTE_MAX    60
#define CLOCK_HOUR_MAX      24
#define CLOCK_ALARM_MAX     4
#define CLOCK_SNOOZE_MAX    5

typedef struct clock_alarm_data {
    char hour;
    char minute;
    char snooze;
    bool enabled;
} clock_alarm_data_t;


#if CLOCK_ALARM_MAX == 1
  #define CLOCK_ALARM_INIT {{ 0, 0, 0, 0}}
#elif CLOCK_ALARM_MAX == 2
  #define CLOCK_ALARM_INIT {{ 0, 0, 0, 0}, { 0, 0, 0, 0}}
#elif CLOCK_ALARM_MAX == 3
  #define CLOCK_ALARM_INIT {{ 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 0, 0, 0}}
#elif CLOCK_ALARM_MAX == 4
  #define CLOCK_ALARM_INIT {{ 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 0, 0, 0}}
#else
  #error "TIMER0_CALLBACK_MAX not supported yet for that size"
#endif
typedef void (*clock_alarm_callback)(char);
typedef void (*clock_callback)(char, char, char, bool);

void clock_init(bool clock_format_24_12_h, clock_callback hms_callback, clock_alarm_callback alarm_callback);
signed char clock_set_alarm(signed char id, char hour, char minute);
bool clock_update_alarm(signed char id, char hour, char minute, bool enabled);
bool clock_stop_alarm(signed char id);
signed char clock_unset_alarm(signed char id);
signed char clock_get_alarm(signed char id, char *hour, char *minute, bool *enabled);

void clock_event_loop(void);
void clock_get_hms(char *hour, char *minute, char *seconds);
bool clock_set_hms(char hour, char minute, char seconds);

#endif