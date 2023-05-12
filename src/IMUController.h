#ifndef imucontroller_h
#define imucontroller_h

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

// #include "mpuDMP6.h"
#include "mpu6050Config.h"
// #define M_PI    3.14159265358979323846
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"
#include <esp_ipc.h>

LinAlg::Matrix<double> IMUControllergyData; 
double IMUControllerfreq = 75; 
double IMUControllerperiod = 1/IMUControllerfreq;

esp_timer_create_args_t IMUControllerLoop_periodic_timer_args;
esp_timer_handle_t IMUControllerLoop_periodic_timer = nullptr;
volatile uint64_t IMUControllerLoop_counter = 0;
volatile bool IMUControllerLoop_flag = false;
volatile bool IMUControllergy80Flag = false;
// double pitch, roll, rad2degree = 180/M_PI;

double __get_pitch( double ax, double ay, double az){
  return atan2(-1*ax, sqrt(ay*ay + az*az ));
}
		
double __get_roll(double ax, double ay, double az){
  return atan2(ay, az + 0.05*ax);
}

double filterX=0, filterY=0, tyme = 0;
// char last_key = '0';
// std::string __keys[2]; 
char matrix[2][2] = {{'A', 'S'}, {'D', 'W'}};
char resultArr[2] = {'0', '0'};
char previousArr[2] = {'0', '0'};

static void IMUControllerLoop(void *param){
  std::stringstream ss; 
  if(IMUControllergy80Flag){
    IMUControllerLoop_counter++;
    IMUControllergyData = sensors.updateRaw();
    // ss << IMUControllerLoop_counter << "," ;
    filterX += 0.05*(IMUControllergyData(0,0)-filterX);
    filterY += 0.05*(IMUControllergyData(0,1)-filterY);
    ss <<= IMUControllergyData;
    ss << std::endl;
  }
  else{
    IMUControllerLoop_counter++;
    ss << IMUControllerLoop_counter << 2.0122 << "," << 3.0122 << "," << 4.0122 
                              << "," << 5.0122 << "," << 6.0122 << "," << 7.0122 << "," << 8.0122 << "," << 9.0122 << std::endl;
  }
  #ifdef WiFistaTCP_h
    client->write(ss.str().c_str());
  #endif
  #ifdef WiFistaMQTT_h
    // //IMUControllergyData(0,0), IMUControllergyData(0,1);
    // //mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
    // // if(millis()-tyme > 0.5){
    // //   if (filterX > 0.200)
    // //     mqttClient.publish("sendkey", 0, false,"D"); // D
    // //   else if (filterX < -0.200) 
    // //     mqttClient.publish("sendkey", 0, false, "W"); // W

    // //   else if (filterY > 0.200)
    // //     mqttClient.publish("sendkey", 0, false,"A"); // A
    // //   else if (filterY < -0.200) 
    // //     mqttClient.publish("sendkey", 0, false, "S"); // S

    // //     tyme = millis();
    // // }
    //     for (int i = 0; i < 2; i++) {
    //       float val=IMUControllergyData(0,i); 
    //       //cout << "coluna: "<< i << "   "; cin >> val;
    //       int pos_neg = 0;
    //       char key;
    
    //       if (val >= 0.2) pos_neg++;
    //       if (pos_neg || val <= -0.2) {
    //         key = __keys[pos_neg][i];
    //       }
    //       //else key = 'n';
    
    //       if (last_key != key) {
    //         last_key = key;
    //         char* strkey = new char[2];
    //         strkey[0] = key;strkey[1] = '\0';
    //         //cout << strkey;
    //         mqttClient.publish("sendkey", 0, false, strkey);
    //       }
    //     }

    

    for (int i = 0; i < 2; i++) {
      float val=IMUControllergyData(0,i);
      int pos_neg = 0;
      
      if (val >= 0.2) pos_neg++;
      if (pos_neg || val <= -0.2) resultArr[i] = matrix[pos_neg][i];
    }
    bool booleanArr[2] = {false, false};

    for (int i = 0; i < 2; i++) {
      if (previousArr[i] != resultArr[i]) {
        previousArr[i] = resultArr[i];
        booleanArr[i] = true;
      }
    }

    char* strkey = new char[3];
    strkey[2] = '\0';
    for (int i = 0; i < 2; i++) {
      if (booleanArr[i]) {
        strkey[i] = resultArr[i];
        mqttClient.publish("sendkey", 0, false, strkey);
      }else{
        strkey[i] = '0';
      }
    }
        //cout << strkey;

    
    
  #endif
    std::cout << ss.str().c_str() << std::endl;
  // }

  if(IMUControllerLoop_counter>=(int)param)
  {
    ESP_ERROR_CHECK(esp_timer_stop(IMUControllerLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(IMUControllerLoop_periodic_timer)); //Timer delete
    IMUControllerLoop_periodic_timer = nullptr;
    IMUControllerLoop_flag = false;
    std::cout << "Finalizou\n";
  }
}

String IMUControllerStop(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUControllerSTOP__MSG && IMUControllerLoop_flag){
    if(IMUControllerLoop_periodic_timer != nullptr){
      ESP_ERROR_CHECK(esp_timer_stop(IMUControllerLoop_periodic_timer)); //Timer pause
      ESP_ERROR_CHECK(esp_timer_delete(IMUControllerLoop_periodic_timer)); //Timer delete
    }
      IMUControllerLoop_periodic_timer = nullptr;
      IMUControllerLoop_flag = false;
      answer = "IMU Stopped";
  } else
    answer += "";
  return answer;
}

String IMUControllerInit(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  //__keys[0] = "AS"; __keys[1] = "DW";
  if (operation == IMUControllerINIT_MSG && !IMUControllerLoop_flag){
    uint8_t sensorType = doc["sensorType"];
    uint16_t freq = doc["frequence"];
    int64_t timeSimulation = doc["simulationTime"];


      std::cout << "inicializando sensor"<< "\n";
      IMUControllergy80Flag = sensors.init();
    
    std::cout << "sensor: " << IMUControllergy80Flag << "\n"; 

    IMUControllerLoop_counter = 0;
    IMUControllerLoop_flag = true;
    IMUControllerLoop_periodic_timer_args.callback = &IMUControllerLoop;
    IMUControllerLoop_periodic_timer_args.name = "IMUControllerInit";
    IMUControllerLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&IMUControllerLoop_periodic_timer_args, &IMUControllerLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(IMUControllerLoop_periodic_timer, 1000000.0/freq));
    
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif