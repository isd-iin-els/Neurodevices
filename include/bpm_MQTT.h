#ifndef NBSTIMULATOR_h
#define NBSTIMULATOR_h
//Alterar limiar de corte via rede
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


volatile uint64_t bpm_Counter; volatile bool bpm_flag = false, bpmSTREAM_flag = false;
esp_timer_create_args_t bpm_periodic_timer_args;
esp_timer_handle_t bpm_periodic_timer = nullptr;
double bpm_filt = 0,bpm_threshold = 30;
double bpm_time, bpm_atime, bpm_afilt = 0,bpm=0,last_dbpm = 0,bpm_scale=4.4;
uint8_t bpm_devicePin=36;
uint16_t bpm_freq = 0,batimentos = 0;
bool flag_bpmTime = false;
int lastValue = 0;
int dbpm = 0;

void IRAM_ATTR bpmUpdate(void *param){
    bpm_Counter++;
    
    int sensorValue = analogRead(bpm_devicePin);
    bpm_filt += 0.01*(sensorValue-bpm_filt);
    last_dbpm = dbpm;
    dbpm = bpm_filt-lastValue;
    lastValue = bpm_filt;
    
    if(dbpm >= 0 && last_dbpm<0)
      batimentos++;
    if(int(bpm_Counter)%(int(bpm_freq/2)) == 0){
      double bpmTemp = batimentos*2*bpm_scale;
      bpm_afilt += 0.03*(bpmTemp-bpm_afilt);
      Serial.println(bpm_afilt);
      batimentos = 0;
      std::stringstream ss;
      ss << bpm_Counter << " , " << bpm_afilt  << "\r\n";
      //Serial.print("BPM = ");Serial.println(bpm);
      #ifdef WiFistaTCP_h
      client->write(ss.str().c_str());
      #endif
      #ifdef WiFistaMQTT_h
      mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
      #endif
    }
    
    
    // EMA_S_low = (EMA_a_low*sensorValue) + ((1-EMA_a_low)*EMA_S_low);  //run the EMA
    // EMA_S_high = (EMA_a_high*sensorValue) + ((1-EMA_a_high)*EMA_S_high);

    // highpass = sensorValue - EMA_S_low;     //find the high-pass as before (for comparison)
    // bpm_filt = EMA_S_high - EMA_S_low;      //find the band-pass
    // //Serial.println(bpm_filt-bpm_afilt);
    // // if(bpm_Counter < 200)
    // // {
    // //   bpm_afilt += bpm_filt/200;
    // // }
    // //else 
    // if(dbpm > bpm_threshold){
    //   //bpm_afilt = bpm_filt;
    //   bpm_time = millis();
    //   double bpmTemp = (bpm_time-bpm_atime)*60;
    //   bpm += 0.005*(bpmTemp-bpm);
    //   bpm_atime = bpm_time;
      
    //   //std::cout << ss.str().c_str();
    //   // if(bpm_Counter%5==0){
    //     std::stringstream ss;
    //     ss << bpm_Counter << " , " << bpm  << "\r\n";
    //     //Serial.print("BPM = ");Serial.println(bpm);
    //     #ifdef WiFistaTCP_h
    //       client->write(ss.str().c_str());
    //     #endif
    //     #ifdef WiFistaMQTT_h
    //       mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
    //     #endif
    //   // }
    // }uint16_t 
    // Serial.println(dbpm);
  if(bpm_afilt < bpm_threshold)
     digitalWrite(2,0);
  else if(bpm_afilt > bpm_threshold+20)
     digitalWrite(2,1);
  // Serial.println(bpm_Counter);
  if(bpm_Counter==(uint64_t)param || !bpmSTREAM_flag)
  {
    // client->write("stop\r\n");
    
    // Serial.println((int)param);
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(bpm_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(bpm_periodic_timer)); //Timer delete
    bpm_periodic_timer = nullptr;
    bpmSTREAM_flag = false;
  }
}

String bpmStream(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  
  if (operation == BPMSTREAM_MSG && !bpmSTREAM_flag){
    bpm_threshold = double(doc["bpm_threshold"]);
    bpm_scale = uint8_t(doc["bpm_scale"]);
    bpm_freq = doc["frequence"];
    int64_t timeSimulation = doc["timeout"];
    std::cout << bpm_devicePin<<"\n"<< bpm_freq<<"\n"<< timeSimulation<<"\n";
    
    analogReadResolution(12);
    analogSetPinAttenuation(bpm_devicePin, ADC_11db);
    adcAttachPin(bpm_devicePin);
    bpm_atime = millis();

    pinMode(2,OUTPUT);

    bpm_Counter = 0;
    bpmSTREAM_flag = true;
    bpm_periodic_timer_args.callback = &bpmUpdate;
    bpm_periodic_timer_args.name = "bpmUpdate";
    bpm_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*bpm_freq));
    ESP_ERROR_CHECK(esp_timer_create(&bpm_periodic_timer_args, &bpm_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(bpm_periodic_timer, 1000000.0/bpm_freq));
    
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

String stopBpmStream(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == STOPBPMSTREAM_MSG){
    // Serial.print("Operation 8, received data: "); Serial.println(msg);
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(bpm_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(bpm_periodic_timer)); //Timer delete
    bpm_periodic_timer = nullptr;
    bpmSTREAM_flag = false;
  }
  else
    answer += "";
  return answer;
}

#endif