#ifndef HAL_H_
#define HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

extern I2S_HandleTypeDef hMemsMicro;
extern DMA_HandleTypeDef hMemsMicroDma;

HAL_StatusTypeDef audio_receive(uint16_t *rxbuffer, int samples);

#ifdef __cplusplus
}
#endif

#endif /* HAL_H_ */
