#ifndef TIMER_H
#define TIMER_H

// Clock 18.432 Mhz => Instruction Clock 4.608 Mhz
// Using a 256x prescaler we have 18000 ticks per second
// For 5ms -> 65536 - 90 = 65446 (0xFFA6) (+ 1 from overflow) -> (0xFFA7)
#define TMRH_PRESCALER 0b111  // 256X prescaler
#define TMRH_5MS 0xFF
#define TMRL_5MS 0xA6

// Maximum number of callbacks
#define TIMER0_CALLBACK_MAX 3

#if TIMER0_CALLBACK_MAX == 1
  #define TIMER0_CALLBACK_INIT { NULL }
#elif TIMER0_CALLBACK_MAX == 2
  #define TIMER0_CALLBACK_INIT { NULL, NULL }
#elif TIMER0_CALLBACK_MAX == 3
  #define TIMER0_CALLBACK_INIT { NULL, NULL, NULL }
#elif TIMER0_CALLBACK_MAX == 4
  #define TIMER0_CALLBACK_INIT { NULL, NULL, NULL, NULL }
#else
  #error "TIMER0_CALLBACK_MAX not supported yet for that size"
#endif

typedef void(*timer0_callback)(void);

void timer0_init(void);
int  timer0_register_callback(timer0_callback callback);
void timer0_handler(void);

#endif