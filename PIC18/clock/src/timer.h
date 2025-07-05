#ifndef TIMER_H
#define TIMER_H

// Clock 18.432 Mhz => Instruction Clock = Clock / 4 = 4.608 Mhz
// In Timer 0, Using a 256x prescaler we have 18000 ticks per second
// For 500ms -> 65536 - 9000 = 56536  (0xDCD8)
// with 9000 was 1/7200 fast so using (0xDCD7)
// with 9001 was 1/36000 fast (27uS), which is in the range of xtal drift
// with 9002 will be 1/36000 slow, which is in the range of xtal drift
// The Strategy is to include a fine tune

// Timer 0
#define TMR0_PRESCALER      0b111  // 256X prescaler
#define TMR0H_500MS         0xDC
#define TMR0L_500MS_FAST    0xD7
#define TMR0L_500MS_SLOW    0xD6

// In Timer 1, Using a 8x prescaler we have 576000 ticks per second
// For 5ms -> 65536 - 2880 =  (0xF4C0)
#define TMR1_PRESCALER 0b00110000 //8X
#define TMR1H_5MS      0xF4
#define TMR1L_5MS      0xC0

// Maximum number of callbacks
#define TIMER0_CALLBACK_MAX 1
#define TIMER1_CALLBACK_MAX 3

#if TIMER0_CALLBACK_MAX == 1
  #define TIMER0_CALLBACK_INIT { timer_dummy_callback }
#elif TIMER0_CALLBACK_MAX == 2
  #define TIMER0_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback }
#elif TIMER0_CALLBACK_MAX == 3
  #define TIMER0_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback, timer_dummy_callback }
#elif TIMER0_CALLBACK_MAX == 4
  #define TIMER0_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback, timer_dummy_callback, timer_dummy_callback }
#else
  #error "TIMER0_CALLBACK_MAX not supported yet for that size"
#endif

#if TIMER1_CALLBACK_MAX == 1
  #define TIMER1_CALLBACK_INIT { timer_dummy_callback }
#elif TIMER1_CALLBACK_MAX == 2
  #define TIMER1_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback }
#elif TIMER1_CALLBACK_MAX == 3
  #define TIMER1_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback, timer_dummy_callback }
#elif TIMER1_CALLBACK_MAX == 4
  #define TIMER1_CALLBACK_INIT { timer_dummy_callback, timer_dummy_callback, timer_dummy_callback, timer_dummy_callback }
#else
  #error "TIMER1_CALLBACK_MAX not supported yet for that size"
#endif

typedef void(*timer_callback)(void);

void timer0_init(signed char fine_tune);
void timer_dummy_callback(void);
void timer0_adjust_set(signed char fine_tune);
signed char timer0_adjust_get(void);
void timer1_init(void);
char timer0_register_callback(timer_callback callback);
char timer1_register_callback(timer_callback callback);
void timer0_handler(void);
void timer1_handler(void);

#endif