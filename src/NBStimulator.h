#ifndef NBSTIMULATOR_h
#define NBSTIMULATOR_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "wifistaTCP.h"

volatile uint64_t BN_Counter; volatile bool BN_flag = false, stopFlag = false;
esp_timer_create_args_t BN_periodic_timer_args;
esp_timer_handle_t BN_periodic_timer = nullptr;

void IRAM_ATTR BNUpdate(void *param){
    BN_Counter++;
    std::stringstream ss;
    ss << BN_Counter << " , " << analogReadMilliVolts(36)  << "\r\n";
    client->write(ss.str().c_str());

  if(BN_Counter==(int)param || stopFlag)
  {
    client->write("stop\r\n");
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(BN_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(BN_periodic_timer)); //Timer delete
    BN_periodic_timer = nullptr;
    BN_flag = false;
  }
}

String neurogenic_bladder_stop(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();
  std::cout << code << std::endl; 

  if (op.toInt() == 12){
    stopFlag = true;

    if(!noAnswer)
      answer += "Loop para estimulacao de neurogenic bladder\r\n";
  }
  else
    answer += "";
  return answer;
}

String neurogenic_bladder_init(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();
  std::cout << code << std::endl; 

  if (op.toInt() == 11 && !BN_flag){
    Serial.print("Operation 11, received data: "); Serial.println(msg);
    BN_flag = true;
    stopFlag = false;

    BN_periodic_timer_args.callback = &BNUpdate;
    BN_periodic_timer_args.name = "BNUpdate";
    BN_periodic_timer_args.arg = (void*)(int (code(0,0)*code(0,1)));
    ESP_ERROR_CHECK(esp_timer_create(&BN_periodic_timer_args, &BN_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(BN_periodic_timer, 1000000.0/code(0,1)));
    BN_Counter = 0;

    if(!noAnswer)
      answer += "Loop para estimulacao de neurogenic bladder\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif