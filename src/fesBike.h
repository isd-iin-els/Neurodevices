#ifndef fesBike_h
#define fesBike_h
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "openLoopFes.h"

double fesCycling, fesCyclingMax; bool fesBike_flag = false, fesCyclingBool = true;
esp_timer_create_args_t fesBike_periodic_timer_args;
esp_timer_handle_t fesBike_periodic_timer = nullptr;
volatile uint64_t fesBike_counter;

void IRAM_ATTR fesBikeLoop(void *param){
    if(fesCyclingBool){
        dispositivo.fes[0].setPowerLevel(fesCycling); 
        if(fesCycling >= fesCyclingMax){
            fesCyclingBool = false;
            fesCycling = 5;
            dispositivo.fes[0].setPowerLevel(0); 
            std::cout << "Mudou 2" << std::endl;
        }
        fesCycling += 0.25;  
    }else{
        dispositivo.fes[2].setPowerLevel(fesCycling); 
        if(fesCycling >= fesCyclingMax){
            fesCyclingBool = true;
            fesCycling = 5;
            dispositivo.fes[2].setPowerLevel(0); 
            std::cout << "Mudou 0" << std::endl;
        }
        fesCycling += 0.25; 
    }   
//   client->write(ss.str().c_str());
//   std::cout << ss.str().c_str() << std::endl;
    // }
    // std::cout << fesBike_counter << std::endl;
    fesBike_counter++;
    if(fesBike_counter==(int)param)
    {
        dispositivo.fes[0].setPowerLevel(0); 
        dispositivo.fes[1].setPowerLevel(0); 
        dispositivo.fes[2].setPowerLevel(0); 
        dispositivo.fes[3].setPowerLevel(0); 

        printf("stop\r\n"); //Print information
        ESP_ERROR_CHECK(esp_timer_stop(fesBike_periodic_timer)); //Timer pause
        ESP_ERROR_CHECK(esp_timer_delete(fesBike_periodic_timer)); //Timer delete
        fesBike_periodic_timer = nullptr;
        fesBike_flag = false;
        openLoopFesStop();
        
    }
}

String fesBikeStart(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 6){
    Serial.print("Operation 6, received data: "); Serial.println(msg);
    // dispositivo.timeOnAndPeriodUpdate(code(0,3),code(0,4));
    openLoopFesInit(code(0,3),code(0,4));
    fesBike_counter = 0;
    fesCyclingMax = code(0,2);
    fesBike_periodic_timer_args.callback = &fesBikeLoop;
    fesBike_periodic_timer_args.name = "fesBikeLoop";
    fesBike_periodic_timer_args.arg = (void*)(int (code(0,0)/code(0,1)));
    ESP_ERROR_CHECK(esp_timer_create(&fesBike_periodic_timer_args, &fesBike_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(fesBike_periodic_timer, code(0,1)*1000000.0));
    
    if(!noAnswer)
        answer += "FesBike iniciada\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif