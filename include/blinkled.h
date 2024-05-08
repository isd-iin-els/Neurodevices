#ifndef BLINKLEDMQTT_h
#define BLINKLEDMQTT_h
#include "openLoopFes.h"
#include "ArduinoJson.h"
#define json_size 512

volatile uint64_t blink_Counter; volatile bool blink_flag = false;
esp_timer_create_args_t blink_periodic_timer_args;
esp_timer_handle_t blink_periodic_timer = nullptr;
volatile bool level = false; int blinkTime = 4;
void IRAM_ATTR blink(void *param){
  blink_Counter++;
  // gpio_set_level(gpio_num_t (2), level);blinkMe(StaticJsonDocument<sizejson>(), 10);
  dispositivo.fes[2].setPowerLevel(level);
  level = !level;
  //Serial.println(level);
  //digitalWrite(2,level);
  if(blink_Counter==(int)param)
  {
    dispositivo.fes[2].setPowerLevel(0);
    //printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(blink_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(blink_periodic_timer)); //Timer delete
    blink_periodic_timer = nullptr;
    blink_flag = false;
    
  }
}

String blinkMe(const StaticJsonDocument<json_size> &doc/*, const uint8_t &operation*/)  {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  String answer;
  // if (operation == BLINKME_MSG){
    blink_periodic_timer_args.callback = &blink;
    blink_periodic_timer_args.name = "blink";
    blink_periodic_timer_args.arg = (void*)(int (blinkTime));
    ESP_ERROR_CHECK(esp_timer_create(&blink_periodic_timer_args, &blink_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(blink_periodic_timer, 500000.0));
    blink_Counter = 0;
    std::stringstream ss;
    ss << "start blinking\n";
    answer = ss.str().c_str();
    return answer;
  // }
  // answer = "";
  return answer;
}

#endif