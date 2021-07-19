#include <sstream>
#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include "wifistaTCP.h"
// #include "mpuDMP6.h"
#include "mpu6050Config.h"
// #define M_PI    3.14159265358979323846
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"

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

static void IMUDataLoop(void *param){
  
  // gyData = sensors.update();
  // ss << IMUDataLoop_counter << "," << gyData(0,0) << "," << gyData(1,0) << "," << gyData(2,0) << std::endl;
  std::stringstream ss; 
  if(mpu6050Flag){
    IMUDataLoop_counter++;
    getEulerAngles();
    ss << IMUDataLoop_counter << "," << mpuData(0,0) << "," << mpuData(0,1)  << "," << mpuData(0,2) << std::endl;
  }
  else if(gy80Flag){
    IMUDataLoop_counter++;
    gyData = sensors.updateRaw();
    ss << IMUDataLoop_counter << "," ;
    ss <<= gyData;
    ss << std::endl;
  }
  else{
    IMUDataLoop_counter++;
    ss << IMUDataLoop_counter << "," << 1.0 << "," << 2.0 << "," << 3.0 << "," << 4.0 
                              << "," << 5.0 << "," << 6.0 << "," << 7.0 << "," << 8.0 << "," << 9.0 << std::endl;
  }
  client->write(ss.str().c_str());
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

   
    // std::cout << ss.str().c_str() << std::endl;
  // }

  if(IMUDataLoop_counter==(int)param)
  {
    client->write("stop\r\n");
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
  LinAlg::Matrix<double> code = msg.c_str();
  std::cout << code;
  if (op.toInt() == 1 && !IMUDataLoop_flag){
    Serial.print("Operation 1, received data: "); Serial.println(msg);
    if(code.getNumberOfColumns() == 3)
    {
      if(code(0,2) == 1)
        mpu6050Flag = mpuInit();
      else if(code(0,2) == 2)
        gy80Flag = sensors.init();
    }
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
    IMUDataLoop_periodic_timer_args.arg = (void*)((int)(code(0,0)*code(0,1)));
    ESP_ERROR_CHECK(esp_timer_create(&IMUDataLoop_periodic_timer_args, &IMUDataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(IMUDataLoop_periodic_timer, 1000000.0/code(0,1)));
    
    
    std::cout << "Tudo Inicializado\n";
    if(!noAnswer)
      answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}

