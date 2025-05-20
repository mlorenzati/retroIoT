#include <stddef.h>
#include "display.h"
#include "timer.h"

char *display_index = NULL;
unsigned char display_buffer[DISPLAY_INDEX_MAX];

void display_handler(void) {
    // Caller by the IRQ timer periodically, fast
    if (display_index == NULL) {
        return;
    }

    if (++(*display_index) >= DISPLAY_INDEX_MAX) {
        *display_index = 0;
    }
    // Prepare in ON 1 of the segments and make output only that one
    char display_activation = 1 << (*display_index);
    DISPLAY_INDEX_PORT_LATCH = display_activation;
    DISPLAY_INDEX_PORT_DIRECTION = (~(display_activation)) | DISPLAY_INDEX_PORT_MASK;
    DISPLAY_SEGMENTS_LATCH = display_buffer[*display_index];
}

void display_init(char *idx) {
    // Configure I/O Ports, display index disabled by default as input
    DISPLAY_INDEX_PORT_DIRECTION = 0xFF;
    
    // Segments as 1 stays off
    DISPLAY_SEGMENTS_LATCH  = 0xFF;
    DISPLAY_SEGMENTS_DIRECTION = 0x00;

    // Initialize display buffer
    for (int i = 0; i < DISPLAY_INDEX_MAX; i++) {
        display_buffer[i]= SEG_DP;
    }

    // Define shared display index
    display_index = idx;

    // Register interrupt handler
    timer0_register_callback(display_handler);
}

void display_update_index(unsigned char data, char index) {
    if (index >= DISPLAY_INDEX_MAX) {
        return;
    }
    display_buffer[index] = data;
}

void display_update_all(const unsigned char* data, bool reverse) {
    for (char i = 0; i < DISPLAY_INDEX_MAX; i++) {
        unsigned char data_index = reverse ? DISPLAY_INDEX_MAX - 1 - i : i;
        display_buffer[i] = data[data_index];
    }
}