// #ifdef ESP32DEV
#ifndef wheelChair_h
#define wheelChair_h

#include <sstream>
#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include <stdio.h>
#include "SistemasdeControle/embeddedTools/primitiveLibs/LinAlg/matrix.h"
#include <wifiMQTT.h>



namespace wheelChair{
esp_timer_create_args_t DataLoop_periodic_timer_args;
esp_timer_handle_t DataLoop_periodic_timer = nullptr;
volatile uint64_t DataLoop_counter = 0;
volatile bool DataLoop_flag = false, state = false;
//abaixo eu fiz

float datas[7] = {1,2,3,4,5,6,7};
//

static void DataLoop(void *param){
  std::stringstream ss;

    //Aqui você atribui as variáveis a datas
    //Aqui você atribui as variáveis a datas
    //Aqui você atribui as variáveis a datas
    //Aqui você atribui as variáveis a datas
    //Aqui você atribui as variáveis a datas

  ss << datas[0] << "," << datas[1] << "," << datas[2] << "," << datas[3] << ","
     << datas[4] << "," << datas[5] << "," << datas[6] << std::endl;
  Serial.println(ss.str().c_str());
  mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());

  if(DataLoop_counter>=(int)param)
  {
    ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
    DataLoop_periodic_timer = nullptr;
    DataLoop_flag = false;
    Serial.println("Finalizou");
  }
}

String sendWheelChairInit(const StaticJsonDocument<sizejson> &doc)  {
    String answer;
    float timeSimulation = (uint16_t)doc["timeSimulation"]; //high
    float freq = (uint16_t)doc["freq"]; //high
    //Aqui você inicializa a aquisição de dados
    //Aqui você inicializa a aquisição de dados
    //Aqui você inicializa a aquisição de dados
    //Aqui você inicializa a aquisição de dados
    //Aqui você inicializa a aquisição de dados


    DataLoop_flag = true;
    DataLoop_periodic_timer_args.callback = &DataLoop;
    DataLoop_periodic_timer_args.name = "wheelChair";
    DataLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, 1000000.0/freq));

    Serial.println("Tudo Inicializado");
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  return answer;
}
}
#endif
// #endif