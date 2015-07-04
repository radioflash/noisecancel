#include "stm32f4xx.h"
#include <cstring>

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
  (void)file;
  (void)line;
  (void)func;
  (void)expr;

  while(1);
}

int main();

extern "C" char __bss_start__, __bss_end__;
extern "C" char __bss2_start__, __bss2_end__;
extern "C" char __etext, __data_start__, __data_end__;
extern "C" char __etext2, __data2_start__, __data2_end__;
struct IsrVector;
extern "C" IsrVector interruptVectorTable;

/* libc, performs C++ initialization */
extern "C" void __libc_init_array();
extern "C" void __Init_Data();

extern "C" void _init()
{
  /* enable data/instruction cache, set wait cycles, set flash latency */
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

  /* enable FPU if needed */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  /* enable coprocessors CP10 and CP11 */
  SCB->CPACR |= (0x0f << 20);
#endif

  //assert((uint32_t)&interruptVectorTable % 0x200 == 0);

  /* set interrupt vector table offset */
  SCB->VTOR = (uint32_t)&interruptVectorTable;

  memcpy(&__data_start__, &__etext, (intptr_t)&__data_end__ - (intptr_t)&__data_start__);
  memcpy(&__data2_start__, &__etext2, (intptr_t)&__data2_end__ - (intptr_t)&__data2_start__);
  memset(&__bss_start__, 0, (intptr_t)&__bss_end__ - (intptr_t)&__bss_start__);
  memset(&__bss2_start__, 0, (intptr_t)&__bss2_end__ - (intptr_t)&__bss2_start__);

  __libc_init_array();

  main();
}



