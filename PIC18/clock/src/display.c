#include <stddef.h>
#include <stdbool.h>
#include "display.h"
#include "timer.h"
// Scan
static char *display_index = NULL;
static char display_frames_cnt = 0;
static bool display_tick_event = false;
static unsigned char display_buffer[DISPLAY_INDEX_MAX];

// Mappings: Numbers 
const unsigned char numbers_7_seg[10] = {
    DISPLAY_NUM_0, DISPLAY_NUM_1, DISPLAY_NUM_2, DISPLAY_NUM_3, DISPLAY_NUM_4, DISPLAY_NUM_5, DISPLAY_NUM_6, DISPLAY_NUM_7, DISPLAY_NUM_8, DISPLAY_NUM_9
};

// Mappings: Text 
const unsigned char char_7_seg[26] = { DISPLAY_CHR_A, DISPLAY_CHR_B, DISPLAY_CHR_C, DISPLAY_CHR_D, DISPLAY_CHR_E, DISPLAY_CHR_F, DISPLAY_CHR_G, 
    DISPLAY_CHR_H, DISPLAY_CHR_I, DISPLAY_CHR_J, DISPLAY_CHR_K, DISPLAY_CHR_L, DISPLAY_CHR_M, DISPLAY_CHR_N, DISPLAY_CHR_O, DISPLAY_CHR_P, DISPLAY_CHR_Q, 
    DISPLAY_CHR_R,DISPLAY_CHR_S, DISPLAY_CHR_T, DISPLAY_CHR_U, DISPLAY_CHR_V, DISPLAY_CHR_W, DISPLAY_CHR_X, DISPLAY_CHR_Y, DISPLAY_CHR_Z
};

// Animations
unsigned char display_animate_action = DISPLAY_ANIMATE_NONE;
unsigned char display_animate_speed  = 0;
unsigned char display_animate_count  = 0;
unsigned char display_animate_index_start = 0;
unsigned char display_animate_index_end   = 0;
const unsigned char *display_animate_data = NULL;
const unsigned char *display_animate_data_end = NULL;

void display_handler(void) {
    // Caller by the IRQ timer periodically, fast
    if (display_index == NULL) {
        return;
    }

    if (++(*display_index) >= DISPLAY_INDEX_MAX) {
        *display_index = 0;
        if (++display_frames_cnt >= DISPLAY_FRAMES_MAX) {
            display_tick_event = true;
        }
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
    timer1_register_callback(display_handler);
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

void display_event_loop(void) {
    if (!display_tick_event) {
        return;
    }
    display_tick_event = false;
    if (++display_animate_count < display_animate_speed) {
        return;
    }
    display_animate_count = 0;
    // Run Animations if requested
    char empty_chars = 0;
    switch (display_animate_action) {
        case DISPLAY_ANIMATE_TEXT_FWD:
            empty_chars = display_text(display_animate_data, display_animate_index_start, DISPLAY_FRAMES_MAX, true);
            if (display_animate_index_start > 0) {
                display_animate_index_start--;
            } else {
                if (display_animate_data < display_animate_data_end) {
                    display_animate_data++;
                }
            }
            if (empty_chars >= display_animate_index_end) {
                display_animate_action = DISPLAY_ANIMATE_NONE;
            }
            break;
        case DISPLAY_ANIMATE_TEXT_RWD:
            empty_chars = display_text(display_animate_data_end, display_animate_index_start, DISPLAY_FRAMES_MAX, true);
            if (display_animate_index_start > 0) {
                display_animate_index_start--;
            } else {
                if (display_animate_data_end > display_animate_data) {
                    display_animate_data_end--;
                }
            }
            break;
        default:
            display_animate_action = DISPLAY_ANIMATE_NONE;
            break;
    }
}

char display_text_length(const char *data) {
    for (int i = 0; i < DISPLAY_MSG_LENGTH; i++) {
        if (data[i] == 0) {
            return i;
        }
    }
    return 0;
}

char display_text(const char* data, char index, char size, bool off_left) {
    // Check index limit
    if (index >= DISPLAY_INDEX_MAX && !off_left) {
        return;
    }

    // Limit size to maximum
    char index_max = size + index;
    if (index_max > DISPLAY_INDEX_MAX) {
        index_max = DISPLAY_INDEX_MAX;
    }
    size = 0;

    //Turn off display on the left if required
    if (off_left && index != 0) {
        for (char i = 0; i < index; i++) {
            display_buffer[i] = DISPLAY_OFF;
        }
    } 
    for (char i = index; i < index_max; i++) {
        char value = *data;
        if (value >= 'a' && value <= 'z') {
            display_buffer[i] = char_7_seg[value - 'a'];
        } else if (value >= 'A' && value <= 'Z') {
            display_buffer[i] = char_7_seg[value - 'A'];
        } else if (value >= '0' && value <= '9') {
            display_buffer[i] = char_7_seg[value - '0'];
        } else if (value == '-') {
            display_buffer[i] = DISPLAY_CHR_MINUS;
        } else if (value == '!') {
            display_buffer[i] = DISPLAY_CHR_EXCLAMATION;
        } else if (value == '?') {
            display_buffer[i] = DISPLAY_CHR_QUESTION;
        } else if (value == '.' || value == ',') {
            display_buffer[i] = DISPLAY_CHR_DOT;
        } else {
            display_buffer[i] = DISPLAY_OFF;
        }
        if (value != 0) {
            data++;
        } else {
            size++;
        }
    }
    return size;
}

bool display_scrolling_text(const char* data, char index_start, char index_end, char speed, bool reverse, bool check_busy) {
    if (check_busy && display_animate_action != DISPLAY_ANIMATE_NONE) {
        return false;
    }
    char text_length = display_text_length(data);
    if (text_length == 0) {
        return false;
    }

    display_animate_action      = reverse ? DISPLAY_ANIMATE_TEXT_RWD : DISPLAY_ANIMATE_TEXT_FWD;
    display_animate_speed       = speed;
    display_animate_count       = speed;
    display_animate_index_start = index_start;
    display_animate_index_end   = index_end > DISPLAY_INDEX_MAX ? 0 : DISPLAY_INDEX_MAX - index_end;
    display_animate_data        = data;
    display_animate_data_end    = data + text_length;
    
    return true;
}