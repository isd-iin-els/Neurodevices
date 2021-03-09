#include <sstream>
#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include "wifistaTCP.h"
#include "mpuDMP6.h"
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"

LinAlg::Matrix<double> gyData;

esp_timer_create_args_t IMUDataLoop_periodic_timer_args;
esp_timer_handle_t IMUDataLoop_periodic_timer = nullptr;
volatile uint64_t IMUDataLoop_counter = 0;
volatile bool IMUDataLoop_flag = false;

void IRAM_ATTR IMUDataLoop(void *param){
  gyData = sensors.update();
  IMUDataLoop_counter++;
  //  if(readDMP6()){
  //     gyData = OUTPUT_YAWPITCHROLL() * 180/M_PI;
      std::stringstream ss; ss << IMUDataLoop_counter; ss <<= gyData;
      client->write(ss.str().c_str());
    // }

  if(IMUDataLoop_counter==(int)param)
  {
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(IMUDataLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(IMUDataLoop_periodic_timer)); //Timer delete
    IMUDataLoop_periodic_timer = nullptr;
    IMUDataLoop_flag = false;
  }
}

String imuSendInit(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());

  if (op.toInt() == 1){
    Serial.print("Oeration 1, received data: "); Serial.println(msg);
    // initDMP6(gpio_num_t(23));
    sensors.init();
    IMUDataLoop_periodic_timer_args.callback = &IMUDataLoop;
    IMUDataLoop_periodic_timer_args.name = "imuSendInit";
    IMUDataLoop_periodic_timer_args.arg = (void*)msg.toInt();
    ESP_ERROR_CHECK(esp_timer_create(&IMUDataLoop_periodic_timer_args, &IMUDataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(IMUDataLoop_periodic_timer, 1000));
    IMUDataLoop_flag = true;
    IMUDataLoop_counter =0;
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

