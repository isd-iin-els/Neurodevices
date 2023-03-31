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


volatile uint64_t bpm_Counter; volatile bool bpm_flag = false, bpmSTREAM_flag = false;
esp_timer_create_args_t bpm_periodic_timer_args;
esp_timer_handle_t bpm_periodic_timer = nullptr;
double bpm_filt = 0,bpm_threshold = 50;
double bpm_time, bpm_atime, bpm_afilt = 0,bpm=0;
uint8_t bpm_devicePin;


float EMA_a_low = 1.0;    //initialization of EMA alpha
float EMA_a_high = 10.;
 
int EMA_S_low = 0;        //initialization of EMA S
int EMA_S_high = 0;
 
int highpass = 0;
int bandpass = 0;
int lastValue = 0;

void IRAM_ATTR bpmUpdate(void *param){
    bpm_Counter++;
    
    int sensorValue = analogRead(bpm_devicePin);
    bpm_filt += 0.1*(sensorValue-bpm_filt);
    int dbpm = bpm_filt-lastValue;
    lastValue = bpm_filt;

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
    // }
    Serial.println(dbpm);
  // if(bpm/400 < 30)
  //   digitalWrite(2,0);
  // else if(bpm/400 > 50)
  //   digitalWrite(2,1);
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
    bpm_devicePin = uint8_t(doc["bpm_devicePin"]);
    uint16_t freq = doc["frequence"];
    int64_t timeSimulation = doc["timeout"];
    std::cout << bpm_devicePin<<"\n"<< freq<<"\n"<< timeSimulation<<"\n";
    
    analogReadResolution(12);
    analogSetPinAttenuation(bpm_devicePin, ADC_11db);
    adcAttachPin(bpm_devicePin);
    bpm_atime = millis();

    pinMode(2,OUTPUT);

    bpm_Counter = 0;
    bpmSTREAM_flag = true;
    bpm_periodic_timer_args.callback = &bpmUpdate;
    bpm_periodic_timer_args.name = "bpmUpdate";
    bpm_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&bpm_periodic_timer_args, &bpm_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(bpm_periodic_timer, 1000000.0/freq));
    
    
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