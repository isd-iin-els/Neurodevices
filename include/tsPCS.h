#ifndef tsPCSLoopfes_h
#define tsPCSLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "openLoopFes.h"
//#include "sendIMUData.h"

namespace tsPCS{

volatile bool Loop_flag = false;


uint8_t modPin[8]    = {27,19,12,18,23,14,26,25},//ca
        levelPin[4]  = {13,4,2,33};

esp_timer_create_args_t DataLoop_periodic_timer_args;
esp_timer_handle_t DataLoop_periodic_timer = nullptr;
hw_timer_t *timer = NULL;
volatile uint64_t DataLoop_counter = 0;
volatile bool     DataLoop_flag = false, state = false;
volatile uint16_t pulse_time = 50; 
volatile uint16_t min_pulse = 1;
volatile uint16_t min_cicle = 2;
volatile uint16_t larger_cicle_function = 4;
volatile uint16_t counter = 0, Tspcsfrequency = 30, TspcsPeriod = 1/30*1000000, TspcsPulsewidth = 200;
volatile bool high_counter = false;

String openLoopTspcsUpdate(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {

    dispositivo.stopLoopFlag = false;
    String answer = openLoopFesUpdate(doc);
    dispositivo.stopLoopFlag = true;

  return answer;
}

static void DataTspcsLoop(void *param){
// void IRAM_ATTR onTimer() {
    if(counter < TspcsPulsewidth){
      if(!high_counter){
        for (int i = 0; i < 4; i++){
          dispositivo.fes[i].setOutputDirectPin();
          dispositivo.fes[i].resetOutputReversePin();
        }
      }else if(high_counter){
        for (int i = 0; i < 4; i++){
          dispositivo.fes[i].resetOutputDirectPin();
          dispositivo.fes[i].setOutputReversePin();
        }
      }
      high_counter = !high_counter;
    } else if(counter >= TspcsPulsewidth && counter <= TspcsPeriod){
        for (int i = 0; i < 4; i++){
          dispositivo.fes[i].resetOutputDirectPin();
          dispositivo.fes[i].resetOutputReversePin();
        }
    } else if(counter > TspcsPeriod) {
      counter = 0;
    }
     counter++;
}

String sendtsPCSInit(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;
   std::cout << "Tudo Inicializado\n";
  // if (operation == TSPCSSENDINIT_MSG && !DataLoop_flag){

    counter     = 0;
    pulse_time  = (uint16_t)doc["pulse_time"];
    Tspcsfrequency  = (uint16_t)doc["Tspcsfrequency"];
    TspcsPeriod     = (1.0/float(Tspcsfrequency)*float(1000000))/float(pulse_time);
    TspcsPulsewidth = (uint16_t)(((float)doc["TspcsPulsewidth"])/(float)pulse_time);
//high

    openLoopTspcsUpdate(doc);

    Serial.println("TspcsPulsewidth");
    Serial.println(TspcsPulsewidth);
    Serial.println("TspcsPeriod");
    Serial.println(TspcsPeriod);

    if(!DataLoop_flag){
      DataLoop_flag = true;
      DataLoop_periodic_timer_args.callback = &DataTspcsLoop;
      DataLoop_periodic_timer_args.name = "tspcsSendInit";
      DataLoop_periodic_timer_args.arg = (void*)((int64_t)(pulse_time));
      ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
      ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, (uint64_t)pulse_time));
    }
  
    

    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  // }
  // else
  //   answer += "";
  return answer;
}

}

#endif