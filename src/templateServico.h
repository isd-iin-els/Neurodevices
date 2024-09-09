#ifdef ESP32DEV//Não mexe
#ifndef tsPCSLoopfes_h //Mudar esse nome para o nome_do_arquivo_h
#define tsPCSLoopfes_h //Mudar esse nome para o nome_do_arquivo_h

#include <sstream>//Não mexe
#include <Arduino.h>//Não mexe
#include <AsyncTCP.h>//Não mexe
#include "WiFi.h"//Não mexe
#include <stdio.h>//Não mexe
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"//Não mexe
#include <wifiMQTT.h>//Não mexe
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"//Não mexe se for usar o fes

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//                 Criar suas variaveis aqui
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

namespace DarUmNome{//muda o nome

esp_timer_create_args_t DataLoop_periodic_timer_args;//Não mexe
esp_timer_handle_t DataLoop_periodic_timer = nullptr;//Não mexe
volatile uint64_t DataLoop_counter = 0;//Não mexe
volatile bool DataLoop_flag = false, state = false;//Não mexe
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//                 Criar suas variaveis aqui
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


static void loop(void *param){
  std::stringstream ss;//Não mexe

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//    Implementar o que você quer processar em tempo real

// exemplo de preenchimento da variável de envio: ss << "," << 1234 << '\n';

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------


  mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());//Não mexe se o padrão é mandar dados de stream
  DataLoop_counter++;//Não mexe
  if(DataLoop_counter>=(int)param)
  {
    ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
    DataLoop_periodic_timer = nullptr;
    DataLoop_flag = false;
    Serial.println("Finalizou");
  }
}

String setup(const StaticJsonDocument<sizejson> &doc)  {
  String answer;

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//                 Criar suas variaveis aqui
// Exemplo de como extrair as variáveis do json. Esse padrão é criado em servicesDef.h 
// se lá você definiu "{\"op\":37,\"timeSimulation\":\"float\",\"freq\":\"float\"}"
// aqui você pega assim:
   float timeSimulation = (uint16_t)doc["timeSimulation"]; 
   float freq = (uint16_t)doc["freq"]; 
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
   
    

    DataLoop_counter = 0;//Não mexe
    DataLoop_flag = true;
    DataLoop_periodic_timer_args.callback = &loop;
    DataLoop_periodic_timer_args.name = "setup";//Se quiser pode mudar a string e renomear a função
    DataLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, 1000000.0/freq));
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  // }
  // else
  //   answer += "";
  return answer;
}
}
#endif
#endif