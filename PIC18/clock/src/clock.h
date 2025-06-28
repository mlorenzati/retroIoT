#ifndef CLOCK_H
#define CLOCK_H
#include <stdbool.h>

#define CLOCK_MSSECOND_CNT  200
#define CLOCK_MSHALFSEC_CNT 100
#define CLOCK_SECOND_CNT    60
#define CLOCK_MINUTE_CNT    60
#define CLOCK_HOUR_CNT      24
#define CLOCK_ALARM_CNT     6

typedef void (*clock_alarm_callback)(char);
typedef void (*clock_callback)(char, char, char, bool);

void clock_init(bool clock_format_24_12_h, clock_callback hms_callback, clock_alarm_callback alarm_callback);
char clock_set_alarm(char hour, char minute, char seconds);
void clock_update_alarm(char alarm_index, char hour, char minute, char seconds);
void clock_remove_alarm(char alarm_index);
void clock_event_loop(void);
void clock_get_hms(char *hour, char *minute, char *seconds);
void clock_set_hms(char hour, char minute, char seconds);

#endif