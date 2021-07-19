#ifndef openLoopfes_h
#define openLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "wifistaTCP.h"

volatile bool openLoop_flag = false;

uint8_t modPin[4]    = {27,4,12,5},
        levelPin[4]  = {13,19,2,18};

Devices::fes4channels dispositivo(levelPin, modPin, 4, 18000);

void openLoopFesInit(uint16_t ton, uint16_t period){
    openLoop_flag = true;
    if(dispositivo.stopLoopFlag){
      dispositivo.stopLoopFlag = false;
      dispositivo.timeOnAndPeriodUpdate(ton,period);
      dispositivo.startLoop();
    }
    
    // dispositivo.stopLoopFlag = false;
    // dispositivo.startLoop();
}

void openLoopFesStop(){
    // dispositivo.stopLoop();
    for(uint8_t i = 0; i < 4; ++i)
      dispositivo.fes[i].setPowerLevel(0); 
  
    dispositivo.stopLoopFlag = true;
    
}

String openLoopFesUpdate(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 2){
    Serial.print("Operation 2, received data: "); Serial.println(msg);
    // if(dispositivo.stopLoopFlag){
    //   dispositivo.stopLoopFlag = false;
    //   dispositivo.timeOnAndPeriodUpdate(code(0,code.getNumberOfColumns()-2),code(0,code.getNumberOfColumns()-1));
    //   dispositivo.startLoop();
    // }
    
    for(uint8_t i = 0; i < code.getNumberOfColumns()-2; ++i)
      dispositivo.fes[i].setPowerLevel(code(0,i));  
    openLoopFesInit(code(0,code.getNumberOfColumns()-2),code(0,code.getNumberOfColumns()-1));

    
    if(!noAnswer)
      answer += "Valores de estimulacao alterados\r\n";
  }
  else
    answer += "";
  return answer;
}

String stopOpenLoopFes(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 8){
    Serial.print("Operation 8, received data: "); Serial.println(msg);
    openLoopFesStop();
    if(!noAnswer)
      answer += "Valores de estimulacao alterados\r\n";
  }
  else
    answer += "";
  return answer;
}


#endif