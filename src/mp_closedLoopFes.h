#ifndef mpclosedLoopFes_h
#define mpclosedLoopFes_h
#include "openLoopFes.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"
#include "SistemasdeControle/embeddedTools/controlLibs/mp_pi.h"
#include "mpu6050Config.h"

LinAlg::Matrix<double> mpclosedLoop_gyData(3,1);

volatile uint64_t MP_PIDSclosedLoop_Counter, MP_PIDSclosedLoopIMU_Counter; volatile bool MP_PIDSclosedLoop_flag = false;
ControlHandler::MP_General_Controller<long double> *pi_pitch = new ControlHandler::MP_PI<long double>(), *pi_roll = new ControlHandler::MP_PI<long double>(); 
volatile long double *mpref; uint8_t mprefSize = 0;
esp_timer_create_args_t MP_PIDSclosedLoop_periodic_timer_args, IMUMP_PIDSClosedLoop_periodic_timer_args;
esp_timer_handle_t MP_PIDSclosedLoop_periodic_timer = nullptr, IMUMP_PIDSClosedLoop_periodic_timer = nullptr;
    
// void IRAM_ATTR MP_PIDSClosedLoop_IMUDataLoop(void *param){
//   LinAlg::Matrix<double> gyDataTemp;
//   MP_PIDSclosedLoopIMU_Counter++;
//   // mpclosedLoop_gyData += 0.05*(sensors.update()-mpclosedLoop_gyData);
//   if(readDMP6()){
//       gyData = OUTPUT_YAWPITCHROLL() * 180/M_PI;
//       mpclosedLoop_gyData += 0.05*(gyData(0.0,LinAlg::Matrix<double>("0,2,1"))-mpclosedLoop_gyData);
//   }

//   if(MP_PIDSclosedLoopIMU_Counter==(int)param)
//   {
//     printf("stop\r\n"); //Print information
//     ESP_ERROR_CHECK(esp_timer_stop(IMUMP_PIDSClosedLoop_periodic_timer)); //Timer pause
//     ESP_ERROR_CHECK(esp_timer_delete(IMUMP_PIDSClosedLoop_periodic_timer)); //Timer delete
//     IMUMP_PIDSClosedLoop_periodic_timer = nullptr;
//   }
// }

LinAlg::Matrix<double> TwoDOFLimbMP_PIDSControl(double mpref1, double mpref2, LinAlg::Matrix<double> sensorData){
    double u1, u2; LinAlg::Matrix<double> ret(1,2);
    u1 = pi_pitch->OutputControl(mpref1, sensorData(0,0));
    if (u1 > 0){
        // u1 += this->pid[0].getInputOperationalPoint();
        dispositivo.fes[0].setPowerLevel(u1); dispositivo.fes[1].setPowerLevel(0); 
    }
    else{
        // u1 = -u1 + this->pid[0].getInputOperationalPoint();
        dispositivo.fes[1].setPowerLevel(u1); dispositivo.fes[0].setPowerLevel(0); 
        u1 = -u1;
    }
    u2 = pi_roll->OutputControl(mpref2, sensorData(1,0));
    if (u2 > 0){
        // u2 += this->pid[1].getInputOperationalPoint();
        dispositivo.fes[2].setPowerLevel(u2); dispositivo.fes[3].setPowerLevel(0); 
    }
    else{
        // u2 = -u2 + this->pid[1].getInputOperationalPoint();
        dispositivo.fes[3].setPowerLevel(u2); dispositivo.fes[2].setPowerLevel(0); 
        u2 = -u2;
    }
    ret(0,0) = u1; ret(0,1) = u2;
    return ret;
}

void IRAM_ATTR TwoDOFLimbMP_PIDSUpdate(void *param){
    MP_PIDSclosedLoop_Counter++;
    getEulerAngles();
    LinAlg::Matrix<double> U = TwoDOFLimbMP_PIDSControl(mpref[0], mpref[1], mpclosedLoop_gyData);
    // LinAlg::Matrix<double> U = dispositivo.TwoDOFLimbControl(mpref[0], mpref[1], mpclosedLoop_gyData);
    // std::cout << "entrou1\n";
    std::stringstream ss;
    // ss << closedLoop_Counter << " , " << mpclosedLoop_gyData(0,0)  << " , " << mpclosedLoop_gyData(1,0) << " , " << mpclosedLoop_gyData(2,0) 
    ss << MP_PIDSclosedLoop_Counter << " , " << mpclosedLoop_gyData(0,0)  << " , " << mpclosedLoop_gyData(1,0) << " , " << mpclosedLoop_gyData(1,0) << " , " << U(0,0) << " , " << U(0,1) <<  " , " << 0;
    client->write(ss.str().c_str());
  std::cout << "entrou1\n";
  if(MP_PIDSclosedLoop_Counter==(int)param)
  {
    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 
    if(mprefSize == 2){
      mpref[0] = 0; mpref[1] = 0;
    }
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(MP_PIDSclosedLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(MP_PIDSclosedLoop_periodic_timer)); //Timer delete
    MP_PIDSclosedLoop_periodic_timer = nullptr;
    MP_PIDSclosedLoop_flag = false;
    openLoopFesStop();
  }
}

String MP_PIDSTwoDOFLimbFes(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
  if (doc["op"] == MP_PIDSTWODOFLIMBFES_MSG && !MP_PIDSclosedLoop_flag){
    // Serial.print("Operation 3, received data: "); Serial.println(msg);
    MP_PIDSclosedLoop_flag = true;

    // mpuInit();
    openLoopFesInit(doc["Ton"], doc["period"]);
    
    // pi_pitch->setControllerParameters(pi_pitch->setRestrictions(dados));
    // pi_roll->setControllerParameters (pi_roll->setRestrictions (dados));

    dispositivo.fes[0].setPowerLevel(0); 
    dispositivo.fes[1].setPowerLevel(0); 
    dispositivo.fes[2].setPowerLevel(0); 
    dispositivo.fes[3].setPowerLevel(0); 
    mpref = new volatile long double[2];
    mprefSize = 2;
    mpref[0] = 0; mpref[1] = 0;

    MP_PIDSclosedLoop_periodic_timer_args.callback = &TwoDOFLimbMP_PIDSUpdate;
    MP_PIDSclosedLoop_periodic_timer_args.name = "TwoDOFLimbMP_PIDSUpdate";
    MP_PIDSclosedLoop_periodic_timer_args.arg = (void*)(int (float(doc["simulationTime"])/float(doc["controlSampleTime"])));
    ESP_ERROR_CHECK(esp_timer_create(&MP_PIDSclosedLoop_periodic_timer_args, &MP_PIDSclosedLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(MP_PIDSclosedLoop_periodic_timer,  float(doc["controlSampleTime"])*1000000.0));
    MP_PIDSclosedLoop_Counter = 0;

    
    // IMUClosedLoop_periodic_timer_args.callback = &ClosedLoop_IMUDataLoop;
    // IMUClosedLoop_periodic_timer_args.name = "imuSendInit";
    // IMUClosedLoop_periodic_timer_args.arg = (void*)(int (code(0,0)/code(0,2)));
    // ESP_ERROR_CHECK(esp_timer_create(&IMUClosedLoop_periodic_timer_args, &IMUClosedLoop_periodic_timer));
    // ESP_ERROR_CHECK(esp_timer_start_periodic(IMUClosedLoop_periodic_timer, code(0,2)*1000000.0));
    MP_PIDSclosedLoopIMU_Counter = 0;
    answer += "1";
  }
  else
    answer += "";
  return answer;
}

String MP_PIDSFesReferenceUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {

  String answer;
  if (operation == MP_PIDSFESREFERENCEUPDATE_MSG){
    // Serial.print("Oeration 4, received data: "); Serial.println(msg);

    mpref[0] = doc["ref1"]; mpref[1] = doc["ref2"];
    if(!noAnswer)
      answer += "mpreferencias dos MP_PIDS atualizadas\r\n";
  }
  else
    answer += "";
  return answer;
}

String MP_PIDSParametersUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation){

  String answer;
  if (doc["op"] == MP_PIDSPARAMETERSUPDATE_MSG){
    // Serial.print("Operation 5, received data: "); Serial.println(msg);
    pi_pitch->insertRegion(doc["pid1_ind"], doc["pid1_constraints"], doc["pid1_pwa"]);
    pi_roll->insertRegion(doc["pid2_ind"], doc["pid2_constraints"], doc["pid2_pwa"]);
    serializeJson(doc, answer);
    // Serial.println(answer);
    // if(!noAnswer)
    //   answer += "Parametros dos MP_PIDS atualizados\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif