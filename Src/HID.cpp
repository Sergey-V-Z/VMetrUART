#include "main.hpp"


void HID::vTaskHID (){
  //objADC.StartConv();
  string tempStr;
  while(1){
    if(usart.newCMD()){
      if(Parser() < 1){
        usart.WriteString("error in command\n\r");
        Status = error;
      }else{
        action();
      }
    }
    adc.endConvesion ? Status = idle : Status = measure;
    vTaskDelay(1);
  }
}

int HID::Parser (){
  int ret = 0;
  ret = sepCMD(usart.readCMD());
  if(ret < 1){
    Status = error;
    return -1;
    
  }else{
    //CMD
    if(List[0] == STOP){
      cmd = stop; 
    }else if(List[0] == START){
      cmd = start;
    }else if(List[0] == RESULT){
      cmd = result;
    }else if(List[0] == STATUS){
      cmd = status;
    }else{
      Status = error;
      return -1;
    }
    
    //chanel
    if(List[1] == CH0){
      CHx = ch0;
    }else if(List[1] == CH1){
      CHx = ch1;
    }else if(List[1] == CH2){
      CHx = ch2;
    }else if(List[1] == CH3){
      CHx = ch3;
    }else{
      //Status = error;
      //return -1;
    }
    
    //calculation
    if(List[2] == NONE){
      calc = none;
    }else if(List[2] == RMS){
      calc = rms;
    }else if(List[2] == AVG){
      calc = avg;
    }else{
      //Status = error;
      //return -1;
    }
    
  }
  List.Clear();
  return 1;
}

int HID::sepCMD(string CMD){
  string seporator = " ";
  size_t pos = 0;
  size_t base_str_size = CMD.size();
  size_t delim_size = seporator.size();
  string temp;
  for (int i = 0; pos < base_str_size; i++) {
    if(i>list_size){return -1;}
    List += temp.assign(CMD, pos, CMD.find(seporator, pos) - pos);
    if (temp.size() == 0){return -1;} 
      pos += temp.size() + delim_size;
  }
  CMD.clear();
  return 1;
}

void HID::action(){
  switch(cmd)
  {
  case stop:
    {
      Status = idle;
      adc.StopConv();
      cmd = endCMD;
      usart.WriteString("ok");
      break;
    }
  case start:
    {
      Status = measure;
      adc.ChanelSet((uint32_t)CHx);
      adc.StartConv();
      cmd = endCMD;
      usart.WriteString("ok");
      break;
    }
  case result:
    {
      string outMessage;
      char str[100] = {0};

      switch(calc)
      {
      case none:
        {
          double data = adc.buffer[adc.rawLength-1]/4095;
          sprintf(str, "%.4lf", data);
          outMessage = str;
          usart.WriteString(outMessage);          
          break;
        }
      case rms:
        {
          double data = calcRMS()/4095;
          sprintf(str, "%.4lf", data);
          outMessage = str;
          usart.WriteString(outMessage);
          break;
        }
      case avg:
        {
          double data = calcAVG()/4095;
          sprintf(str, "%.4lf", data);
          outMessage = str;
          usart.WriteString(outMessage);
          break;
        }
      default:
        {
          break;
        }
      }  
      cmd = endCMD;
      break;
    }    
  case status:
    {
      switch(Status)
      {
      case idle:
        {
          usart.WriteString("idle");
          break;
        }
      case measure:
        {
          usart.WriteString("measure");
          break;
        }
      case error:
        {
          usart.WriteString("error");
          break;
        }        
      default:
        {
          break;
        }
        
      }
      cmd = endCMD;
      break;
    }
  default:
    {
      
      break;
    }
    
  }

}

double HID::calcRMS(){
  // при наличии информиции о входных цепях необходимо добавить пересчет в напряжение
  double rms = 0.0;
  for(int i = 0; i < adc.rawLength; ++i){
    rms += (double) adc.buffer[i] * adc.buffer[i];
  } 
  return sqrt(rms/adc.rawLength);
}

double HID::calcAVG(){
  double avg = 0.0;
  for(int i = 0; i < adc.rawLength; ++i){
    avg += (double) adc.buffer[i];
  }
  return avg/adc.rawLength;
}

HID::HID(USART &_usart, cADC &_adc): usart(_usart), adc(_adc){
  
}
HID::~HID(){
  
}

int list::operator +=(string s){
  if(indexEnd >= list_size){
    return 0;
  }else{
    arrList[indexEnd] = s;
    indexEnd++; 
    return 1;
  }
}

string& list::extractFromStart(){
  if(indexStart = indexEnd){
    return arrList[indexEnd];
  }else{
    return arrList[indexStart++];
  }
}

void list::Clear(){
  for(int i = 0; i < list_size; ++i)
  {
    arrList[i].clear();
  }
  indexEnd = 0;
  indexStart = 0;
}

string& list::operator [](uint32_t i){
  if(i <= list_size){
    return arrList[i];
  }else{
    return NULLstr;
  }
}

list::list(){
  
}

list::~list(){
  
}
