#ifndef BEEP_H
#define BEEP_H

#include <htc.h>
#define BEEP_PIN_DIRECTION  TRISA1
#define BEEP_PIN_LATCH      LATA1

typedef void (*beep_callback)(void);

void beep_init(char divider);
void beep_update_rate(char divider);
void beep_play(const char *data, char size, char play_count, beep_callback on_playback_completed);
void beep_event_loop(void);

#endif