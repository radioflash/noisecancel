#include "hal.h"

HAL_StatusTypeDef audio_receive(uint16_t *rxbuffer, int samples)
{
  return HAL_I2S_Receive_DMA(&hMemsMicro, rxbuffer, samples);
}


