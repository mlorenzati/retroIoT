#include <stddef.h>
#include "display.h"
#include "timer.h"

char *display_index = NULL;
unsigned char display_buffer[DISPLAY_INDEX_MAX];

const unsigned char numbers_7_seg[10] = {
    DISPLAY_NUM_0, DISPLAY_NUM_1, DISPLAY_NUM_2, DISPLAY_NUM_3, DISPLAY_NUM_4, DISPLAY_NUM_5, DISPLAY_NUM_6, DISPLAY_NUM_7, DISPLAY_NUM_8, DISPLAY_NUM_9
};

void display_handler(void) {
    // Caller by the IRQ timer periodically, fast
    if (display_index == NULL) {
        return;
    }

    if (++(*display_index) >= DISPLAY_INDEX_MAX) {
        *display_index = 0;
    }
    // Prepare in ON 1 of the segments and make output only that one
    unsigned char display_activation = ~(1 << (*display_index));
    DISPLAY_INDEX_PORT_LATCH = display_activation;
    DISPLAY_INDEX_PORT_DIRECTION = display_activation | DISPLAY_INDEX_PORT_MASK;
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

void display_number_2_7_seg(int number, char index, char size) {
    if (index >= DISPLAY_INDEX_MAX) {
        // Index is above the max size
        return;
    }
    int max_size = DISPLAY_INDEX_MAX - index;
    size = size > max_size ? max_size : size;
    for (signed char idx = index + size - 1; idx >= index; idx--) {
        int mod = number % 10;
        number -= mod;
		number /= 10;
        display_update_index(numbers_7_seg[mod], idx);
    }
}

void display_update_segment(char segment_id, bool value, char index) {
    if (index >= DISPLAY_INDEX_MAX) {
        // Index is above the max size
        return;
    }
    if (value) {
        display_buffer[index] &= segment_id;
    } else {
        display_buffer[index] |= ~segment_id;
    }
}