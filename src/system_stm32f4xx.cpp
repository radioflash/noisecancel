#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include <cstring>
#include <cassert>


#define MEMS_MICRO_DMA_CLK_ENABLE       __DMA1_CLK_ENABLE
#define MEMS_MICRO_DMA_STREAM           DMA1_Stream3
#define MEMS_MICRO_DMA_CHANNEL          DMA_CHANNEL_0
#define MEMS_MICRO_DMA_IRQ              DMA1_Stream3_IRQn
#define MEMS_MICRO_DMA_PERIPH_DATA_SIZE DMA_PDATAALIGN_HALFWORD
#define MEMS_MICRO_DMA_MEM_DATA_SIZE    DMA_MDATAALIGN_HALFWORD
#define MEMS_MICRO_DMA_IRQ_PREPRIO      0
#define MEMS_MICRO_DMA_IRQHandler       DMA1_Stream3_IRQHandler

#define MEMS_MICRO_I2S                  SPI2
#define MEMS_MICRO_CLK_ENABLE           __SPI2_CLK_ENABLE


uint32_t SystemCoreClock = 168000000;
__IO const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

I2S_HandleTypeDef hMemsMicro;
DMA_HandleTypeDef hMemsMicroDma;

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
  (void)file;
  (void)line;
  (void)func;
  (void)expr;

  /* reinitialize and enable red LED (discovery board) */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

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

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Configure the SysTick to have interrupt in 1us time basis*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  /*Configure the SysTick IRQ priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0);

  /* Return function status */
  return HAL_OK;
}

static void clock_init()
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  //HAL_InitTick(0);

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* configure I2S clock for 48kHz audio frequency */
  RCC_PeriphCLKInitTypeDef rccclkinit;
  HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);
  rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  rccclkinit.PLLI2S.PLLI2SN = 192;
  rccclkinit.PLLI2S.PLLI2SR = 4;
  HAL_RCCEx_PeriphCLKConfig(&rccclkinit);

  HAL_InitTick(0);
}

void mems_micro_dma_init()
{
  /* discovery MEMS microphone, I2S2 DMA1 Channel 0: PC3 */
  MEMS_MICRO_DMA_CLK_ENABLE();

    /* Configure the hdma_i2sRx handle parameters */
  hMemsMicroDma.Init.Channel             = MEMS_MICRO_DMA_CHANNEL;
  hMemsMicroDma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hMemsMicroDma.Init.PeriphInc           = DMA_PINC_DISABLE;
  hMemsMicroDma.Init.MemInc              = DMA_MINC_ENABLE;
  hMemsMicroDma.Init.PeriphDataAlignment = MEMS_MICRO_DMA_PERIPH_DATA_SIZE;
  hMemsMicroDma.Init.MemDataAlignment    = MEMS_MICRO_DMA_MEM_DATA_SIZE;
  hMemsMicroDma.Init.Mode                = DMA_CIRCULAR;
  hMemsMicroDma.Init.Priority            = DMA_PRIORITY_HIGH;
  hMemsMicroDma.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hMemsMicroDma.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hMemsMicroDma.Init.MemBurst            = DMA_MBURST_SINGLE;
  hMemsMicroDma.Init.PeriphBurst         = DMA_MBURST_SINGLE;

  hMemsMicroDma.Instance = MEMS_MICRO_DMA_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(&hMemsMicro, hdmarx, hMemsMicroDma);

  /* Deinitialize the Stream for new transfer */
  assert(HAL_OK == HAL_DMA_DeInit(&hMemsMicroDma));

  /* Configure the DMA Stream */
  assert(HAL_OK == HAL_DMA_Init(&hMemsMicroDma));

  /* I2S DMA IRQ Channel configuration */
  HAL_NVIC_SetPriority(MEMS_MICRO_DMA_IRQ, MEMS_MICRO_DMA_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(MEMS_MICRO_DMA_IRQ);
}

void gpio_init()
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  /* discovery button: PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* discovery LEDs: PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void mems_micro_gpio_init()
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

  /* discovery MEMS microphone, I2S2_CK: PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* discovery MEMS microphone, I2S2_SD: PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void i2s_init()
{
  MEMS_MICRO_CLK_ENABLE();

  /* Initialize the hAudioInI2s Instance parameter */
  hMemsMicro.Instance          = MEMS_MICRO_I2S;

  /* Disable I2S block */
  __HAL_I2S_DISABLE(&hMemsMicro);

  /* I2S2 peripheral configuration */
  hMemsMicro.Init.AudioFreq    = I2S_AUDIOFREQ_48K;
  hMemsMicro.Init.ClockSource  = I2S_CLOCK_PLL;
  hMemsMicro.Init.CPOL         = I2S_CPOL_HIGH;
  hMemsMicro.Init.DataFormat   = I2S_DATAFORMAT_16B;
  hMemsMicro.Init.MCLKOutput   = I2S_MCLKOUTPUT_DISABLE;
  hMemsMicro.Init.Mode         = I2S_MODE_MASTER_RX;
  hMemsMicro.Init.Standard     = I2S_STANDARD_LSB;

  mems_micro_gpio_init();
  mems_micro_dma_init();

  assert(HAL_I2S_Init(&hMemsMicro) == HAL_OK);
}

extern "C" void _init() {}

extern "C" void Reset_Handler()
{
  /* 4 bits for preemption priority */
  NVIC_SetPriorityGrouping(4);

  /* enable data/instruction cache, set wait cycles, set flash latency */
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

  assert(READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY) == FLASH_ACR_LATENCY_5WS);

  /* enable FPU if needed */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  /* enable coprocessors CP10 and CP11 */
  SCB->CPACR |= (0x0f << 20);
#endif

  clock_init();

  /* data initialization */
  memcpy(&__data_start__, &__etext, (intptr_t)&__data_end__ - (intptr_t)&__data_start__);
  memcpy(&__data2_start__, &__etext2, (intptr_t)&__data2_end__ - (intptr_t)&__data2_start__);
  memset(&__bss_start__, 0, (intptr_t)&__bss_end__ - (intptr_t)&__bss_start__);
  memset(&__bss2_start__, 0, (intptr_t)&__bss2_end__ - (intptr_t)&__bss2_start__);

  /* set interrupt vector table offset */
  SCB->VTOR = (uint32_t)&interruptVectorTable;

  /* c++ constructors */
  __libc_init_array();

  gpio_init();
  i2s_init();

  main();
}




