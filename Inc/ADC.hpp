#include "stm32f1xx.h"
#include "FreeRTOS.h"

using namespace std;

//******************
// CLASS: ADC
//
// DESCRIPTION:
//  --
//
// CREATED: 20.08.2020, by Ierixon-HP
//
// FILE: ADC.cpp
//
class cADC
{
public:
   cADC();
  ~cADC();
  
  int Init(void);
  void ChanelSet(uint32_t);
  void StartConv(void);
  void StopConv(void);

  uint32_t rawLength = 10;
  bool endConvesion = true;
  uint16_t buffer[100] = {0};
  
};