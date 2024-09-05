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
    double tempoBaseline = 10;
    u8_t stimState = 0;
    unsigned long startTime;
    unsigned long  recovery_time;
    bool stimFlag = false;

    //calcâneo 1 (4), calcâneo 2 (3), Meta2 (2), Médiopé (1), Meta1(6), hálux (5)
    std::stringstream readData()
    {
      std::stringstream ss;

      ss << DataLoop_counter << " , " << analogRead(33) //Calcaneo 1
                            << " , " << analogRead(32) // Calcaneo 2
                            << " , " << analogRead(39) // Meta 1
                            << " , " << analogRead(36) //Mediopé
                            << " , " << analogRead(35) // Meta 2
                            << " , " << analogRead(34) //Halux
                            << "\r\n";
      datasave += ss.str().c_str();
      return ss;
    }
    
    void publishData(){
      if (DataLoop_counter%uint16_t(Tempenvio*freq)==0){
        mqttClient.publish(devstream.str().c_str(), 0, false, datasave.c_str());
        datasave="";
      }
    }

    void baseline(double freq, uint64_t DataLoop_counter)
    {
        readData();
        publishData();
    }

    void heelStrike(LinAlg::Matrix<double> data, double Stimtime,String Stimtopics)
    {
      // std::stringstream ss; ss << data(0,0) << "," << data(0,1) << "," << data(0,2) << "," << data(0,3) << "," << data(0,4) << "," << data(0,5) << "," << data(0,6) << "," << data(0,7);
      // Serial.println(ss.str().c_str());
      //calcâneo 1 (4), calcâneo 2 (3), Meta2 (2), Médiopé (1), Meta1(6), hálux (5)
      if(data(0,1) > Trigger(0,0) && data(0,2) > Trigger(0,0) && stimState == 0){
        std::stringstream stim;
        Serial.println("Entrou heelStrike");
        stim << "{\"op\":2,\"m\":\"" << Stimintensidade << ',' <<Stimintensidade << ',' << Stimintensidade << ',' << Stimintensidade << "\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";

        mqttClient.publish(Stimtopics.c_str(), 0, false, stim.str().c_str()); 
        stimState = 1;
        startTime = millis(); 
        recovery_time = random(Recoverytime(0,0),Recoverytime(0,1))*1000;
        
      } 
    }

    void midFootStrike(LinAlg::Matrix<double> data, double Stimtime,String Stimtopics)
    {
      //calcâneo 1 (4), calcâneo 2 (3), Meta2 (2), Médiopé (1), Meta1(6), hálux (5)
      if(data(0,1) < Threshold(0,0) && data(0,2) < Threshold(0,0) &&
         data(0,4) > Trigger(0,0)   && stimState == 2){
        std::stringstream stim;
      Serial.println("Entrou midFootStrike");

        //stim << "{\"op\":2,\"m\":\"" << "0,0," << Stimintensidade << ',' << Stimintensidade << "\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";
        stim << "{\"op\":2,\"m\":\"" << Stimintensidade << ',' <<Stimintensidade << ',' << Stimintensidade << ',' << Stimintensidade << "\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";
        mqttClient.publish(Stimtopics.c_str(), 0, false, stim.str().c_str()); 
        stimState = 3;
        startTime = millis(); 
        recovery_time = random(Recoverytime(0,0),Recoverytime(0,1))*1000;
        
      } 
    }

    void foreFootStrike(LinAlg::Matrix<double> data, double Stimtime,String Stimtopics)
    {
       //grupo 1 calcâneo 1, calcâneo 2 ativando e 
       //calcâneo 1 (4), calcâneo 2 (3), Meta2 (2), Médiopé (1), Meta1(6), hálux (5)
      if(data(0,4) < Threshold(0,0) && 
         data(0,3) > Trigger(0,0)   && data(0,5) < Trigger(0,0) && data(0,6) < Trigger(0,0) && 
         stimState == 4){
        std::stringstream stim;
        Serial.println("Entrou foreFootStrike");

        //stim << "{\"op\":2,\"m\":\"" << "0,0," << Stimintensidade << ',' << Stimintensidade << "\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";
        stim << "{\"op\":2,\"m\":\"" << Stimintensidade << ',' <<Stimintensidade << ',' << Stimintensidade << ',' << Stimintensidade << "\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";
        mqttClient.publish(Stimtopics.c_str(), 0, false, stim.str().c_str()); 
        stimState = 5;
        startTime = millis(); 
        recovery_time = random(Recoverytime(0,0),Recoverytime(0,1))*1000;
        
      } 
    }

    void recoverTime(double freq, uint64_t DataLoop_counter, uint8_t state, bool lastState = false, double stimTime = 0)
    {
      if(stimState == state){
        unsigned long elapsedTime = millis() - startTime; 
        if(elapsedTime > recovery_time){
          std::stringstream stim;
          Serial.println(recovery_time);
        Serial.println("Entrou recoverTime");
          stim << "{\"op\":2,\"m\":\"0,0,0,0\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";

          mqttClient.publish(Stimtopics.c_str(), 0, false, stim.str().c_str()); 
          stimFlag = false;
          stimState = state+1;
          if(lastState)
            stimState = 0;  
        }else if(elapsedTime > stimTime*1000 && !stimFlag){
          stimFlag = true;
          std::stringstream stim;
          stim << "{\"op\":2,\"m\":\"0,0,0,0\",\"t\":" << Stimpulsew << ",\"p\":" << 1000000/Stimfreq << "}\n";

          mqttClient.publish(Stimtopics.c_str(), 0, false, stim.str().c_str());
        }
      }
    }

    void experiment(double Stimtime,String Stimtopics)
    { 
      std::stringstream ss = readData();
      
      heelStrike(ss.str().c_str(), Stimtime, Stimtopics);
      recoverTime(freq,DataLoop_counter,1,false,Stimtime);

      midFootStrike(ss.str().c_str(), Stimtime,Stimtopics);
      recoverTime(freq,DataLoop_counter,3,false,Stimtime);

      foreFootStrike(ss.str().c_str(), Stimtime, Stimtopics);
      recoverTime(freq,DataLoop_counter,5,true,Stimtime);
      
      publishData();
    }

    void canIFinishExperiment(void *param)
    {
       if(DataLoop_counter>=(int)param && DataLoop_flag){
        ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
        ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
        DataLoop_periodic_timer = nullptr;
        DataLoop_flag = false;
        Serial.println("Finalizou");
      }
    }

    static void loop(void *param){// 8.3ms
       //Serial.println("Entrou Loop");
       if(DataLoop_counter <= tempoBaseline*freq){
         baseline(freq, DataLoop_counter);
        Serial.println("Entrou Baseline");
       }
      else{
        //Serial.println("Entrou experimento");
         experiment(Stimtime,Stimtopics);
       }

      DataLoop_counter++;
      canIFinishExperiment(param);
    }

    String json2String(const char* json){
      return String(json);
    }

    String StimPerturb(const StaticJsonDocument<sizejson> &doc)  {
        serializeJson(doc, Serial);
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

        stimState = 0;

        // Serial.println(doc.as<String>().c_str());
        // delay(5000);

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