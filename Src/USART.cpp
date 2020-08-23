#include "main.hpp"

int USART::Init(){
  switch(usartNum)
  {
    case 1: // USART1
    {
      volatile uint32_t tmp = 0;
      uint32_t brr = 0;
    
      SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
      tmp = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
      
      SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
      tmp = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
      MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10 | GPIO_CRH_CNF10_1 | GPIO_CRH_CNF9_0,
                              GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_0);
      
      NVIC_EnableIRQ(USART1_IRQn);
                              
      //USART1 DMA Init for recive
      SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
      //Set transfer direction (Memory to Peripheral)
      MODIFY_REG(DMA1_Channel4->CCR, DMA_CCR_MEM2MEM, DMA_CCR_DIR);
      //Set priority level
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_PL);
      //Transfer mode NORMAL
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_CIRC);
      //Set peripheral no increment mode
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_PINC);
      //Set memory increment mode
      SET_BIT(DMA1_Channel4->CCR, DMA_CCR_MINC);
      //Set peripheral data width
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_PSIZE_1 | DMA_CCR_PSIZE_0);
      //Set memory data width
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_MSIZE_1 | DMA_CCR_MSIZE_0);
      
      //Disable DMA channels
      CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_EN);
      //Clear Channel 4 global interrupt flag
      WRITE_REG(DMA1->IFCR, DMA_IFCR_CGIF4);
      //Clear Channel 4  transfer complete flag
      WRITE_REG(DMA1->IFCR, DMA_IFCR_CTCIF4);
      //Clear Channel 4 transfer error flag
      WRITE_REG(DMA1->IFCR, DMA_IFCR_CTEIF4);

      //Enable DMA Mode for transmission
      SET_BIT(USART1->CR3, USART_CR3_DMAT);      
      //Enable Channel 4 Transfer complete interrupt
      SET_BIT(DMA1_Channel4->CCR, DMA_CCR_TCIE);
      //Enable Channel 4 Transfer error interrupt
      SET_BIT(DMA1_Channel4->CCR, DMA_CCR_TEIE);

      //Configure the Source and Destination addresses
      WRITE_REG(DMA1_Channel4->CPAR, (uint32_t)&(USART1->DR));
      WRITE_REG(DMA1_Channel4->CMAR, (uint32_t)&txString);
      
      if(READ_BIT(USART1->CR1, USART_CR1_UE) != USART_CR1_UE){
        MODIFY_REG(USART1->CR1, USART_CR1_M | USART_CR1_PCE | USART_CR1_PS,
                                USART_CR1_TE | USART_CR1_RE | USART_CR1_PEIE | USART_CR1_RE | USART_CR1_RXNEIE);
        SET_BIT(USART1->CR3, USART_CR3_EIE);
      }
      
      CLEAR_BIT(USART1->CR2, USART_CR2_LINEN | USART_CR2_CLKEN);
      CLEAR_BIT(USART1->CR3, USART_CR3_SCEN | USART_CR3_IREN | USART_CR3_HDSEL);
      
      brr = configCPU_CLOCK_HZ + (BaudRate >> 1);
      brr /= BaudRate;
      WRITE_REG(USART1->BRR, brr); //((0X27)<<4|0x01)
                
      SET_BIT(USART1->CR1, USART_CR1_UE); 
      WriteString();
      return 1;
    }
    case 2:
    {
      return 0;
    }
    default:
    {
      return 0;
    }
    
  }
}

void USART::inByte(uint8_t byte){
  if((endCMD == true) & !(byte == 0x0D | byte == 0x00 | byte == 0x0A)){
    CMD.clear();
    endCMD = false;
  }
  
  if(byte == 0x0D | byte == 0x00 | byte == 0x0A){
    endCMD = true;
  }else{
    CMD.push_back((char)byte);  
  }
}
      
int USART::WriteString(void){
  if(txEnd){
    
    txEnd = false;
    //Disable DMA channel 4
    CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_EN);
    //Set Number of data to transfer
    MODIFY_REG(DMA1_Channel4->CNDTR, DMA_CNDTR_NDT, txString.length());
    //Enable DMA channel 4
    SET_BIT(DMA1_Channel4->CCR, DMA_CCR_EN); 
    return 1;
  }
  else return 0;
}

int USART::WriteString(string txData){
  txString.append(answer);
  txString.append(txData);
  txString.append("\r\n");
  txString.append(invite);
  return WriteString();
}

void USART::TxEnd(void){
  txString.clear();
  txEnd = true;
}

bool USART::newCMD(void){
  return endCMD;
}

string USART::readCMD(void){
  endCMD = false;
  string tmpCMD = CMD;
  CMD.clear();
  return tmpCMD;
}

USART::USART(uint16_t numUSART, uint32_t BaudRate): usartNum(numUSART), BaudRate(BaudRate){

}

USART::~USART(){
  
}

