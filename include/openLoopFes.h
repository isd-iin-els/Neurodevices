#ifndef openLoopfes_h
#define openLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "sendIMUData.h"

volatile bool openLoop_flag = false, sensor_flag = false;

int CS_PIN  = 23;
int UD_PIN  = 21;
int INC_PIN = 22;

int OPTO_PIN1 = 18;
int OPTO_PIN2 = 19;

void setResistance(int percent);

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

    pinMode(CS_PIN, OUTPUT);
    pinMode(UD_PIN, OUTPUT);
    pinMode(INC_PIN, OUTPUT);
    pinMode(OPTO_PIN1, OUTPUT);
    pinMode(OPTO_PIN2, OUTPUT);

    digitalWrite(CS_PIN, HIGH);
    digitalWrite(INC_PIN, HIGH);

    // valor inicial de intensidade (pode ser ajustado)
    setResistance(10);

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
  
  // if (operation == OPENLOOPTONFREQUPDATE_MSG){
    openLoopFesStop();
    openLoopFesInit(doc["t"],doc["p"]);
    return String("Ton and Frequency sucessfully updated!");

  // }
  // else
    // answer += "";
}

String openLoopFesConfig(const StaticJsonDocument<sizejson> &doc)  {
  openLoopFesInit(doc["t"],doc["p"]);
  return String("System Successfully Configured");
}

String openLoopFesUpdate(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {

  Serial.println("DEU CERTO");

  String answer;

    const char *msg = doc["m"];
    LinAlg::Matrix<double> code = msg;

  setResistance(code(0,0)); // atualizar o potenciômetro digital

    for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
      dispositivo.fes[i].setPowerLevel(code(0,i));  

    if (doc.containsKey("f")) //f is for fade
      for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
        dispositivo.fes[i].setFadeTime(doc["f"]); 


         // --------- NOVO: atualizar o potenciômetro digital ---------
    if(doc.containsKey("r"))
        setResistance(doc["r"]);

    if(dispositivo.stopLoopFlag)
        openLoopFesInit(doc["t"],doc["p"]);

  return String("1");
}


String stopOpenLoopFes(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  // if (operation == STOPOPENLOOPFES_MSG){
    // Serial.print("Operation 8, received data: "); Serial.println(msg);
    openLoopFesStop();
    return String("1");
  // }
  // else
  //   answer += "";
}
void setResistance(int percent) {

    percent = constrain(percent, 0, 100);

    // Zerar resistência
    digitalWrite(UD_PIN, LOW);
    for(int i = 0; i < 100; i++) {
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(CS_PIN, HIGH);
        delayMicroseconds(5);
    }

    // Posicionar na resistência desejada
    digitalWrite(UD_PIN, HIGH);
    for(int i = 0; i < percent; i++) {
        digitalWrite(CS_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(INC_PIN, HIGH);
        delayMicroseconds(5);
        digitalWrite(CS_PIN, HIGH);
        delayMicroseconds(5);
    }
}

#endif