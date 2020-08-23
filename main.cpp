#include "main.hpp"


//Defines


//Prototype
void vTaskCMD_Parser (void *arg);
void SystemClockInit();
void RCC_Deinit();
void SysTick_Init ();
void ADC_Init();
void  DMA1_Init();
extern "C" {
  void USART1_Data(uint8_t);
  void USARTtx_Fin(void);
  void ADC_EOC(void);
  void ADC_Fin(void);
}
//objects
USART Uart(1, 115200);
cADC objADC;
HID objHID(Uart, objADC);

//variables
//uint16_t gBuff[100];

int main(void)
{
  RCC_Deinit();
  SystemClockInit();
  SysTick_Init ();
  DMA1_Init();
  Uart.Init();
  objADC.Init();
  //Uart.WriteString(">> Hello");
  
  xTaskCreate(vTaskCMD_Parser, "CMD_Parser", 1024, NULL, 1, NULL);
  
  vTaskStartScheduler();
  
  while (1)
  {
  
  }
}

void vTaskCMD_Parser (void *arg){
  //objADC.StartConv();
  objHID.vTaskHID();
}

void SystemClockInit(){

  //swd map
  CLEAR_BIT(AFIO->MAPR, AFIO_MAPR_SWJ_CFG);
  SET_BIT(AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
  
  //RCC init
  SET_BIT(RCC->CR, RCC_CR_HSEON);
  while(READ_BIT(RCC->CR, RCC_CR_HSERDY) != RCC_CR_HSERDY){}
  
  //Flash latency
  CLEAR_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
  SET_BIT (FLASH->ACR, FLASH_ACR_PRFTBE);
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
  
  //RCC init continuation
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL, RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);
  
  SET_BIT(RCC->CR, RCC_CR_PLLON);
  while(READ_BIT(RCC->CR, RCC_CR_PLLRDY != RCC_CR_PLLRDY)){}
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
  while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}

}

void RCC_Deinit(){
  SET_BIT(RCC->CR, RCC_CR_HSION);
  while(READ_BIT(RCC->CR, RCC_CR_HSIRDY) != RCC_CR_HSIRDY){}
  MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
  
  CLEAR_REG(RCC->CFGR);
  while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET){}

  CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
  while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET){}

  CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
  while(READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET){}
  
  CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
  //reset all CSR flags
  SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}

void SysTick_Init (){  
  MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, configCPU_CLOCK_HZ / configTICK_RATE_HZ - 1);
  CLEAR_BIT(SysTick->VAL, SysTick_VAL_CURRENT_Msk);
  SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

void  DMA1_Init(void)
{ 
  volatile uint32_t tmpreg = 0;
  //DMA controller clock enable
  SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
  tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
  (void)tmpreg;
  //DMA1_Channel4_IRQn interrupt init
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  //DMA1_Channel1_IRQn interrupt init
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

//Handlers
void USART1_Data(uint8_t b){
  Uart.inByte(b);
}

void USARTtx_Fin(){
  Uart.TxEnd();
}

void ADC_Fin(){
  //CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
  CLEAR_BIT(ADC1->CR2, ADC_CR2_SWSTART);
  objADC.endConvesion = true;
}

void ADC_EOC(){
  //objADC
}
