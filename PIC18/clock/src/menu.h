#ifndef MENU_H
#define MENU_H

#define MENU_MODE_STARTUP        0
#define MENU_MODE_CLOCK          1
#define MENU_MODE_ALARM_SET      2
#define MENU_MODE_ALARM_TRIGERED 3
#define MENU_MODE_ALARM_SNOOZE   4

void menu_init(char mode);
void menu_event_loop(void);
#endif