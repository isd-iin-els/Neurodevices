#ifndef openLoopfes_h
#define openLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "wifistaTCP.h"

volatile bool openLoop_flag = false;

uint8_t modPin[4]    = {27,4,12,5},
        levelPin[4]  = {13,19,2,18};

Devices::fes4channels dispositivo(levelPin, modPin, 4, 18000);

String openLoopFesUpdate(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 2){
    Serial.print("Oeration 2, received data: "); Serial.println(msg);
    dispositivo.fes[0].setPowerLevel(code(0,1)); 
    dispositivo.fes[1].setPowerLevel(code(0,2)); 
    dispositivo.fes[2].setPowerLevel(code(0,3)); 
    dispositivo.fes[3].setPowerLevel(code(0,4));  
    openLoop_flag = true;
    answer += "Valores de estimulacao alterados\r\n";
  }
  else
    answer += "";
  return answer;
}

void openLoopFesInit(){
    dispositivo.stopLoop();
    dispositivo.startLoop();
}

void openLoopFesStop(){
    dispositivo.stopLoop();
    openLoop_flag = false;
}
#endif