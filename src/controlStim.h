#ifdef ESP32DEV//Não mexe
#ifndef controlStim_h //Mudar esse nome para o nome_do_arquivo_h
#define controlStim_h //Mudar esse nome para o nome_do_arquivo_h

#include <sstream>//Não mexe
#include <Arduino.h>//Não mexe
#include <AsyncTCP.h>//Não mexe
#include "WiFi.h"//Não mexe
#include <stdio.h>//Não mexe
#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"//Não mexe
#include <wifiMQTT.h>//Não mexe
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#define setStim_PARAMETERS "{\"op\":41,\"on_off\":\"bool\"}"
namespace ControlStim{
    unsigned long  on_off = 0;

    String setStim(const StaticJsonDocument<sizejson> &doc)  {
        on_off = (bool)doc["on_off"]; 
        if(on_off){
            digitalWrite(26, HIGH);
            digitalWrite(27, HIGH);
            dispositivo.fes[2].setPowerLevel(200);
            
        }else{
            digitalWrite(26, LOW);
            digitalWrite(27, LOW);
            dispositivo.fes[2].setPowerLevel(0);
        }
        return "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
    }
}
#endif
#endif