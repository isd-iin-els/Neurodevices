#ifdef ESP32DEV
#ifndef tsPCSLoopfes_h
#define tsPCSLoopfes_h

#include <sstream>
#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include <stdio.h>
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include <Adafruit_ADS1X15.h>
#include <wifiMQTT.h>
//#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
//#include "sendIMUData.h"

Adafruit_ADS1115 ads;

namespace adc1115MQTT{
    esp_timer_create_args_t DataLoop_periodic_timer_args;
    esp_timer_handle_t DataLoop_periodic_timer = nullptr;
    volatile uint64_t DataLoop_counter = 0;
    volatile bool DataLoop_flag = false, state = false;
    float Voltage = 0.0;

    static void DataLoop(void *param){
        std::stringstream ss;

        int16_t adc0;
        adc0 = ads.readADC_SingleEnded(0);
        Voltage = (adc0 * 0.1875)/1000;
        ss << Voltage << std::endl;
        mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());

        if(DataLoop_counter>=(int)param)
        {
            ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
            ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
            DataLoop_periodic_timer = nullptr;
            DataLoop_flag = false;
            Serial.println("Finalizou");
        }
    }

    String sendAdc1115Init(const StaticJsonDocument<sizejson> &doc)  {
        String answer;
        Serial.println("Tudo Inicializado");
    
        float timeSimulation = (uint16_t)doc["timeSimulation"]; //high
        float freq = (uint16_t)doc["freq"]; //high
        if (!ads.begin()) {
            Serial.println("Failed to initialize ADC!");
            return "Failed to initialize ADC!";
        }else{
            Serial.println("Inicializou o ADC");
        }
        


        DataLoop_flag = true;
        DataLoop_periodic_timer_args.callback = &DataLoop;
        DataLoop_periodic_timer_args.name = "adc1115SendInit";
        DataLoop_periodic_timer_args.arg = (void*)((int64_t)(timeSimulation*freq));
        ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, 1000000.0/freq));
        
        std::cout << "Tudo Inicializado\n";
        answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";

        return answer;
    }
}

#endif
#endif