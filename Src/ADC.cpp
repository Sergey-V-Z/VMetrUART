#include "main.hpp"

extern uint32_t gBuff[100];

int cADC::Init(void){

  /***************************GPIO*******************************/
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
  //mod in and andlog
  CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3);
  CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3);
  /***************************ADC*******************************/
  //12MGhz and ADC en
  SET_BIT(RCC->CFGR, RCC_CFGR_ADCPRE_DIV6);
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
  
  //calibration
  SET_BIT(ADC1->CR2, ADC_CR2_ADON);
  SET_BIT(ADC1->CR2, ADC_CR2_RSTCAL);
  for(int i = 0; i < 2; ++i){}
  SET_BIT(ADC1->CR2, ADC_CR2_CAL); 
  while (!READ_BIT(ADC1->CR2, ADC_CR2_CAL)){}
  CLEAR_BIT(ADC1->CR2, ADC_CR2_ADON);
  
  //7.5 cycles
  SET_BIT(ADC1->SMPR2, ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP1_0
          | ADC_SMPR2_SMP2_0 | ADC_SMPR2_SMP3_0);
  
  // mode
  SET_BIT(ADC1->CR2, ADC_CR2_CONT);
  SET_BIT(ADC1->CR1, ADC_CR1_SCAN); 
  //SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG | ADC_CR2_DMA ); // timer start
  SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG | ADC_CR2_DMA ); // soft start
  
  //NVIC_EnableIRQ(ADC1_IRQn);
  //SET_BIT(ADC1->CR1, ADC_CR1_JEOCIE);

  //sequences
  CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L);
  CLEAR_BIT(ADC1->SQR3, ADC_SQR3_SQ1); //ch0
  
  SET_BIT(ADC1->CR2, ADC_CR2_ADON);
  
  /***************************DMA*******************************/
  //ADC1 DMA Init
  //Set transfer direction (Peripheral to Memory)
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_MEM2MEM | DMA_CCR_DIR);
  //Set priority level
  SET_BIT(DMA1_Channel1->CCR, DMA_CCR_PL);
  //Transfer mode Circulare
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_CIRC);
  //Set peripheral no increment mode
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_PINC);
  //Set memory increment mode
  SET_BIT(DMA1_Channel1->CCR, DMA_CCR_MINC);
  //Set peripheral data width
  MODIFY_REG(DMA1_Channel1->CCR, DMA_CCR_PSIZE_1, DMA_CCR_PSIZE_0);
  //Set memory data width
  MODIFY_REG(DMA1_Channel1->CCR, DMA_CCR_MSIZE_1, DMA_CCR_MSIZE_0);
  
  //Disable DMA channels
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
  //Clear Channel 1 global interrupt flag
  WRITE_REG(DMA1->IFCR, DMA_IFCR_CGIF1);
  //Clear Channel 1  transfer complete flag
  WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF1);
  //Clear Channel 1 transfer error flag
  WRITE_REG(DMA1->IFCR, DMA_IFCR_CTEIF1);
        
  //Enable Channel 1 Transfer complete interrupt
  SET_BIT(DMA1_Channel1->CCR, DMA_CCR_TCIE);
  //Enable Channel 1 Transfer error interrupt
  SET_BIT(DMA1_Channel1->CCR, DMA_CCR_TEIE);
  
  //Configure the Source and Destination addresses
  WRITE_REG(DMA1_Channel1->CPAR, (uint32_t)&(ADC1->DR));
  WRITE_REG(DMA1_Channel1->CMAR, (uint32_t)&buffer);
  
  /***************************TIM*******************************/
//  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);        
//  WRITE_REG(TIM3->PSC, 72-1);                    
//  WRITE_REG(TIM3->ARR, 1000-1);                    
//  SET_BIT(TIM3->CR2, TIM_CR2_MMS_1);            
//  SET_BIT(TIM3->DIER, TIM_DIER_UDE);
   
   
  return 1;  
}

void cADC::ChanelSet(uint32_t ch){
  
  switch(ch)
  {
  case 0:
    {
      CLEAR_BIT(ADC1->SQR3, ADC_SQR3_SQ1); //ch0
      break;
    }
  case 1:
    {
      MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1, ADC_SQR3_SQ1_0); //ch1
      break;
    }
  case 2:
    {
      MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1, ADC_SQR3_SQ1_1); //ch2
      break;
    }
  case 3:
    {
      MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1, ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_0); //ch3
      break;
    }
  default:
    {
      CLEAR_BIT(ADC1->SQR3, ADC_SQR3_SQ1); //ch0
      break;
    }
    
  }

}

void cADC::StartConv(){

  endConvesion = false;
  //settings DMA
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
  MODIFY_REG(DMA1_Channel1->CNDTR, DMA_CNDTR_NDT, rawLength);
  SET_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);

  SET_BIT(ADC1->CR2, ADC_CR2_SWSTART);
  
  //start timer
  //SET_BIT(TIM3->CR1, TIM_CR1_CEN);
  
}

void cADC::StopConv(){

  endConvesion = true;
  //settings DMA
  CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR_EN);
  MODIFY_REG(DMA1_Channel1->CNDTR, DMA_CNDTR_NDT, rawLength);

  CLEAR_BIT(ADC1->CR2, ADC_CR2_SWSTART);
  
}
 cADC::cADC(){
  
}

 cADC::~cADC(){
  
}

          