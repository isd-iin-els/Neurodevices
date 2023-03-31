#ifndef sendimudata_h
#define sendimudata_h

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

LinAlg::Matrix<double> gyData; 
double freq = 75; 
double period = 1/freq;

esp_timer_create_args_t IMUDataLoop_periodic_timer_args;
esp_timer_handle_t IMUDataLoop_periodic_timer = nullptr;
volatile uint64_t IMUDataLoop_counter = 0;
volatile bool IMUDataLoop_flag = false;
volatile bool mpu6050Flag = false, gy80Flag = false;
// double pitch, roll, rad2degree = 180/M_PI;

// double get_pitch( double ax, double ay, double az){
//     return atan2(-1*ax, sqrt(ay*ay + az*az ));
// }
		
// double get_roll(double ax, double ay, double az){
//     return atan2(ay, az + 0.05*ax);
// }

// double get_yaw(double magx, double magy, double magz, double pitch, double roll){
// 		return atan2(sin(roll)*magz - cos(roll)*magy,	cos(pitch)*magx + sin(roll)*sin(pitch)*magy + cos(roll)*sin(pitch)*magz);
//     //return atan2(magy,magx); 
// }

static String getIMUData(){
  std::stringstream ss; 
  if(mpu6050Flag){
    getEulerAngles();
    ss << mpuData(0,0) << "," << mpuData(0,1)  << "," << mpuData(0,2) << std::endl;
  }
  else if(gy80Flag){
    gyData = sensors.updateRaw();
    ss <<= gyData;
    ss << std::endl;
  }
  else{
    ss << 2.0 << "," << 3.0 << "," << 4.0 
                              << "," << 5.0 << "," << 6.0 << "," << 7.0 << "," << 8.0 << "," << 9.0 << std::endl;
  }
  return ss.str().c_str();
}

static void IMUDataLoop(void *param){
  
  // gyData = sensors.update();
  // ss << IMUDataLoop_counter << "," << gyData(0,0) << "," << gyData(1,0) << "," << gyData(2,0) << std::endl;
  std::stringstream ss; 
  if(mpu6050Flag){
    IMUDataLoop_counter++;
    getEulerAngles();
    // ss << IMUDataLoop_counter << "," << mpuData(0,0) << "," << mpuData(0,1)  << "," << mpuData(0,2) << std::endl;
    ss << mpuData(0,0) << "," << mpuData(0,1)  << "," << mpuData(0,2) << std::endl;
  }
  else if(gy80Flag){
    IMUDataLoop_counter++;
    gyData = sensors.updateRaw();
    // ss << IMUDataLoop_counter << "," ;
    ss <<= gyData;
    ss << std::endl;
  }
  else{
    IMUDataLoop_counter++;
    // ss << IMUDataLoop_counter << "," << 1.0122 << "," << 2.0122 << "," << 3.0122 << "," << 4.0122 
    ss << IMUDataLoop_counter << 2.0122 << "," << 3.0122 << "," << 4.0122 
                              << "," << 5.0122 << "," << 6.0122 << "," << 7.0122 << "," << 8.0122 << "," << 9.0122 << std::endl;
  }
  #ifdef WiFistaTCP_h
    client->write(ss.str().c_str());
  #endif
  #ifdef WiFistaMQTT_h
    mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
  #endif
  // Serial.println(ss.str().c_str());
  //  if(readDMP6()){
  //     // gyData = OUTPUT_YAWPITCHROLL() * 180/M_PI;
  //     gyData = OUTPUT_RAWACCEL();
  //     pitch = get_pitch( gyData(0,0), gyData(0,1), gyData(0,2));
  //     roll = get_roll( gyData(0,0), gyData(0,1), gyData(0,2));
  //****************************************
  //  //comentei initmpu
  //****************************************
  
    // std::stringstream ss; //ss << std::setw(2*coutPrecision+1) << std::setprecision(coutPrecision) << std::fixed;
    // ss << IMUDataLoop_counter << "," << gyData(0,0) << "," << gyData(0,1) << "," << gyData(0,2) << std::endl;
  
  // pitch = get_pitch( gyData(0,0), gyData(0,1), gyData(0,2));
  // roll = get_roll( gyData(0,0), gyData(0,1), gyData(0,2));
  // ss << IMUDataLoop_counter << "," << pitch << ",  ";
  // // ss << ',' << IMUDataLoop_counter << "," << pitch << ",  ";//matlab
  // ss <<  roll << ",  ";
  // ss << get_yaw( gyData(0,6), gyData(0,7), gyData(0,8),pitch,roll) << "\r\n";
  //ss << gx << ",    " << gy << ",    " << gz << "\r\n";//gyData(0,0) << ',    ' << gyData(0,1) << ',    ' << gyData(0,2) << ',    ' << gx*rad2degree << ',    ' << gy*rad2degree << ',    ' << gz*rad2degree << ',    ' << gyData(0,6) << ',    ' << gyData(0,7) << ',    ' << gyData(0,8) << '\r\n';

   
    std::cout << ss.str().c_str() << std::endl;
  // }

  // if(IMUDataLoop_counter>=(int)param)
  // {
  //   ESP_ERROR_CHECK(esp_timer_stop(IMUDataLoop_periodic_timer)); //Timer pause
  //   ESP_ERROR_CHECK(esp_timer_delete(IMUDataLoop_periodic_timer)); //Timer delete
  //   IMUDataLoop_periodic_timer = nullptr;
  //   IMUDataLoop_flag = false;
  //   std::cout << "Finalizou\n";
  // }
}

String imuSendStop(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUSENDSTOP__MSG && IMUDataLoop_flag){
    if(IMUDataLoop_periodic_timer != nullptr){
      ESP_ERROR_CHECK(esp_timer_stop(IMUDataLoop_periodic_timer)); //Timer pause
      ESP_ERROR_CHECK(esp_timer_delete(IMUDataLoop_periodic_timer)); //Timer delete
    }
      IMUDataLoop_periodic_timer = nullptr;
      IMUDataLoop_flag = false;
      // IMUDataLoop_counter = -1;
      answer = "IMU Stopped";
      // std::cout << IMUDataLoop_counter << " " << answer.c_str() << std::endl;
  } else
    answer += "";
  return answer;
}

String imuSendInit(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUSENDINIT_MSG && !IMUDataLoop_flag){
    uint8_t sensorType = doc["sensorType"];
    uint16_t freq = doc["frequence"];
    int64_t timeSimulation = doc["simulationTime"];

    

    // if(sensorType == 1 && !mpu6050Flag){
    //   std::cout << "inicializando sensor"<< "\n";
    //   mpu6050Flag = mpuInit();
    // }
    // else if(sensorType == 2 && !gy80Flag){
      std::cout << "inicializando sensor"<< "\n";
      gy80Flag = sensors.init();
    // }
    
    std::cout << "sensor: " << gy80Flag << "\n"; 
    // initDMP6(gpio_num_t(23));
    // gy80Flag = sensors.init();
    // mpu6050Flag = mpuInit();
    // if (!){
    //   answer += "Erro ao acessar o IMU";
    //   return answer;
    // }
    IMUDataLoop_counter = 0;
    IMUDataLoop_flag = true;
    IMUDataLoop_periodic_timer_args.callback = &IMUDataLoop;
    IMUDataLoop_periodic_timer_args.name = "imuSendInit";
    IMUDataLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
    ESP_ERROR_CHECK(esp_timer_create(&IMUDataLoop_periodic_timer_args, &IMUDataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(IMUDataLoop_periodic_timer, 1000000.0/freq));
    
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

// void calibAcc(void *arg){
//     LinAlg::Matrix<int16_t> raw(1,3);
//     sensors.setAccOffset(raw);
//     gyData = sensors.updateRaw();
//     for (uint8_t i = 0; i< 100; ++i){
//       gyData += sensors.updateRaw()/100.0;
//       // LinAlg::Matrix<double> read = sensors.updateRaw();
//       // for(int8_t j = 0; j< read)
//       // gyData += 
//     }
//     raw(0,0) = (gyData(0,0)-1)/0.00390625;
//     raw(0,1) = gyData(0,1)/0.00390625;
//     raw(0,2) = gyData(0,2)/0.00390625;
    
//     sensors.setAccOffset(raw);
//     sensors.saveAccOffset();
//     gyData = sensors.updateRaw();
//     std::cout << gyData << std::endl;
//     vTaskDelete(NULL);
// }

String imuAccelerometerCalibration(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUACCELEROMETERCALIBRATION_MSG && !IMUDataLoop_flag){
    uint8_t sensorType = doc["sensorType"];
    uint16_t freq = doc["frequence"];

    if(sensorType == 1 && !mpu6050Flag){
      std::cout << "inicializando sensor"<< "\n";
      mpu6050Flag = mpuInit();
    }
    else if(sensorType == 2){
      std::cout << "Calibrando Acelerometro"<< "\n";
      gy80Flag = sensors.init();
      if(gy80Flag){
        LinAlg::Matrix<int16_t> raw(1,3);
        sensors.setAccOffset(raw);
        gyData = sensors.updateRaw();
        std::cout << "Antes de Calibrar"<< gyData << "\n";
        for (uint8_t i = 0; i< 100; ++i){
          gyData += sensors.updateRaw();
          delay(14);
          // LinAlg::Matrix<double> read = sensors.updateRaw();
          // for(int8_t j = 0; j< read)
          // gyData += 
        }
        gyData /= 0.390625;
        std::cout << "Valor medio Lido: "<< gyData << "\n";
        raw(0,0) = (gyData(0,0)-256);
        raw(0,1) = gyData(0,1);
        raw(0,2) = gyData(0,2);
        
        sensors.setAccOffset(raw);
        sensors.saveAccOffset();
        gyData = sensors.updateRaw();
        std::cout << "Depois de Calibrar"<< gyData << std::endl;
      }
        // xTaskCreatePinnedToCore(calibAcc, 
        //               "CalibAcc", 
        //               16384, 
        //               NULL, 
        //               4, 
        //               NULL,
        //               APP_CPU_NUM);
    }
    
    // std::cout << "sensor: " << gy80Flag << "\n"; 
    answer += "Acelerômetro calibrado com Sucesso\r\n";
  }
  else
    answer += "";
  return answer;
}

// void calibGyr(void *arg){
//     LinAlg::Matrix<int16_t> raw(1,3);
//     sensors.setGyrOffset(raw);
//     gyData = sensors.updateRaw();
//     for (uint8_t i = 0; i< 100; ++i){
//       gyData += sensors.updateRaw()/100.0;
//     }
//     raw(0,0) = gyData(0,3)/0.00875;
//     raw(0,1) = gyData(0,4)/0.00875;
//     raw(0,2) = gyData(0,5)/0.00875;
//     gyData = sensors.updateRaw();
    
//     sensors.setGyrOffset(raw);
//     sensors.saveGyrOffset();

//     std::cout << gyData << std::endl;
//     vTaskDelete(NULL);
// }

String imuGiroscopeCalibration(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUGIROSCOPECALIBRATION_MSG && !IMUDataLoop_flag){
    uint8_t sensorType = doc["sensorType"];
    uint16_t freq = doc["frequence"];

    if(sensorType == 1 && !mpu6050Flag){
      std::cout << "inicializando sensor"<< "\n";
      mpu6050Flag = mpuInit();
    }
    else if(sensorType == 2){
      std::cout << "Calibrando Giroscopio"<< "\n";
      gy80Flag = sensors.init();
      if(gy80Flag){
        LinAlg::Matrix<int16_t> raw(1,3);
        sensors.setGyrOffset(raw);
        gyData = sensors.updateRaw();
        std::cout << "Antes de Calibrar"<< gyData << "\n";
        for (uint8_t i = 0; i< 100; ++i){
          gyData += sensors.updateRaw();
          delay(14);
        }
        gyData /= 0.875;
        std::cout << "Valor medio Lido: "<< gyData << "\n";
        
        raw(0,0) = gyData(0,3);
        raw(0,1) = gyData(0,4);
        raw(0,2) = gyData(0,5);
    
        sensors.setGyrOffset(raw);
        sensors.saveGyrOffset();
        gyData = sensors.updateRaw();
        std::cout << "Valor de Comparacao"<< gyData/0.00875 << std::endl;
        std::cout << "Depois de Calibrar"<< gyData << std::endl;
      }
      // xTaskCreatePinnedToCore(calibGyr, 
      //                   "CalibGyr", 
      //                   16384, 
      //                   NULL, 
      //                   4, 
      //                   NULL,
      //                   APP_CPU_NUM);
    }
  
    answer += "Giroscópio calibrado com Sucesso\r\n";
  }
  else
    answer += "";
  return answer;
}



void calibMag(void *arg){
   int32_t min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
   LinAlg::Matrix<int16_t> raw(1,3),mag_calib(1,3);
   sensors.setGyrOffset(raw);
    for (unsigned i = 0; i < 1200; ++i)
    {
        gyData = sensors.updateRaw();
        raw(0,0) = gyData(0,6)/0.92;
        raw(0,1) = gyData(0,7)/0.92;
        raw(0,2) = gyData(0,8)/0.92;
        if (raw(0,0) < min_x)
            min_x = raw(0,0);
        if ( raw(0,1) < min_y)
            min_y =  raw(0,1);
        if (raw(0,2) < min_z)
            min_z = raw(0,2);

        if (raw(0,0) > max_x)
            max_x = raw(0,0);
        if ( raw(0,1) > max_y)
            max_y =  raw(0,1);
        if (raw(0,2) > max_z)
            max_z = raw(0,2);

        vTaskDelay(14 / portTICK_PERIOD_MS);
        // printf("%d\n",i);
        std::cout << raw << min_x << max_x << std::endl;
    }

    // printf("Aqui\n");
    mag_calib(0,0) = (max_x + min_x) / 2;
    mag_calib(0,1) = (max_y + min_y) / 2;
    mag_calib(0,2) = (max_z + min_z) / 2;
    sensors.setMagOffset(mag_calib);
    sensors.saveMagOffset();
    vTaskDelete(NULL);
}

String imuMagnetometerCalibration(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == IMUMAGNETOMETERCALIBRATION_MSG && !IMUDataLoop_flag){
    uint8_t sensorType = doc["sensorType"];
    uint16_t freq = doc["frequence"];

    if(sensorType == 1 && !mpu6050Flag){
      std::cout << "inicializando sensor"<< "\n";
      mpu6050Flag = mpuInit();
    }
    else if(sensorType == 2){
      std::cout << "Calibrando Magnetometro"<< "\n";
      gy80Flag = sensors.init();
      if(gy80Flag)
        xTaskCreatePinnedToCore(calibMag, 
                        "CalibMag", 
                        16384, 
                        NULL, 
                        4, 
                        NULL,
                        APP_CPU_NUM);

    }
    
    answer += "Magnetômetro calibrado com Sucesso\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif