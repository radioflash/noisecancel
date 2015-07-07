#include "hal.h"
#include "pdm_filter.h"

HAL_StatusTypeDef audio_receive(uint16_t *rxbuffer, int samples);
static uint16_t audio_data[];

int main()
{
  PDMFilter_InitStruct pdmFilter;
  PDM_Filter_Init(&pdmFilter);

  audio_receive(audio_data, sizeof(audio_data) / sizeof(*audio_data));

  while (1) {
    if(HAL_DMA_GetState(&hMemsMicroDma) == HAL_DMA_STATE_READY_MEM0) {
      PDM_Filter_64_MSB();
    }
  }
}
