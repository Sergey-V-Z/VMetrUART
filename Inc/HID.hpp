#include "stm32f1xx.h"
#include "FreeRTOS.h" 

#define list_size 5
// templates
#define STOP  "stop" 
#define START  "start"
#define RESULT  "result"
#define STATUS  "status"

#define CH0  "ch0" 
#define CH1  "ch1"
#define CH2  "ch2"
#define CH3  "ch3"

#define NONE  "none" 
#define RMS  "rms"
#define AVG  "avg"

#define IDLE  "idle" 
#define MEASURE  "measure"
#define ERROR  "error"

//typedef
enum CMD{
  stop,
  start,
  result,
  status,
  endCMD
};

enum chanel{
  ch0,
  ch1,
  ch2,
  ch3   
};

enum calculation{
  none,
  rms,
  avg
};

enum stat {
  idle,
  measure,
  error
};

//******************
// CLASS: list
//
// DESCRIPTION:
//  --
//
// CREATED: 23.08.2020, by Ierixon-HP
//
// FILE: HID.hpp
//
class list
{
public:
  int operator +=(string s);
  string& operator [](uint32_t i);
  string& extractFromStart();
  void Clear(void);
  list();
  ~list();
  
private:
  string arrList[list_size];
  string NULLstr = "";
  uint32_t indexEnd = 0;
  uint32_t indexStart = 0;

  
};

//******************
// CLASS: HID
//
// DESCRIPTION:
//  Human interface device
//
// CREATED: 22.08.2020, by Ierixon-HP
//
// FILE: HID.cpp
//
class HID
{
public:
  HID(USART &_usart, cADC &_adc);
  ~HID();
  void vTaskHID (void);
  
  
private:
  int sepCMD(string);
  void action(void);
  double calcAVG();
  double calcRMS();
  int Parser (void);
  
  USART         &usart;
  cADC          &adc;
  list          List;
  //string        strStatus = "ok";
  CMD           cmd;
  chanel        CHx;
  calculation   calc;
  stat        Status;
  
  
};

