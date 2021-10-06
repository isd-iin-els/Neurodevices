#ifndef closedLoopFes_h
#define closedLoopFes_h
#include "openLoopFes.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"
#include "mpu6050Config.h"

LinAlg::Matrix<double> closedLoop_gyData(3,1);

volatile uint64_t closedLoop_Counter, closedLoopIMU_Counter; volatile bool closedLoop_flag = false;
volatile ControlHandler::PID<long double> pid[2]; volatile long double *ref; uint8_t refSize = 0;
esp_timer_create_args_t closedLoop_periodic_timer_args, IMUClosedLoop_periodic_timer_args;
esp_timer_handle_t closedLoop_periodic_timer = nullptr, IMUClosedLoop_periodic_timer = nullptr;

// void IRAM_ATTR ClosedLoop_IMUDataLoop(void *param){
//   LinAlg::Matrix<double> gyDataTemp;
//   closedLoopIMU_Counter++;
//   // closedLoop_gyData += 0.05*(sensors.update()-closedLoop_gyData);
//   if(readDMP6()){
//       gyData = OUTPUT_YAWPITCHROLL() * 180/M_PI;
//       closedLoop_gyData += 0.05*(gyData(0.0,LinAlg::Matrix<double>("0,2,1"))-closedLoop_gyData);
//   }

//   if(closedLoopIMU_Counter==(int)param)
//   {
//     printf("stop\r\n"); //Print information
//     ESP_ERROR_CHECK(esp_timer_stop(IMUClosedLoop_periodic_timer)); //Timer pause
//     ESP_ERROR_CHECK(esp_timer_delete(IMUClosedLoop_periodic_timer)); //Timer delete
//     IMUClosedLoop_periodic_timer = nullptr;
//   }
// }

void IRAM_ATTR TwoDOFLimbControlLoop(void *param){
    closedLoop_Counter++;
    getEulerAngles(); 
    LinAlg::Matrix<double> U = dispositivo.TwoDOFLimbControl(ref[0], ref[1], ~mpuData);
    // LinAlg::Matrix<double> U = dispositivo.TwoDOFLimbControl(ref[0], ref[1], closedLoop_gyData);

    std::stringstream ss;
    // ss << closedLoop_Counter << " , " << closedLoop_gyData(0,0)  << " , " << closedLoop_gyData(1,0) << " , " << closedLoop_gyData(2,0) 
    ss << closedLoop_Counter << " , " << mpuData(0,0)  << " , " << mpuData(0,1) << " , " << mpuData(0,2) 
                             << " , " << U(0,0) << " , " << U(0,1) <<  " , " << 0 << "\r\n";
    client->write(ss.str().c_str());

  if(closedLoop_Counter==(int)param)
  {
    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 
    if(refSize == 2){
      ref[0] = 0; ref[1] = 0;
    }
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(closedLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(closedLoop_periodic_timer)); //Timer delete
    closedLoop_periodic_timer = nullptr;
    closedLoop_flag = false;
    openLoopFesStop();
  }
}

String TwoDOFLimbFesControl(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  // LinAlg::Matrix<double> code = msg.c_str();
  // std::cout << code << std::endl; 

  String answer;
  if (operation == CLOSEDLOOPFESCONTROL_MSG && !closedLoop_flag){
    // Serial.print("Operation 3, received data: "); Serial.println(msg);
    closedLoop_flag = true;
    // const char *msg = doc["m"];
    // LinAlg::Matrix<double> code = msg;
    // sensors.init();
    // dispositivo.resetTimeOnAndPeriod(code(0,3),code(0,4));
    mpuInit();
    openLoopFesInit(doc["Ton"], doc["period"]);
    
    dispositivo.getPID(0).setParams(doc["kp1"],doc["ki1"],doc["kd1"]); dispositivo.getPID(0).setSampleTime(doc["controlSampleTime"]); dispositivo.getPID(0).setLimits(doc["minInputLimit1"],doc["maxInputLimit1"]);
    dispositivo.getPID(1).setParams(doc["kp2"],doc["ki2"],doc["kd2"]); dispositivo.getPID(1).setSampleTime(doc["controlSampleTime"]); dispositivo.getPID(1).setLimits(doc["minInputLimit2"],doc["maxInputLimit2"]);
    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 
    ref = new volatile long double[2];
    refSize = 2;
    ref[0] = doc["ref1"]; ref[1] = doc["ref2"];

    closedLoop_periodic_timer_args.callback = &TwoDOFLimbControlLoop;
    closedLoop_periodic_timer_args.name = "TwoDOFLimbControlLoop";
    closedLoop_periodic_timer_args.arg = (void*)(int (float(doc["simulationTime"])/float(doc["controlSampleTime"])));
    ESP_ERROR_CHECK(esp_timer_create(&closedLoop_periodic_timer_args, &closedLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(closedLoop_periodic_timer, float(doc["controlSampleTime"])*1000000.0));
    closedLoop_Counter = 0;

    
    // IMUClosedLoop_periodic_timer_args.callback = &ClosedLoop_IMUDataLoop;
    // IMUClosedLoop_periodic_timer_args.name = "imuSendInit";
    // IMUClosedLoop_periodic_timer_args.arg = (void*)(int (code(0,0)/code(0,2)));
    // ESP_ERROR_CHECK(esp_timer_create(&IMUClosedLoop_periodic_timer_args, &IMUClosedLoop_periodic_timer));
    // ESP_ERROR_CHECK(esp_timer_start_periodic(IMUClosedLoop_periodic_timer, code(0,2)*1000000.0));
    closedLoopIMU_Counter = 0;
    if(!noAnswer)
      answer += "Loop para estimulacao em malha fechada\r\n";
  }
  else
    answer += "";
  return answer;
}

String closedLoopFesReferenceUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  // LinAlg::Matrix<double> code = msg.c_str();


  String answer;
  if (operation == CLOSEDLOOPFESREFERENCEUPDATE_MSG){
    // Serial.print("Oeration 4, received data: "); Serial.println(msg);
    // if(closedLoop_flag)
    // String msg = "ref0";
    // for(uint8_t i = 1; i <= doc["pidQtd"]; ++i){
    //   msg[msg.length()-1] = '0'+i;
    //   ref[i] = doc[msg];
    // }
    ref[0] = doc["ref1"]; ref[1] = doc["ref2"];
    if(!noAnswer)
      answer += "Referencias dos controladores PID atualizadas\r\n";
  }
  else
    answer += "";
  return answer;
}

String PIDsParametersUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  // LinAlg::Matrix<double> code = msg.c_str();
  String answer;
  if (operation == PIDSPARAMETERSUPDATE_MSG){
    // Serial.print("Operation 5, received data: "); Serial.println(msg);
    // if(closedLoop_flag){

    dispositivo.getPID(0).setParams(doc["kp1"],doc["ki1"],doc["kd1"]); 
    dispositivo.getPID(0).setSampleTime(doc["controlSampleTime"]); 
    dispositivo.getPID(0).setLimits(doc["minInputLimit1"],doc["maxInputLimit1"]);
    dispositivo.getPID(0).setInputOperationalPoint(doc["operationalP1"]);

    dispositivo.getPID(1).setParams(doc["kp2"],doc["ki2"],doc["kd2"]); 
    dispositivo.getPID(1).setSampleTime(doc["controlSampleTime"]); 
    dispositivo.getPID(1).setLimits(doc["minInputLimit2"],doc["maxInputLimit2"]);
    dispositivo.getPID(1).setInputOperationalPoint(doc["operationalP2"]);
      
    // String kp = "kp0", ki = "ki0", kd = "kd0", iminLimit = "minInputLimit0", imaxLimit = "maxInputLimit0", op = "operationalP0";
    
    // for(uint8_t i = 1; i <= doc["pidQtd"]; ++i){
    //   kp[kp.length()-1] = '0'+i; ki[ki.length()-1] = '0'+i; kd[kd.length()-1] = '0'+i;
    //   iminLimit[iminLimit.length()-1] = '0'+i; imaxLimit[imaxLimit.length()-1] = '0'+i;
    //   op[op.length()-1] = '0'+i;
    //   dispositivo.getPID(i-1).setParams(doc[kp],doc[ki],doc[kd]);  dispositivo.getPID(i-1).setLimits(doc[iminLimit],doc[imaxLimit]); dispositivo.getPID(i).setInputOperationalPoint(doc[op]);
    // }
    if(!noAnswer)
      answer += "Parametros dos PIDs atualizados\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif