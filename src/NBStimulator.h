#ifndef NBSTIMULATOR_h
#define NBSTIMULATOR_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include <sstream>
#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#ifdef WiFistaTCP_h
  #include "wifistaTCP.h"
#endif
#ifdef WiFistaMQTT_h
  #include "wifistaMQTT.h"
#endif


volatile uint64_t BN_Counter; volatile bool BN_flag = false, ADCSTREAM_flag = false;
esp_timer_create_args_t BN_periodic_timer_args;
esp_timer_handle_t BN_periodic_timer = nullptr; uint8_t devicePin;

void IRAM_ATTR BNUpdate(void *param){
    BN_Counter++;
    std::stringstream ss;
    ss << BN_Counter << " , " << analogReadMilliVolts(devicePin)  << "\r\n";
    // Serial.println(ss.str().c_str());
    // client->write(ss.str().c_str());
  #ifdef WiFistaTCP_h
    client->write(ss.str().c_str());
  #endif
  #ifdef WiFistaMQTT_h
    mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
  #endif
  // Serial.println(BN_Counter);
  if(BN_Counter==(uint64_t)param || !ADCSTREAM_flag)
  {
    // client->write("stop\r\n");
    
    // Serial.println((int)param);
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(BN_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(BN_periodic_timer)); //Timer delete
    BN_periodic_timer = nullptr;
    ADCSTREAM_flag = false;
  }
}

String adcStream(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  
  if (operation == ADCSTREAM_MSG && !ADCSTREAM_flag){
    devicePin = (uint8_t) doc["devicePin"];
    uint16_t freq = doc["frequence"];
    int64_t timeSimulation = doc["timeout"];
    std::cout << devicePin<<"\n"<< freq<<"\n"<< timeSimulation<<"\n";
    
    analogReadResolution(12);
    analogSetPinAttenuation(devicePin, ADC_11db);
    adcAttachPin(devicePin);
    BN_Counter = 0;
    ADCSTREAM_flag = true;
    BN_periodic_timer_args.callback = &BNUpdate;
    BN_periodic_timer_args.name = "BNUpdate";
    BN_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&BN_periodic_timer_args, &BN_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(BN_periodic_timer, 1000000.0/freq));
    
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

// String neurogenic_bladder_stop(void* data, size_t len) {
//   char* d = reinterpret_cast<char*>(data); String msg,answer;
//   for (size_t i = 0; i < len; ++i) msg += d[i];
//   uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
//   msg = msg.substring(index+1,msg.length());
//   LinAlg::Matrix<double> code = msg.c_str();
//   std::cout << code << std::endl; 

//   if (op.toInt() == 12){
//     stopFlag = true;

//     if(!noAnswer)
//       answer += "Loop para estimulacao de neurogenic bladder\r\n";
//   }
//   else
//     answer += "";
//   return answer;
// }

// String neurogenic_bladder_init(void* data, size_t len) {
//   char* d = reinterpret_cast<char*>(data); String msg,answer;
//   for (size_t i = 0; i < len; ++i) msg += d[i];
//   uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
//   msg = msg.substring(index+1,msg.length());
//   LinAlg::Matrix<double> code = msg.c_str();
//   std::cout << code << std::endl; 

//   if (op.toInt() == 11 && !BN_flag){
//     Serial.print("Operation 11, received data: "); Serial.println(msg);
//     BN_flag = true;
//     stopFlag = false;

//     BN_periodic_timer_args.callback = &BNUpdate;
//     BN_periodic_timer_args.name = "BNUpdate";
//     BN_periodic_timer_args.arg = (void*)(int (code(0,0)*code(0,1)));
//     ESP_ERROR_CHECK(esp_timer_create(&BN_periodic_timer_args, &BN_periodic_timer));
//     ESP_ERROR_CHECK(esp_timer_start_periodic(BN_periodic_timer, 1000000.0/code(0,1)));
//     BN_Counter = 0;

//     if(!noAnswer)
//       answer += "Loop para estimulacao de neurogenic bladder\r\n";
//   }
//   else
//     answer += "";
//   return answer;
// }

#endif