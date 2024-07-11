#ifndef openLoopfes_h
#define openLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "sendIMUData.h"

volatile bool openLoop_flag = false, sensor_flag = false;

// -----------------------ESP32devkit-------------------------------------------
#ifdef ESP32DEV
uint8_t modPin[8]    = {27,19,12,18,23,14,26,25},//ca andré
        levelPin[4]  = {13,4,2,33};
// uint8_t modPin[8]    = {4,12,0,0,0,0,0,0},//ca andré
//         levelPin[4]  = {2,0,0,0};
Devices::fes4channels dispositivo(levelPin, modPin, 4, 18000,200,20000,true);

// -----------------------ESP32C3-------------------------------------------
#elif ESP32C3DEV
uint8_t modPin[2]    = {3,4},//ca andré
        levelPin[1]  = {2};
Devices::fes4channels dispositivo(levelPin, modPin, 1, 18000,200,20000,true);

#elif ESP32S2DEV
uint8_t modPin[2]    = {3,4},//ca andré
        levelPin[1]  = {2};
Devices::fes4channels dispositivo(levelPin, modPin, 1, 18000,200,20000,true);

#endif

void openLoopFesInit(uint32_t ton, uint32_t period){
    openLoop_flag = true;
    if(dispositivo.stopLoopFlag){
      dispositivo.stopLoopFlag = false;
      dispositivo.timeOnAndPeriodUpdate(ton,period);
      dispositivo.startLoop();
    }
    
    dispositivo.stopLoopFlag = false;
    // dispositivo.startLoop();
}

void openLoopFesStop(){
    // dispositivo.stopLoop();
    if(!dispositivo.stopLoopFlag){
      for(uint8_t i = 0; i < 4; ++i)
        dispositivo.fes[i].setPowerLevel(0); 

      dispositivo.stopLoopFlag = true;
    }
    
}

String openLoopTonFreqUpdate(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/) {

  String answer;
  // if (operation == OPENLOOPTONFREQUPDATE_MSG){
    openLoopFesStop();
    openLoopFesInit(doc["t"],doc["p"]);
    answer += "Ton and Frequency sucessfully updated!";
  // }
  // else
    // answer += "";
  return answer;
}

String openLoopFesConfig(const StaticJsonDocument<sizejson> &doc)  {
  openLoopFesInit(doc["t"],doc["p"]);
  return String("System Successfully Configured");
}

String openLoopFesUpdate(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {

  String answer;

    const char *msg = doc["m"];
    LinAlg::Matrix<double> code = msg;

    for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
      dispositivo.fes[i].setPowerLevel(code(0,i));  
    if(dispositivo.stopLoopFlag){
      openLoopFesInit(doc["t"],doc["p"]);
    }
    if (doc.containsKey("f")) //f is for fade
      for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
        dispositivo.fes[i].setFadeTime(doc["f"]); 


  answer += "1";
  return answer;
}


String stopOpenLoopFes(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;
  // if (operation == STOPOPENLOOPFES_MSG){
    // Serial.print("Operation 8, received data: "); Serial.println(msg);
    openLoopFesStop();
    answer += "1";
  // }
  // else
  //   answer += "";
  return answer;
}


#endif