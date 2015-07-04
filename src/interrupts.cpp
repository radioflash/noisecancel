#include <stdint.h>

/* default entry point in C */
extern "C" void _init();
/* from linker script */
extern "C" char __StackTop;

typedef struct {
  uint32_t stacktop;
  uint32_t entryPoint;
} IsrVector;

extern "C" const IsrVector interruptVectorTable [[gnu::section (".isr_vector")]] [[gnu::used]] = {
  (uint32_t)&__StackTop,
  (uint32_t)&_init,
};


