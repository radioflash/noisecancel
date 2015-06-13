#include <cstdint>
#include <cstring>

int main();

extern "C" void __libc_init_array();
extern "C" char __bss_start__, __bss_end__;
extern "C" char __bss2_start__, __bss2_end__;
extern "C" char __etext, __data_start__, __data_end__;
extern "C" char __etext2, __data2_start__, __data2_end__;

extern "C" void startup()
{
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
