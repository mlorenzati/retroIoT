#include <stdbool.h>
#include <stddef.h>
#include "beep.h"
#include "timer.h"

char beep_divider = 40;
char beep_counter = 0;
const char *beep_play_data = NULL;
char beep_play_size = 0;
char beep_play_index = 0;
char beep_play_count = 0;
bool beep_event_completed = false;
beep_callback beep_playback_completed = NULL;

void beep_handler(void) {
    if (++beep_counter < beep_divider) {
       return;
    }
    beep_counter = 0;

    // Play sequence if active
    if (beep_play_count == 0) {
        return;
    }

    // If a sequence completed, restart
    if (beep_play_index >= beep_play_size) {
        beep_play_index = 0;
        if (--beep_play_count == 0) {
            BEEP_PIN_LATCH = 0;
            return;
        }
    }

    char data_byte = beep_play_data[beep_play_index >> 3];
    bool data_bit = (data_byte >> (7 - (beep_play_index & 0x7))) & 1;
    BEEP_PIN_LATCH = data_bit;
    beep_play_index++;
}

void beep_init(char divider) {
    // Setup beep pin as out
    BEEP_PIN_DIRECTION = 0;
    BEEP_PIN_LATCH = 0;

    // Set tick system divider
    beep_divider = divider;

    // Register interrupt handler
    timer1_register_callback(beep_handler);
}

void beep_update_rate(char divider) {
    // Update tick system divider
    beep_divider = divider;
}

void beep_play(const char *data, char size, char play_count, beep_callback on_playback_completed) {
    beep_play_data  = data;
    beep_play_size  = size;
    beep_play_count = play_count;
    beep_play_index = 0;
    beep_playback_completed = on_playback_completed;
}

void beep_event_loop(void) {
    if (beep_event_completed && beep_playback_completed) {
        beep_playback_completed();
    }
}