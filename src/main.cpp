#include "stm32f4xx.h"
#include <cstdint>
#include <cstring>

#define FLASH_SET_LATENCY(__LATENCY__) (*(__IO uint8_t *)ACR_BYTE0_ADDRESS = (uint8_t)(__LATENCY__))

int main();

extern "C" void __libc_init_array();
extern "C" char __bss_start__, __bss_end__;
extern "C" char __bss2_start__, __bss2_end__;
extern "C" char __etext, __data_start__, __data_end__;
extern "C" char __etext2, __data2_start__, __data2_end__;

extern "C" void startup()
{
  /* clock setup */

  FLASH_SET_LATENCY(FLASH_ACR_LATENCY_7WS);

  /* enable instruction cache */
  FLASH->ACR |= FLASH_ACR_ICEN;

  /* enable data cache */
  FLASH->ACR |= FLASH_ACR_DCEN;


  memcpy(&__data_start__, &__etext, &__data_end__ - &__data_start__);
  memcpy(&__data2_start__, &__etext2, &__data2_end__ - &__data2_start__);
  memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
  memset(&__bss2_start__, 0, &__bss2_end__ - &__bss2_start__);

  __libc_init_array();

  main();
}

volatile int64_t a = 10ll, b = 5ll, c = 0;

int main() {
  while (1) {
    c = a / b;
  }
}
