/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void USART1_Data(uint8_t);
void DMA1_Fin(void);
void ADC_EOC(void);
void ADC_Fin(void);
/* Private functions ---------------------------------------------------------*/


void USART1_IRQHandler(void)
{
  if((READ_BIT(USART1->SR, USART_SR_RXNE) == (USART_SR_RXNE)) && (READ_BIT(USART1->CR1, USART_CR1_RXNEIE) == (USART_CR1_RXNEIE)))
  {
    USART1_Data(USART1->DR);
  }
  else
  {
    if(READ_BIT(USART1->SR, USART_SR_ORE) == (USART_SR_ORE))
    {
      (void) USART1->DR;
    }
    else if(READ_BIT(USART1->SR, USART_SR_FE) == (USART_SR_FE))
    {
      (void) USART1->DR;
    }
    else if(READ_BIT(USART1->SR, USART_SR_ORE) == (USART_SR_ORE))
    {
      (void) USART1->DR;
    }
  }
}

void DMA1_Channel4_IRQHandler(void)
{
  if(READ_BIT(DMA1->ISR, DMA_ISR_TCIF4) == (DMA_ISR_TCIF4))
  {
    WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF4);
    USARTtx_Fin();
  }
  else if(READ_BIT(DMA1->ISR, DMA_ISR_TEIF4) == (DMA_ISR_TEIF4))
  {
    //Disable DMA channels
    CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_EN);
  }
}

void DMA1_Channel1_IRQHandler(void)
{
  if(READ_BIT(DMA1->ISR, DMA_ISR_TCIF1) == (DMA_ISR_TCIF1))
  {
    WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF1);
    ADC_Fin();
  }
  else if(READ_BIT(DMA1->ISR, DMA_ISR_TEIF1) == (DMA_ISR_TEIF1))
  {
    //Disable DMA channels
    CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
  }
}
void ADC1_2_IRQHandler(void)
{
   if(READ_BIT(ADC1->SR, ADC_SR_JEOC))
   {
      //записываем результат в переменную 
      //temp =ADC1->DR;
     ADC_EOC();
   }
   //сбрасываем все флаги в регистре статуса
   CLEAR_REG(ADC1->SR);
}