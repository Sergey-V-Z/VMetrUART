#include "stm32f1xx.h"
#include "FreeRTOS.h" 
#include <string>

using namespace std;

//******************
// CLASS: 
//
// DESCRIPTION:
//  --
//
// CREATED: 19.08.2020, by Ierixon-HP
//
// FILE: USART.hpp
//
class USART
{
public:
   USART(uint16_t numUSART, uint32_t BaudRate);
  ~USART();
  int Init(void);
  void inByte(uint8_t byte);
  int WriteString(void);
  int WriteString(string txData);
  void TxEnd(void);
  bool newCMD(void);
  string readCMD(void);
  
private:
  const string invite = ">>:";
  const string answer = "<<:";
  string txString = ">>:";
  uint16_t usartNum = 0;
  uint32_t BaudRate = 115200;
  string CMD = "";
  bool flagCR = false;
  bool endCMD = false;
  bool txEnd = true;
 
};

