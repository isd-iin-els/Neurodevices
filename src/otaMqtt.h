#include <Arduino.h>
#include <map>
#include <sstream>
#include <iostream>
#include <ArduinoJson.h>
#include <AsyncMqttClient.h>
#include <Update.h>
#include "servicesDef.h"
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <thread>
#include "asyncOTA.h"

namespace OTAMQTT{
    // int contentLength = 0;
    // bool isValidContentType = false;
    
    // String getHeaderValue(String header, String headerName) {
    //     return header.substring(strlen(headerName.c_str()));
    // }

    // String getBinName(String url) {
    //     int index = 0;

    //     // Search for last /
    //     for (int i = 0; i < url.length(); i++) {
    //         if (url[i] == '/') {
    //             index = i;
    //         }
    //     }

    //     String binName = "";

    //     // Create binName
    //     for (int i = index; i < url.length(); i++) {
    //         binName += url[i];
    //     }

    //     return binName;
    // }

    // String getHostName(String url) {
    //     int index = 0;

    //     // Search for last /
    //     for (int i = 0; i < url.length(); i++) {
    //         if (url[i] == '/') {
    //             index = i;
    //         }
    //     }

    //     String hostName = "";

    //     // Create binName
    //     for (int i = 0; i < index; i++) {
    //         hostName += url[i];
    //     }

    //     return hostName;
    // }

    void update(String binFolder, String url, String md5) {
        // String bin = getBinName(url);
        // String host = getHostName(url);

            ESBOTA ota;
            url = "http://"+url+binFolder+"|" + md5;
            Serial.println(url);
            Serial.println(url.length());
            ota.begin(strdup(url.c_str()),url.length());
    }

    

    String updateFirmware(const StaticJsonDocument<512> &doc, const uint8_t &operation) {
        String answer = "";
        if (operation == UPDATE_FIRMWARE_MSG){
            String binFolder = doc["binLocation"];
            String url = doc["url"];
            String md5 = doc["md5"];
            
            Serial.println("Requesting Update!");
            //Serial.println();
            update(binFolder,url,md5);
        }
        return answer; 
    }
}