#include <iostream>
#include <time.h>
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "SistemasdeControle/embeddedTools/communicationLibs/wifi/wifista.h"
#include "I2Cdev.h"
#include "SistemasdeControle/embeddedTools/sensors/sensorfusion.h"
#include "SistemasdeControle/embeddedTools/sensors/kalmanfilter.h"

uint8_t modPin[4] = {27,4,12,5},
        levelPin[4]   = {13,19,2,18};
Devices::fes4channels dispositivo(levelPin, modPin, 4, 18000); double ref1, ref2;
// ControlHandler::PID<long double> pid[2]; double ref1, ref2;
esp_timer_create_args_t periodic_timer_args;

bool flag = false, flag2 = true;
uint8_t modoFuncionamento;
double fesCycling = 2, fesCyclingMax = 0; bool fesCyclingBool = false;

LinAlg::Matrix<double> referenceTracking; bool referenceTrackingFlag = false; uint32_t controStep = 0;

Communication::WifiSTA* wifi = Communication::WifiSTA::GetInstance(); uint64_t wificounter = 0; 
std::stringstream wifidata;

GY80::sensorfusion sensors;
LinAlg::Matrix<double> gyData(3,1);

esp_timer_handle_t periodic_timer;
int sign(double value) {  if (value > 0) return 1;  else return -1; }
static void mpuRead(void *para);

#define GYRO_FULLSCALE			 (250)
#if   GYRO_FULLSCALE == 250
    #define GyroAxis_Sensitive (float)(131.0)
#endif

static void wifiCallback(void){ 
    if (!flag)
    {
        flag = true;
        periodic_timer_args.callback = &mpuRead;
        periodic_timer_args.name = "ccdfkgdjf";
        ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 100000));
    }  

    LinAlg::Matrix<double> code = wifi->getData();
	std::cout << code;
    if(code(0,0) == 0){
        //  for(uint8_t i = 1; i < 5; ++i)
            dispositivo.fes[0].setPowerLevel(code(0,1)); 
            dispositivo.fes[1].setPowerLevel(code(0,2)); 
            dispositivo.fes[2].setPowerLevel(code(0,3)); 
            dispositivo.fes[3].setPowerLevel(code(0,4));
            modoFuncionamento = 0;
    } else if(code(0,0) == 1){
        ref1 = code(0,1);
        ref2 = code(0,2);
    } else if(code(0,0) == 2)
    {
        dispositivo.getPID(0).setParams(code(0,3),code(0,4),code(0,5));  dispositivo.getPID(0).setLimits(code(0,2),code(0,1)); dispositivo.getPID(0).setInputOperationalPoint(code(0,11));
        dispositivo.getPID(1).setParams(code(0,8),code(0,9),code(0,10)); dispositivo.getPID(1).setLimits(code(0,7),code(0,6)); dispositivo.getPID(1).setInputOperationalPoint(code(0,12));
        modoFuncionamento = 1;
    }else if(code(0,0) == 3){
        modoFuncionamento = 3;
        fesCyclingMax = code(0,1);
    }else if(code(0,0) == 4){
        if(!referenceTrackingFlag){
            referenceTracking = code(from(1)-->3,uint32_t(0));
            referenceTrackingFlag = true;
        } else{
            referenceTracking = referenceTracking||code(from(1)-->3,uint32_t(0));
        }
        modoFuncionamento = 4;
    }else if(code(0,0) == 5){
        modoFuncionamento = 5;
    }

    if(modoFuncionamento != 4)
        referenceTrackingFlag = false;
   
    //wifi1 << str.str().c_str();
}

void mpuRead(void *para){
   
    if(modoFuncionamento == 0){
        gyData += 0.05*(sensors.update()-gyData);
        std::cout << wificounter << " , " << gyData(0,0)  << " , " << gyData(1,0) << " , " << gyData(2,0) << std::endl;

        if (flag2){
            dispositivo.getPID(0).resetIntegralValue();
            dispositivo.getPID(1).resetIntegralValue();
            flag2 = false;
        }
    }
    
    if(modoFuncionamento == 1){
        flag2 = true;
        // double u1, u2;
        gyData += 0.05*(sensors.update()-gyData);
        LinAlg::Matrix<double> U = dispositivo.performOneControlStep(ref1, ref2, gyData);
        // u1 = pid[0].OutputControl(ref1, gyData(0,0));
        // if (u1 > 0){
        //     u1 += pid[0].getInputOperationalPoint();
        //     dispositivo.fes[0].setPowerLevel(u1); 
        //     dispositivo.fes[1].setPowerLevel(0); 
        // }
        // else{
        //     u1 = -u1 + pid[0].getInputOperationalPoint();
        //     dispositivo.fes[1].setPowerLevel(u1); 
        //     dispositivo.fes[0].setPowerLevel(0); 
        //     u1 = -u1;
        // }

        // u2 = pid[1].OutputControl(ref2, gyData(1,0));
        // if (u2 > 0){
        //     u2 += pid[1].getInputOperationalPoint();
        //     dispositivo.fes[2].setPowerLevel(u2); 
        //     dispositivo.fes[3].setPowerLevel(0); 
        // }
        // else{
        //     u2 = -u2 + pid[1].getInputOperationalPoint();
        //     dispositivo.fes[3].setPowerLevel(u2);
        //     dispositivo.fes[2].setPowerLevel(0); 
        //     u2 = -u2;
        // }

        wifidata << wificounter << " , " << gyData(0,0)  << " , " << gyData(1,0) << " , " << gyData(2,0) << " , " << U(0,0) << " , " << U(0,1);
        std::cout << wificounter << " , " << gyData(0,0)  << " , " << gyData(1,0) << " , " << gyData(2,0) << " , " << U(0,0) << " , " << U(0,1) << std::endl;
        if((wificounter++)%5 == 0){
            wifi->write(wifidata.str().c_str());
        //     // std::cout << wifidata.str().c_str();
            wifidata.str("");
        } else {  wifidata << ";"; std::cout <<  std::endl;}
        
    }else if(modoFuncionamento == 3){
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
        // std::cout << (int)fesCycling << std::endl;
    }else if(modoFuncionamento == 4){
        gyData += 0.05*(sensors.update()-gyData);
        std::cout << wificounter << " , " << gyData(0,0)  << " , " << gyData(1,0) << " , " << gyData(2,0) << "," << 0< < "," << 0;
        wifidata << wificounter << " , " << gyData(0,0)  << " , " << gyData(1,0) << " , " << gyData(2,0) << "," << 0< < "," << 0;
        if((wificounter++)%5 == 0){
            wifi->write(wifidata.str().c_str());
            wifidata.str("");
        } else {  wifidata << ";"; std::cout <<  std::endl;}
    }else if(modoFuncionamento == 5){
        if(controStep < referenceTracking.getNumberOfRows()){
            gyData += 0.05*(sensors.update()-gyData);
            dispositivo.performOneControlStep(referenceTracking(0,0), referenceTracking(0,1), gyData);
            controStep++;
        }else{controStep = 0;}
    }
}



void setup(){ 
    Serial.begin(115200);
    dispositivo.startLoop();  

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);

    
    dispositivo.getPID(0).setParams(1,0.1,0); dispositivo.getPID(0).setSampleTime(1); dispositivo.getPID(0).setLimits(1.1,1.5);
    dispositivo.getPID(1).setParams(1,0.1,0); dispositivo.getPID(1).setSampleTime(1); dispositivo.getPID(1).setLimits(1.1,1.8);
    sensors.init();
    std::cout << "Entrou1" << std::endl;
    wifi->connect(wifiCallback);
    std::cout << "Entrou6" << std::endl;
    wifi->initializeComunication(); 
  }

void loop(){
    
}