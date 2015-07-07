#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* default entry point in C */
extern "C" void Reset_Handler();
/* from linker script */
extern char __StackTop;

typedef struct {
  uint32_t stacktop;
  uint32_t entryPoint;

  uint32_t NMI_Handler;
  uint32_t HardFault_Handler;
  uint32_t MemManage_Handler;
  uint32_t BusFault_Handler;
  uint32_t UsageFault_Handler;
  uint32_t reserved_1;
  uint32_t reserved_2;
  uint32_t reserved_3;
  uint32_t reserved_4;
  uint32_t SVC_Handle;
  uint32_t DebugMon_Handler;
  uint32_t reserved_5;
  uint32_t PendSV_Handler;
  uint32_t SysTick_Handler;

  uint32_t WWDG_IRQHandler;
  uint32_t PVD_IRQHandler;
  uint32_t TAMP_STAMP_IRQHandler;
  uint32_t RTC_WKUP_IRQHandler;
  uint32_t FLASH_IRQHandler;
  uint32_t RCC_IRQHandler;
  uint32_t EXTI0_IRQHandler;
  uint32_t EXTI1_IRQHandler;
  uint32_t EXTI2_IRQHandler;
  uint32_t EXTI3_IRQHandler;
  uint32_t EXTI4_IRQHandler;
  uint32_t DMA1_Stream0_IRQHandler;
  uint32_t DMA1_Stream1_IRQHandler;
  uint32_t DMA1_Stream2_IRQHandler;
  uint32_t DMA1_Stream3_IRQHandler;
  uint32_t DMA1_Stream4_IRQHandler;
  uint32_t DMA1_Stream5_IRQHandler;
  uint32_t DMA1_Stream6_IRQHandler;
  uint32_t ADC_IRQHandler;
  uint32_t CAN1_TX_IRQHandler;
  uint32_t CAN1_RX0_IRQHandler;
  uint32_t CAN1_RX1_IRQHandler;
  uint32_t CAN1_SCE_IRQHandler;
  uint32_t EXTI9_5_IRQHandler;
  uint32_t TIM1_BRK_TIM9_IRQHandler;
  uint32_t TIM1_UP_TIM10_IRQHandler;
  uint32_t TIM1_TRG_COM_TIM11_IRQHandler;
  uint32_t TIM1_CC_IRQHandler;
  uint32_t TIM2_IRQHandler;
  uint32_t TIM3_IRQHandler;
  uint32_t TIM4_IRQHandler;
  uint32_t I2C1_EV_IRQHandler;
  uint32_t I2C1_ER_IRQHandler;
  uint32_t I2C2_EV_IRQHandler;
  uint32_t I2C2_ER_IRQHandler;
  uint32_t SPI1_IRQHandler;
  uint32_t SPI2_IRQHandler;
  uint32_t USART1_IRQHandler;
  uint32_t USART2_IRQHandler;
  uint32_t USART3_IRQHandler;
  uint32_t EXTI15_10_IRQHandler;
  uint32_t RTC_Alarm_IRQHandler;
  uint32_t OTG_FS_WKUP_IRQHandler;
  uint32_t TIM8_BRK_TIM12_IRQHandler;
  uint32_t TIM8_UP_TIM13_IRQHandler;
  uint32_t TIM8_TRG_COM_TIM14_IRQHandler;
  uint32_t TIM8_CC_IRQHandler;
  uint32_t DMA1_Stream7_IRQHandler;
  uint32_t FSMC_IRQHandler;
  uint32_t SDIO_IRQHandler;
  uint32_t TIM5_IRQHandler;
  uint32_t SPI3_IRQHandler;
  uint32_t UART4_IRQHandler;
  uint32_t UART5_IRQHandler;
  uint32_t TIM6_DAC_IRQHandler;
  uint32_t TIM7_IRQHandler;
  uint32_t DMA2_Stream0_IRQHandler;
  uint32_t DMA2_Stream1_IRQHandler;
  uint32_t DMA2_Stream2_IRQHandler;
  uint32_t DMA2_Stream3_IRQHandler;
  uint32_t DMA2_Stream4_IRQHandler;
  uint32_t ETH_IRQHandler;
  uint32_t ETH_WKUP_IRQHandler;
  uint32_t CAN2_TX_IRQHandler;
  uint32_t CAN2_RX0_IRQHandler;
  uint32_t CAN2_RX1_IRQHandler;
  uint32_t CAN2_SCE_IRQHandler;
  uint32_t OTG_FS_IRQHandler;
  uint32_t DMA2_Stream5_IRQHandler;
  uint32_t DMA2_Stream6_IRQHandler;
  uint32_t DMA2_Stream7_IRQHandler;
  uint32_t USART6_IRQHandler;
  uint32_t I2C3_EV_IRQHandler;
  uint32_t I2C3_ER_IRQHandler;
  uint32_t OTG_HS_EP1_OUT_IRQHandler;
  uint32_t OTG_HS_EP1_IN_IRQHandler;
  uint32_t OTG_HS_WKUP_IRQHandler;
  uint32_t OTG_HS_IRQHandler;
  uint32_t DCMI_IRQHandler;
  uint32_t reserved_6;
  uint32_t HASH_RNG_IRQHandler;
  uint32_t FPU_IRQHandler;
} IsrVector;

extern "C" void defaultHandler()
{
  while(1);
}
__weak void NMI_Handler() { defaultHandler(); }
__weak void HardFault_Handler() { defaultHandler(); }
__weak void MemManage_Handler() { defaultHandler(); }
__weak void BusFault_Handler() { defaultHandler(); }
__weak void UsageFault_Handler() { defaultHandler(); }
__weak void SVC_Handle() { defaultHandler(); }
__weak void DebugMon_Handler() { defaultHandler(); }
__weak void PendSV_Handler() { defaultHandler(); }

extern "C" void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();

  if(HAL_GetTick() % 500 == 0) {
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
  }
}

extern "C" void DMA1_Stream3_IRQHandler()
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_DMA_IRQHandler(&hMemsMicroDma);
}

__weak void WWDG_IRQHandler() { defaultHandler(); }
__weak void PVD_IRQHandler() { defaultHandler(); }
__weak void TAMP_STAMP_IRQHandler() { defaultHandler(); }
__weak void RTC_WKUP_IRQHandler() { defaultHandler(); }
__weak void FLASH_IRQHandler() { defaultHandler(); }
__weak void RCC_IRQHandler() { defaultHandler(); }
__weak void EXTI0_IRQHandler() { defaultHandler(); }
__weak void EXTI1_IRQHandler() { defaultHandler(); }
__weak void EXTI2_IRQHandler() { defaultHandler(); }
__weak void EXTI3_IRQHandler() { defaultHandler(); }
__weak void EXTI4_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream0_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream1_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream2_IRQHandler() { defaultHandler(); }

__weak void DMA1_Stream4_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream5_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream6_IRQHandler() { defaultHandler(); }
__weak void ADC_IRQHandler() { defaultHandler(); }
__weak void CAN1_TX_IRQHandler() { defaultHandler(); }
__weak void CAN1_RX0_IRQHandler() { defaultHandler(); }
__weak void CAN1_RX1_IRQHandler() { defaultHandler(); }
__weak void CAN1_SCE_IRQHandler() { defaultHandler(); }
__weak void EXTI9_5_IRQHandler() { defaultHandler(); }
__weak void TIM1_BRK_TIM9_IRQHandler() { defaultHandler(); }
__weak void TIM1_UP_TIM10_IRQHandler() { defaultHandler(); }
__weak void TIM1_TRG_COM_TIM11_IRQHandler() { defaultHandler(); }
__weak void TIM1_CC_IRQHandler() { defaultHandler(); }
__weak void TIM2_IRQHandler() { defaultHandler(); }
__weak void TIM3_IRQHandler() { defaultHandler(); }
__weak void TIM4_IRQHandler() { defaultHandler(); }
__weak void I2C1_EV_IRQHandler() { defaultHandler(); }
__weak void I2C1_ER_IRQHandler() { defaultHandler(); }
__weak void I2C2_EV_IRQHandler() { defaultHandler(); }
__weak void I2C2_ER_IRQHandler() { defaultHandler(); }
__weak void SPI1_IRQHandler() { defaultHandler(); }
__weak void SPI2_IRQHandler() { defaultHandler(); }
__weak void USART1_IRQHandler() { defaultHandler(); }
__weak void USART2_IRQHandler() { defaultHandler(); }
__weak void USART3_IRQHandler() { defaultHandler(); }
__weak void EXTI15_10_IRQHandler() { defaultHandler(); }
__weak void RTC_Alarm_IRQHandler() { defaultHandler(); }
__weak void OTG_FS_WKUP_IRQHandler() { defaultHandler(); }
__weak void TIM8_BRK_TIM12_IRQHandler() { defaultHandler(); }
__weak void TIM8_UP_TIM13_IRQHandler() { defaultHandler(); }
__weak void TIM8_TRG_COM_TIM14_IRQHandler() { defaultHandler(); }
__weak void TIM8_CC_IRQHandler() { defaultHandler(); }
__weak void DMA1_Stream7_IRQHandler() { defaultHandler(); }
__weak void FSMC_IRQHandler() { defaultHandler(); }
__weak void SDIO_IRQHandler() { defaultHandler(); }
__weak void TIM5_IRQHandler() { defaultHandler(); }
__weak void SPI3_IRQHandler() { defaultHandler(); }
__weak void UART4_IRQHandler() { defaultHandler(); }
__weak void UART5_IRQHandler() { defaultHandler(); }
__weak void TIM6_DAC_IRQHandler() { defaultHandler(); }
__weak void TIM7_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream0_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream1_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream2_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream3_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream4_IRQHandler() { defaultHandler(); }
__weak void ETH_IRQHandler() { defaultHandler(); }
__weak void ETH_WKUP_IRQHandler() { defaultHandler(); }
__weak void CAN2_TX_IRQHandler() { defaultHandler(); }
__weak void CAN2_RX0_IRQHandler() { defaultHandler(); }
__weak void CAN2_RX1_IRQHandler() { defaultHandler(); }
__weak void CAN2_SCE_IRQHandler() { defaultHandler(); }
__weak void OTG_FS_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream5_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream6_IRQHandler() { defaultHandler(); }
__weak void DMA2_Stream7_IRQHandler() { defaultHandler(); }
__weak void USART6_IRQHandler() { defaultHandler(); }
__weak void I2C3_EV_IRQHandler() { defaultHandler(); }
__weak void I2C3_ER_IRQHandler() { defaultHandler(); }
__weak void OTG_HS_EP1_OUT_IRQHandler() { defaultHandler(); }
__weak void OTG_HS_EP1_IN_IRQHandler() { defaultHandler(); }
__weak void OTG_HS_WKUP_IRQHandler() { defaultHandler(); }
__weak void OTG_HS_IRQHandler() { defaultHandler(); }
__weak void DCMI_IRQHandler() { defaultHandler(); }
__weak void HASH_RNG_IRQHandler() { defaultHandler(); }
__weak void FPU_IRQHandler() { defaultHandler(); }

extern "C" const IsrVector interruptVectorTable [[gnu::section (".isr_vector")]] [[gnu::used]] = {
  (uint32_t)&__StackTop,
  (uint32_t)&Reset_Handler,

  (uint32_t)&NMI_Handler,
  (uint32_t)&HardFault_Handler,
  (uint32_t)&MemManage_Handler,
  (uint32_t)&BusFault_Handler,
  (uint32_t)&UsageFault_Handler,
  0,
  0,
  0,
  0,
  (uint32_t)&SVC_Handle,
  (uint32_t)&DebugMon_Handler,
  0,
  (uint32_t)&PendSV_Handler,
  (uint32_t)&SysTick_Handler,

  (uint32_t)&WWDG_IRQHandler,
  (uint32_t)&PVD_IRQHandler,
  (uint32_t)&TAMP_STAMP_IRQHandler,
  (uint32_t)&RTC_WKUP_IRQHandler,
  (uint32_t)&FLASH_IRQHandler,
  (uint32_t)&RCC_IRQHandler,
  (uint32_t)&EXTI0_IRQHandler,
  (uint32_t)&EXTI1_IRQHandler,
  (uint32_t)&EXTI2_IRQHandler,
  (uint32_t)&EXTI3_IRQHandler,
  (uint32_t)&EXTI4_IRQHandler,
  (uint32_t)&DMA1_Stream0_IRQHandler,
  (uint32_t)&DMA1_Stream1_IRQHandler,
  (uint32_t)&DMA1_Stream2_IRQHandler,
  (uint32_t)&DMA1_Stream3_IRQHandler,
  (uint32_t)&DMA1_Stream4_IRQHandler,
  (uint32_t)&DMA1_Stream5_IRQHandler,
  (uint32_t)&DMA1_Stream6_IRQHandler,
  (uint32_t)&ADC_IRQHandler,
  (uint32_t)&CAN1_TX_IRQHandler,
  (uint32_t)&CAN1_RX0_IRQHandler,
  (uint32_t)&CAN1_RX1_IRQHandler,
  (uint32_t)&CAN1_SCE_IRQHandler,
  (uint32_t)&EXTI9_5_IRQHandler,
  (uint32_t)&TIM1_BRK_TIM9_IRQHandler,
  (uint32_t)&TIM1_UP_TIM10_IRQHandler,
  (uint32_t)&TIM1_TRG_COM_TIM11_IRQHandler,
  (uint32_t)&TIM1_CC_IRQHandler,
  (uint32_t)&TIM2_IRQHandler,
  (uint32_t)&TIM3_IRQHandler,
  (uint32_t)&TIM4_IRQHandler,
  (uint32_t)&I2C1_EV_IRQHandler,
  (uint32_t)&I2C1_ER_IRQHandler,
  (uint32_t)&I2C2_EV_IRQHandler,
  (uint32_t)&I2C2_ER_IRQHandler,
  (uint32_t)&SPI1_IRQHandler,
  (uint32_t)&SPI2_IRQHandler,
  (uint32_t)&USART1_IRQHandler,
  (uint32_t)&USART2_IRQHandler,
  (uint32_t)&USART3_IRQHandler,
  (uint32_t)&EXTI15_10_IRQHandler,
  (uint32_t)&RTC_Alarm_IRQHandler,
  (uint32_t)&OTG_FS_WKUP_IRQHandler,
  (uint32_t)&TIM8_BRK_TIM12_IRQHandler,
  (uint32_t)&TIM8_UP_TIM13_IRQHandler,
  (uint32_t)&TIM8_TRG_COM_TIM14_IRQHandler,
  (uint32_t)&TIM8_CC_IRQHandler,
  (uint32_t)&DMA1_Stream7_IRQHandler,
  (uint32_t)&FSMC_IRQHandler,
  (uint32_t)&SDIO_IRQHandler,
  (uint32_t)&TIM5_IRQHandler,
  (uint32_t)&SPI3_IRQHandler,
  (uint32_t)&UART4_IRQHandler,
  (uint32_t)&UART5_IRQHandler,
  (uint32_t)&TIM6_DAC_IRQHandler,
  (uint32_t)&TIM7_IRQHandler,
  (uint32_t)&DMA2_Stream0_IRQHandler,
  (uint32_t)&DMA2_Stream1_IRQHandler,
  (uint32_t)&DMA2_Stream2_IRQHandler,
  (uint32_t)&DMA2_Stream3_IRQHandler,
  (uint32_t)&DMA2_Stream4_IRQHandler,
  (uint32_t)&ETH_IRQHandler,
  (uint32_t)&ETH_WKUP_IRQHandler,
  (uint32_t)&CAN2_TX_IRQHandler,
  (uint32_t)&CAN2_RX0_IRQHandler,
  (uint32_t)&CAN2_RX1_IRQHandler,
  (uint32_t)&CAN2_SCE_IRQHandler,
  (uint32_t)&OTG_FS_IRQHandler,
  (uint32_t)&DMA2_Stream5_IRQHandler,
  (uint32_t)&DMA2_Stream6_IRQHandler,
  (uint32_t)&DMA2_Stream7_IRQHandler,
  (uint32_t)&USART6_IRQHandler,
  (uint32_t)&I2C3_EV_IRQHandler,
  (uint32_t)&I2C3_ER_IRQHandler,
  (uint32_t)&OTG_HS_EP1_OUT_IRQHandler,
  (uint32_t)&OTG_HS_EP1_IN_IRQHandler,
  (uint32_t)&OTG_HS_WKUP_IRQHandler,
  (uint32_t)&OTG_HS_IRQHandler,
  (uint32_t)&DCMI_IRQHandler,
  0,
  (uint32_t)&HASH_RNG_IRQHandler,
  (uint32_t)&FPU_IRQHandler
};

#ifdef __cplusplus
}
#endif


