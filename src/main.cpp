#include "stm32f4xx.h"
#include <cstdint>
#include <cstring>
#include <cassert>

volatile int64_t a = 10ll, b = 5ll, c = 0;

int main() {

  while (1) {
    CRC->DR = a / b;
  }
}
