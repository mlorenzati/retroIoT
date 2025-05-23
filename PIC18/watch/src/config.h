#ifndef CONFIG_H
#define CONFIG_H
#include <htc.h>

// PIC 18F252 configs https://gputils.sourceforge.io/html-help/PIC18F252-conf.html

// CONFIG1H - Oscillator: HS, switching disabled
__CONFIG(1, OSC_HS & OSCS_OFF);

// CONFIG2L - Power-up Timer ON, BOR ON, 4.2V
__CONFIG(2, PWRT_ON & BOR_ON & BORV_42 & WDT_OFF & WDTPS_128);

// CONFIG3H - CCP2 on RC1
__CONFIG(5, CCP2MUX_ON);

// CONFIG4L - Stack Overflow Reset ON, LVP OFF, Debug OFF
__CONFIG(6, STVR_ON & LVP_OFF & DEBUG_OFF);

// CONFIG5L - No code protection
__CONFIG(8, CP0_OFF & CP1_OFF & CP2_OFF & CP3_OFF);

// CONFIG5H - No boot/data EEPROM code protection
__CONFIG(9, CPB_OFF & CPD_OFF);

// CONFIG6L - No write protection
__CONFIG(10, WRT0_OFF & WRT1_OFF & WRT2_OFF & WRT3_OFF);

// CONFIG6H - No config/boot/data EEPROM write protection
__CONFIG(11, WRTC_OFF & WRTB_OFF & WRTD_OFF);

// CONFIG7L - No table read protection
__CONFIG(12, EBTR0_OFF & EBTR1_OFF & EBTR2_OFF & EBTR3_OFF);

// CONFIG7H - No boot block table read protection
__CONFIG(13, EBTRB_OFF);

#define FORCE_INCLUDE(fn) \
    do { if (0) { keyboard_status_t __ks = {0}; fn(__ks); } } while (0)

#endif