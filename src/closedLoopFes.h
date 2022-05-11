#ifndef closedLoopFes_h
#define closedLoopFes_h
#include "openLoopFes.h"

LinAlg::Matrix<double> closedLoop_gyData(3,1), ref(1,2);

volatile uint64_t closedLoop_Counter, closedLoopIMU_Counter; volatile bool closedLoop_flag = false;
volatile ControlHandler::PID<long double> pid[2]; 
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
void getEulerAnglesGY80(){
    // mpu.getEvent(&a, &g, &temp);
    LinAlg::Matrix<double> sensorsRaw = sensors.updateRaw();
    // Serial.println("1.1");
    // std::cout << closedLoop_gyData << std::endl;
    closedLoop_gyData(0,0) = get_pitch(sensorsRaw(0,0), sensorsRaw(0,1), sensorsRaw(0,2));
    // Serial.println("1.2");
    closedLoop_gyData(1,0) = get_roll(sensorsRaw(0,0), sensorsRaw(0,1), sensorsRaw(0,2));
    // if(mpuData(0,1) < 0) mpuData(0,1) = mpuData(0,1) +360;
}

void IRAM_ATTR TwoDOFLimbControlLoop(void *param){
    closedLoop_Counter++;
    // getEulerAngles(); 
    // if(mpu6050Flag)
    // LinAlg::Matrix<double> U = dispositivo.TwoDOFLimbControl(ref(0,0), ref(0,1), ~mpuData);
    // else if(gy80Flag){
      // Serial.println("1");
    getEulerAnglesGY80();
    // Serial.println("2");
    LinAlg::Matrix<double> U = dispositivo.TwoDOFLimbControl(ref(0,0), ref(0,1), closedLoop_gyData);
    // Serial.println("3");
    std::stringstream ss;
    // ss << closedLoop_Counter << ",a1:" << mpuData(0,0)  << ",a2:" << mpuData(0,1) << ",a3:" << mpuData(0,2) // <<  "\r\n";
    ss << closedLoop_Counter << ",a1:" << closedLoop_gyData(0,0)  << ",a2:" << closedLoop_gyData(1,0) << ",a3:" << closedLoop_gyData(2,0) 
                             << ",U1:" << U(0,0) << ",U2:" << U(0,1) <<  ",R1:" 
                             << ref(0,0) <<  ",R2:" << ref(0,1) <<  ",E1:" << U(0,2) <<  ",E2:" << U(0,3) 
                             <<  ",IE1:" << U(0,4) <<  ",IE2:" << U(0,5) <<  "\r\n";
                          
    std::cout << ss.str().c_str();
    #ifdef WiFistaTCP_h
      client->write(ss.str().c_str());
    #endif
    #ifdef WiFistaMQTT_h
      mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
    #endif

  if(closedLoop_Counter==(int)param)
  {
    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 
    
    for(int i = 0; i < ref.getNumberOfColumns(); ++i)
      ref(0,i) = 0;

    #ifdef WiFistaTCP_h
      client->write("stop\r\n");
    #endif
    #ifdef WiFistaMQTT_h
      mqttClient.publish(devstream.str().c_str(), 0, false, "stop\r\n");
    #endif
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(closedLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(closedLoop_periodic_timer)); //Timer delete
    closedLoop_periodic_timer = nullptr;
    closedLoop_flag = false;
    openLoopFesStop();
  }
}

String TwoDOFLimbFesControl(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {

  String answer;
  if (operation == CLOSEDLOOPFESCONTROL_MSG && !closedLoop_flag){
    closedLoop_flag = true;
    // mpuInit();
    sensors.init();
    openLoopFesInit(doc["Ton"], doc["period"]);
    String kpt = doc["kp"], kit  = doc["ki"], kdt = doc["kd"], minInputLimitt  = doc["minInputLimit"], 
           maxInputLimitt  = doc["maxInputLimit"], opPt  = doc["operationalP"], referencet  = doc["ref"];
    LinAlg::Matrix<double> kp = kpt.c_str(), ki = kit.c_str(), kd = kdt.c_str(), minInputLimit = minInputLimitt.c_str(), 
                           maxInputLimit = maxInputLimitt.c_str(), opP = opPt.c_str(), ref = referencet.c_str();

    
    for (unsigned i = 0; i < kp.getNumberOfColumns(); ++i){
      std::cout << "Kp:" << kp(0,i) << "Ki:" << ki(0,i) << "Kd:" << kd(0,i) 
                << "controlSampleTime:" << doc["controlSampleTime"] 
                << "minInputLimit:" << minInputLimit(0,i) << "maxInputLimit:" << maxInputLimit(0,i) 
                << "opP:" << opP(0,i) << "\n";
      dispositivo.getPID(i).setParams(kp(0,i),ki(0,i),kd(0,i)); 
      dispositivo.getPID(i).setSampleTime(float(doc["controlSampleTime"])); 
      dispositivo.getPID(i).setLimits(minInputLimit(0,i),maxInputLimit(0,i));
      dispositivo.getPID(i).setInputOperationalPoint(opP(0,i));
      dispositivo.getPID(i).resetIntegralValue();
      std::cout << "passou" << i << "\n";
    }
    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 

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
    // closedLoopIMU_Counter = 0;
    answer += "Loop para estimulacao em malha fechada\r\n";
  }
  else
    answer += "";
  return answer;
}

String closedLoopFesReferenceUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == CLOSEDLOOPFESREFERENCEUPDATE_MSG){
    String referencet  = doc["r"];
    ref = referencet.c_str();
    std::cout << referencet.c_str() << std::endl;
    answer += "Referencias dos controladores PID atualizadas\r\n";
  }
  else
    answer += "";
  return answer;
}

String PIDsParametersUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (operation == PIDSPARAMETERSUPDATE_MSG){
    String kpt = doc["kp"], kit  = doc["ki"], kdt = doc["kd"], minInputLimitt  = doc["minInputLimit"], 
           maxInputLimitt  = doc["maxInputLimit"], opPt  = doc["operationalP"], referencet  = doc["r"];
    LinAlg::Matrix<double> kp = kpt.c_str(), ki = kit.c_str(), kd = kdt.c_str(), minInputLimit = minInputLimitt.c_str(), 
                           maxInputLimit = maxInputLimitt.c_str(), opP = opPt.c_str();

    for (unsigned i = 0; i < kp.getNumberOfColumns(); ++i){
      std::cout << "Kp:" << kp(0,i) << "Ki:" << ki(0,i) << "Kd:" << kd(0,i) 
                << "controlSampleTime:" << doc["controlSampleTime"] 
                << "minInputLimit:" << minInputLimit(0,i) << "maxInputLimit:" << maxInputLimit(0,i) 
                << "opP:" << opP(0,i) << "\n";
      dispositivo.getPID(i).setParams(kp(0,i),ki(0,i),kd(0,i)); 
      dispositivo.getPID(i).setSampleTime(float(doc["controlSampleTime"])); 
      dispositivo.getPID(i).setLimits(minInputLimit(0,i),maxInputLimit(0,i));
      dispositivo.getPID(i).setInputOperationalPoint(opP(0,i));
    }
    
    answer += "Parametros dos PIDs atualizados\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif