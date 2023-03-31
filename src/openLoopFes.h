#ifndef openLoopfes_h
#define openLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "sendIMUData.h"

volatile bool openLoop_flag = false, sensor_flag = false;

// uint8_t modPin[4]    = {27,4,12,5},
//         levelPin[4]  = {13,19,2,18};
// uint8_t modPin[4]    = {27,19,12,18},//cc
//         levelPin[4]  = {13,4,2,5};]
// uint8_t modPin[8]    = {27,19,12,18,23,14,26,25},//ca
//         levelPin[4]  = {13,4,2,33};
uint8_t modPin[8]    = {12,14,2,15,21,23,26,32},//ca
        levelPin[4]  = {13,4,22,18};

Devices::fes4channels dispositivo(levelPin, modPin, 4, 18000,350,100000,true);

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

String openLoopTonFreqUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {

  String answer;
  if (operation == OPENLOOPTONFREQUPDATE_MSG){
    openLoopFesStop();
    openLoopFesInit(doc["t"],doc["p"]);
    answer += "Ton and Frequency sucessfully updated!";
  }
  else
    answer += "";
  return answer;
}

String openLoopFesUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {

  String answer;
  if (operation == OPENLOOPFESUPDATE_MSG){
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

    // if(!sensor_flag)
      // if(sensorType == 1)
        // mpu6050Flag = mpuInit();
      // else if(sensorType == 2)
    //gy80Flag = sensors.init();

    //answer += getIMUData();
    //std::cout << answer.c_str() << std::endl; 
    answer += "1";
  }
  else
    answer += "1";
  return answer;
}


String stopOpenLoopFes(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == STOPOPENLOOPFES_MSG){
    // Serial.print("Operation 8, received data: "); Serial.println(msg);
    openLoopFesStop();
    answer += "1";
  }
  else
    answer += "";
  return answer;
}


#endif