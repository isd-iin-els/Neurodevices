#ifndef SENDINSOLEDATA_h
#define SENDINSOLEDATA_h

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


volatile uint64_t insole_Counter; volatile bool insole_flag = false, insoleSTREAM_flag = false;
esp_timer_create_args_t insole_periodic_timer_args;
esp_timer_handle_t insole_periodic_timer = nullptr;

void IRAM_ATTR insoleUpdate(void *param){
    insole_Counter++;
    std::stringstream ss;
    ss << insole_Counter << " , " << analogRead(36)  
                        //  << " , " << analogRead(37) 
                        //  << " , " << analogRead(38) 
                         << " , " << analogRead(39)
                         << " , " << analogRead(32)
                         << " , " << analogRead(33)
                         << " , " << analogRead(34)
                         << " , " << analogRead(35)
                        //  << " , " << analogRead(0)
                        //  << " , " << analogRead(ADC2_CHANNEL_1)
                        //  << " , " << analogRead(ADC2_CHANNEL_2)
                        //  << " , " << analogRead(ADC2_CHANNEL_3)
                        //  << " , " << analogRead(ADC2_CHANNEL_4)
                        //  << " , " << analogRead(ADC2_CHANNEL_5) 
                        //  << " , " << analogRead(ADC2_CHANNEL_6)
                        //  << " , " << analogRead(ADC2_CHANNEL_7)
                         << "\r\n";
    Serial.println(ss.str().c_str());
    // client->write(ss.str().c_str());
  #ifdef WiFistaTCP_h
    client->write(ss.str().c_str());
  #endif
  #ifdef WiFistaMQTT_h
    mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
  #endif
  // Serial.println(insole_Counter);
  if(insole_Counter==(uint64_t)param || !insoleSTREAM_flag)
  {
    // client->write("stop\r\n");
    
    // Serial.println((int)param);
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(insole_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(insole_periodic_timer)); //Timer delete
    insole_periodic_timer = nullptr;
    insoleSTREAM_flag = false;
  }
}

String stopInsoleStream(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == STOPINSOLESTREAM_MSG && insoleSTREAM_flag){
    if(insole_periodic_timer != nullptr){
      ESP_ERROR_CHECK(esp_timer_stop(insole_periodic_timer)); //Timer pause
      ESP_ERROR_CHECK(esp_timer_delete(insole_periodic_timer)); //Timer delete
    }
      insole_periodic_timer = nullptr;
      insoleSTREAM_flag = false;
      // IMUDataLoop_counter = -1;
      answer = "aCQUISITION Stopped";
      // std::cout << IMUDataLoop_counter << " " << answer.c_str() << std::endl;
  } else
    answer += "";
  return answer;
}

String insoleStream(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  
  if (operation == INSOLESTREAM_MSG && !insoleSTREAM_flag){
    uint16_t freq = doc["frequence"];
    int64_t timeSimulation = doc["timeout"];
    std::cout << freq<<"\n"<< timeSimulation<<"\n";
    
    analogReadResolution(12);

    analogSetPinAttenuation(36, ADC_11db);
    adcAttachPin(36);
    // analogSetPinAttenuation(37, ADC_11db);
    // adcAttachPin(37);
    // analogSetPinAttenuation(38, ADC_11db);
    //adcAttachPin(38);
    analogSetPinAttenuation(39, ADC_11db);
    adcAttachPin(39);
    analogSetPinAttenuation(32, ADC_11db);
    adcAttachPin(32);
    analogSetPinAttenuation(33, ADC_11db);
    adcAttachPin(33);
    analogSetPinAttenuation(34, ADC_11db);
    adcAttachPin(34);
    analogSetPinAttenuation(35, ADC_11db);
    adcAttachPin(35);
    // analogSetPinAttenuation(0, ADC_11db);
    // adcAttachPin(0);
    // analogSetPinAttenuation(ADC2_CHANNEL_1, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_1);
    // analogSetPinAttenuation(ADC2_CHANNEL_2, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_2);
    // analogSetPinAttenuation(ADC2_CHANNEL_3, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_3);
    // analogSetPinAttenuation(ADC2_CHANNEL_4, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_4);
    // analogSetPinAttenuation(ADC2_CHANNEL_5, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_5);
    // analogSetPinAttenuation(ADC2_CHANNEL_6, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_6);
    // analogSetPinAttenuation(ADC2_CHANNEL_7, ADC_11db);
    // adcAttachPin(ADC2_CHANNEL_7);

    insole_Counter = 0;
    insoleSTREAM_flag = true;
    insole_periodic_timer_args.callback = &insoleUpdate;
    insole_periodic_timer_args.name = "insoleUpdate";
    insole_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&insole_periodic_timer_args, &insole_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(insole_periodic_timer, 1000000.0/freq));
    
    
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

//   if (op.toInt() == 11 && !insole_flag){
//     Serial.print("Operation 11, received data: "); Serial.println(msg);
//     insole_flag = true;
//     stopFlag = false;

//     insole_periodic_timer_args.callback = &insoleUpdate;
//     insole_periodic_timer_args.name = "insoleUpdate";
//     insole_periodic_timer_args.arg = (void*)(int (code(0,0)*code(0,1)));
//     ESP_ERROR_CHECK(esp_timer_create(&insole_periodic_timer_args, &insole_periodic_timer));
//     ESP_ERROR_CHECK(esp_timer_start_periodic(insole_periodic_timer, 1000000.0/code(0,1)));
//     insole_Counter = 0;

//     if(!noAnswer)
//       answer += "Loop para estimulacao de neurogenic bladder\r\n";
//   }
//   else
//     answer += "";
//   return answer;
// }

#endif