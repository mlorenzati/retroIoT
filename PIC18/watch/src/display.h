#ifndef DISPLAY_H
#define DISPLAY_H

#include <htc.h>
#include <stdbool.h>
// Display Map
// ..A..
// F   B
// ..G..
// E   C
// ..D.. (DP)

//    SEGMENT : WORD       : BIT
#define SEG_F   0b11111110 // 0
#define SEG_G   0b11111101 // 1
#define SEG_A   0b11111011 // 2
#define SEG_B   0b11110111 // 3
#define SEG_E   0b11101111 // 4
#define SEG_D   0b11011111 // 5
#define SEG_C   0b10111111 // 6
#define SEG_DP  0b01111111 // 7

// Numbers
#define DISPLAY_NUM_0 (SEG_F & SEG_A & SEG_B & SEG_E & SEG_D & SEG_C)
#define DISPLAY_NUM_1 (SEG_B & SEG_C)
#define DISPLAY_NUM_2 (SEG_G & SEG_A & SEG_B & SEG_E & SEG_D)
#define DISPLAY_NUM_3 (SEG_G & SEG_A & SEG_B & SEG_D & SEG_C)
#define DISPLAY_NUM_4 (SEG_G & SEG_G & SEG_B & SEG_C)
#define DISPLAY_NUM_5 (SEG_F & SEG_G & SEG_A & SEG_D & SEG_C)
#define DISPLAY_NUM_6 (SEG_F & SEG_G & SEG_A & SEG_E & SEG_D & SEG_C)
#define DISPLAY_NUM_7 (SEG_A & SEG_B & SEG_B)
#define DISPLAY_NUM_8 (SEG_F & SEG_G & SEG_A & SEG_B & SEG_E & SEG_D & SEG_C)
#define DISPLAY_NUM_9 (SEG_F & SEG_G & SEG_A & SEG_B & SEG_D & SEG_C)

// Chars
#define DISPLAY_CHR_A (SEG_F & SEG_G & SEG_A & SEG_B & SEG_E & SEG_C)
#define DISPLAY_CHR_B DISPLAY_NUM_8
#define DISPLAY_CHR_C (SEG_F & SEG_A & SEG_E & SEG_D)
#define DISPLAY_CHR_E (SEG_F & SEG_G & SEG_A & SEG_E & SEG_D)
#define DISPLAY_CHR_F (SEG_F & SEG_G & SEG_A & SEG_E)
#define DISPLAY_CHR_G DISPLAY_NUM_6
#define DISPLAY_CHR_H (SEG_F & SEG_G & SEG_B & SEG_E & SEG_C)
#define DISPLAY_CHR_I DISPLAY_NUM_1
#define DISPLAY_CHR_J (SEG_B & SEG_E & SEG_D & SEG_C)
#define DISPLAY_CHR_K DISPLAY_CHR_H
#define DISPLAY_CHR_L (SEG_F & SEG_E & SEG_D)
#define DISPLAY_CHR_M (SEG_F & SEG_A & SEG_B & SEG_E & SEG_C)
#define DISPLAY_CHR_N (SEG_G & SEG_E & SEG_C)
#define DISPLAY_CHR_Ñ (SEG_G & SEG_A & SEG_E & SEG_C)
#define DISPLAY_CHR_O DISPLAY_NUM_0
#define DISPLAY_CHR_P (SEG_F & SEG_G & SEG_A & SEG_B & SEG_E)
#define DISPLAY_CHR_Q (DISPLAY_CHR_O & SEG_DP)
#define DISPLAY_CHR_R (DISPLAY_CHR_H & SEG_DP)
#define DISPLAY_CHR_S DISPLAY_NUM_5
#define DISPLAY_CHR_T DISPLAY_NUM_7
#define DISPLAY_CHR_U (SEG_F & SEG_B & SEG_E & SEG_D & SEG_C)
#define DISPLAY_CHR_V (SEG_E & SEG_D & SEG_C)
#define DISPLAY_CHR_W (DISPLAY_CHR_V & SEG_DP)
#define DISPLAY_CHR_X DISPLAY_CHR_H
#define DISPLAY_CHR_Y (SEG_F & SEG_G & SEG_B & SEG_D & SEG_C)
#define DISPLAY_CHR_Z DISPLAY_NUM_2

// Symbols
#define DISPLAY_CHR_EXCLAMATION (SEG_B & SEG_DP)
#define DISPLAY_CHR_QUESTION    (SEG_F & SEG_G & SEG_A & SEG_C & SEG_DP)

// Display INDEX PINS, RB5..RB0 (RB7:RB6 are PGD/PGC for ICSP)
#define DISPLAY_INDEX_PORT_DIRECTION  TRISB
#define DISPLAY_INDEX_PORT_LATCH      LATB
#define DISPLAY_INDEX_PORT_MASK       0b11000000

// Display Segments are in RC7:RC0
#define DISPLAY_SEGMENTS_DIRECTION    TRISC
#define DISPLAY_SEGMENTS_LATCH        LATC   

// Display Index management
#define DISPLAY_INDEX_MAX 6

void display_init(char *display_index);
// Data should be larger or equal in size to DISPLAY_INDEX_MAX
void display_update_all(const unsigned char* data, bool reverse);
void display_update_index(unsigned char data, char index);
#endif