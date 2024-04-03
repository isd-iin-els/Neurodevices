#ifdef ESP32DEV//Não mexe
#ifndef Stimperturb_h //Mudar esse nome para o nome_do_arquivo_h
#define Stimperturb_h //Mudar esse nome para o nome_do_arquivo_h

#include <sstream>//Não mexe
#include <Arduino.h>//Não mexe
#include <AsyncTCP.h>//Não mexe
#include "WiFi.h"//Não mexe
#include <stdio.h>//Não mexe
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"//Não mexe
#include <wifiMQTT.h>//Não mexe
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"//Não mexe se for usar o fes

#define SENDINSOLE_PARAMETERS "{\"op\":38,\"timeSimulation\":\"float\",\"freq\":\"float\",\"Trigger\":\"string\",\"Threshold\":\"string\",\"Recoverytime\":\"string\",\"Stimtopics\":\"string\",\"Stimtime\":\"float\",\"Stimfreq\":\"float\",\"Stimpulsew\":\"float\",\"Stimintensidade\":\"float\",\"Tempenvio\":\"float\"}"//Mudar para o nome da função e os parametros que ela recebe

namespace protpertubation{

    esp_timer_create_args_t DataLoop_periodic_timer_args;//Não mexe
    esp_timer_handle_t DataLoop_periodic_timer = nullptr;//Não mexe
    volatile uint64_t DataLoop_counter = 0;//Não mexe
    volatile bool DataLoop_flag = false, state = false;//Não mexe

    LinAlg::Matrix<double> Trigger;
    LinAlg::Matrix<double> Threshold;
    LinAlg::Matrix<double> Recoverytime;
    String Stimtopics;
    double Stimfreq;
    double Stimpulsew;
    double Stimintensidade;
    double Stimtime;
    double Tempenvio;
    String datasave;
    double freq;

    static void loop(void *param){
      std::stringstream ss;

      ss << DataLoop_counter << " , " << analogRead(36)   
                            << " , " << analogRead(39)
                            << " , " << analogRead(32)
                            << " , " << analogRead(33)
                            << " , " << analogRead(34)
                            << " , " << analogRead(35)
                            << "\r\n";
      datasave += ss.str().c_str();

      if (DataLoop_counter%uint16_t(Tempenvio*freq)==0){
        mqttClient.publish(devstream.str().c_str(), 0, false, datasave.c_str());
        datasave="";
      }

      DataLoop_counter++;
      if(DataLoop_counter>=(int)param && DataLoop_flag){
        ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
        ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
        DataLoop_periodic_timer = nullptr;
        DataLoop_flag = false;
        Serial.println("Finalizou");
      }
    }

    String json2String(const char* json){
      return String(json);
    }

    String StimPerturb(const StaticJsonDocument<sizejson> &doc)  {
        freq      = (uint16_t)doc["freq"]; 
        datasave  = "";
        Stimtime  = (double)doc["Stimtime"];
        Trigger   = LinAlg::Matrix<double>(json2String(doc["Trigger"]).c_str());
        Threshold = LinAlg::Matrix<double>(json2String(doc["Threshold"]).c_str());

        Stimfreq   = (double)doc["Stimfreq"]; 
        Tempenvio  = (uint16_t)doc["Tempenvio"];
        Stimtopics = json2String(doc["Stimtopics"]);
        Stimpulsew = (double)doc["Stimpulsew"];

        Recoverytime    = LinAlg::Matrix<double>(json2String(doc["Recoverytime"]).c_str());
        Stimintensidade = (double)doc["Stimintensidade"];
        
        uint16_t timeSimulation = (uint16_t)doc["timeSimulation"]; 

        analogReadResolution(12);
        analogSetPinAttenuation(36, ADC_11db);        adcAttachPin(36);
        analogSetPinAttenuation(39, ADC_11db);        adcAttachPin(39);
        analogSetPinAttenuation(32, ADC_11db);        adcAttachPin(32);
        analogSetPinAttenuation(33, ADC_11db);        adcAttachPin(33);
        analogSetPinAttenuation(34, ADC_11db);        adcAttachPin(34);
        analogSetPinAttenuation(35, ADC_11db);        adcAttachPin(35);

        DataLoop_counter= 0;
        DataLoop_flag = true;
        DataLoop_periodic_timer_args.callback = &loop;
        DataLoop_periodic_timer_args.name = "Stimperturb";//Se quiser pode mudar a string e renomear a função
        DataLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
        ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, 1000000.0/freq));

        return "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
    }
}
#endif
#endif