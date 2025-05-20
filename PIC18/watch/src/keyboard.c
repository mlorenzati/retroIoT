#include "keyboard.h"
#include "timer.h"
#include <stddef.h>

void keyboard_handler(void) {
    // Caller by the IRQ timer periodically, fast
}

const char *key_index = NULL;
void keyboard_init(const char *idx) {
    // Configure I/O Ports

    // Define shared display index
    key_index = idx;

    // Register interrupt handler
    timer0_register_callback(keyboard_handler);
}