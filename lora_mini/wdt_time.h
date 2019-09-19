#ifndef WDT_TIME_H
#define WDT_TIME_H 1
#include "cpu_clock.h"
//reboot
#define REBOOT_   asm volatile ("  rjmp 0")
#define _8Mhz 0
void power_down_8s() { //进入powerdown模式，8秒钟唤醒一次
  clock_prescale_set(_8Mhz); //powerdown时，时钟停止，所以可以最高速。加快操作
  power_adc_disable();
  // power_spi_disable();
  power_twi_disable();
  power_timer1_disable();
  power_timer2_disable();
  // lora_off();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();  //enter sleep mode
  sleep_disable();
}

uint8_t half_sec = 0, sec = 0, minute = 0, hour = 0, day = 0, min_dog = 0;
//8秒中断程序
ISR(WDT_vect) {
  half_sec++;
  if (half_sec < 4) return;
  // sec+=8;
  half_sec = 0;
  sec++;
  if (sec < 60) return;
  sec -= 60;
  minute++;
  min_dog++;
  if (min_dog > 10) REBOOT_;
  if (minute < 60) return;
  minute -= 60;
  hour++;
  if (hour < 24)return;
  hour -= 24;
  day++;
}

#if !defined(WDTCSR)
#if defined(WDTCR)
#  define WDTCSR     WDTCR
#else
#  define WDTCSR     WDT
#endif
#endif

#if !defined(WDIE)
# define WDIE 6
#endif

void setup_watchdog(int ii) {
  byte bb;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}
#endif
