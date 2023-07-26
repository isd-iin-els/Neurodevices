#ifndef WiFistaMQTT_h
#define WiFistaMQTT_h

#include <Arduino.h>
#include <map>
#include <ArduinoJson.h>
#include "servicesDef.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <sstream>
#include <iostream>
#include <webConfiguration.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "otaMqtt.h"



AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
const uint16_t sizejson = 512;
std::stringstream cmd2dev,devans,devstream,connectionStatus;
String lastWillTopic;


typedef String (*SDLEventFunction)(const StaticJsonDocument<sizejson> &doc);
std::map<int,SDLEventFunction> functions;
std::map<String,String> functionalitiesParameters;

//-------------------------------------------------------------------------------
void addFunctions(String functioname, String parameters, SDLEventFunction function, uint16_t operation) {
    functions[operation] = function;
    functionalitiesParameters[functioname] = parameters;
    // Serial.println(parameters);
}

void removeFunctions(String functioname) {
    // functions.erase(functioname);
    functionalitiesParameters.erase(functioname);
}

String restart(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;
  // if (operation == RESTART_MSG)
    ESP.restart();
  return answer;
}

String alive(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;
  // if (operation == ALIVE_MSG)
    return "1";
  return answer;
}

String whoAmI(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;
  // if (operation == WHOAMI_MSG){
    std::stringstream ss;
    ss << "{";
    ss <<  "\"Device\":\"DOIT Esp32 DevKit v1\",";
    ss << "\"Device Function\":\"" << devFunction.c_str() << "\",";
    ss << "\"Device ID\":" << ESP.getEfuseMac() << ","; 
    ss << "\"Device IP\":\"" << WiFi.localIP().toString().c_str() << "\","; 
    ss << "\"Topics\":[\"" << cmd2dev.str().c_str() << "\",\"" << devans.str().c_str() << "\",\"" << devstream.str().c_str()<< "\",\"" << connectionStatus.str().c_str()<< "\"],"; 
    ss << "\"Implemented Functionalities\": {" ;
    for(std::map<String,String>::iterator iter = functionalitiesParameters.begin(); iter != functionalitiesParameters.end(); ++iter)
      ss << "\"" << iter->first.c_str() << "\":" << functionalitiesParameters[iter->first.c_str()].c_str() << ",";
    ss.seekp(-1, std::ios_base::end);
    ss.seekp(-1, std::ios_base::end);
    ss << "} }\0";
    answer = ss.str().c_str();
    return answer;
  // }
  // answer = "";
  return answer;
}

#include "blinkled.h"

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
  

}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}


void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP()); 
        blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
        connectToMqtt();
        // mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
        // xTimerStart(mqttReconnectTimer, 0);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        connectToWifi();
        //xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        //xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  std::cout << "MQTT Connected\n";
  std::cout << cmd2dev.str().c_str() <<"\n";
  std::cout << devans.str().c_str() << "\n";
  std::cout << devstream.str().c_str() << "\n";
  std::cout << connectionStatus.str().c_str() << "\n";
  
  //adicionar tópico broadcast para play, pause e stop, adicionar tópico de log e erros
  mqttClient.publish("newdev", 0, true, whoAmI(StaticJsonDocument<sizejson>()).c_str());
  mqttClient.publish(connectionStatus.str().c_str(), 0, true, "Online");
  mqttClient.subscribe(cmd2dev.str().c_str(), 0);
  mqttClient.subscribe("OTAMQTT", 0);
  //blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  String text; 
  switch( reason) {
  case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
     text = "TCP_DISCONNECTED"; 

     break; 
  case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
     text = "MQTT_UNACCEPTABLE_PROTOCOL_VERSION"; 

     break; 
  case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
     text = "MQTT_IDENTIFIER_REJECTED";  

     break;
  case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE: 
     text = "MQTT_SERVER_UNAVAILABLE"; 

     break;
  case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
     text = "MQTT_MALFORMED_CREDENTIALS"; 

     break;
  case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
     text = "MQTT_NOT_AUTHORIZED"; 

     break;
  
  }
  Serial.printf(" [%8u] Disconnected from the broker reason = %s\n", millis(), text.c_str());
  Serial.printf(" [%8u] Reconnecting to MQTT..\n", millis());
  if (WiFi.isConnected()) {
          connectToMqtt();
   }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String topico = topic;
  StaticJsonDocument<sizejson> doc;
  DeserializationError error = deserializeJson(doc, payload);

  //Serial.print(F("msg\n"));

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

    int op = uint16_t(doc["op"]); 
  if(functions.count(op) != 0){
    std::cout << "Entrou aqui1\n";
    String ans = functions[op](doc);
    std::cout << "Entrou aqui2\n";
    std::cout << ans;
    if( ans != "")//{
        mqttClient.publish(devans.str().c_str(), 0, true, ans.c_str());
  }

  // if(topico == cmd2dev.str().c_str()){]
  //---------------------------------------------------------------------------------------------
  

  //   uint8_t operation = doc["op"]; 
  //     // break;
  // // }
  // for (std::map<String,SDLEventFunction>::iterator it = functions.begin() ; it != functions.end(); ++it){
  //   Serial.println(it->first);
  //   String ans = ((it->second)(doc,operation));
  //   if( ans != ""){
  //     mqttClient.publish(devans.str().c_str(), 0, true, ans.c_str());
  //     break;
  //   }
  // }
}

void onMqttPublish(uint16_t packetId) {
  // Serial.println("Publish acknowledged.");
  // Serial.print("  packetId: ");
  // Serial.println(packetId);
}

bool wifiSTAMQTTInit(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  
  if(loadEEPROM()){
      // wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

      WiFi.onEvent(WiFiEvent);
      connectToWifi();
      
      mqttClient.onConnect(onMqttConnect);
      mqttClient.onDisconnect(onMqttDisconnect);
      // mqttClient.onSubscribe(onMqttSubscribe);
      // mqttClient.onUnsubscribe(onMqttUnsubscribe);
      mqttClient.onMessage(onMqttMessage);
      // mqttClient.onPublish(onMqttPublish);
      mqttClient.setServer(MQTT_HOST.c_str(), MQTT_PORT);//teste MQTT_HOST.c_str()
      
      mqttClient.setKeepAlive(60);
      
      // mqttClient.setCredentials("neurogenicbladder","PajmuJM0imwyIxve");
      //mqttClient.setCredentials(login.c_str(),pass.c_str());
      
      resetEEPROMValuesRoutine();

      std::stringstream shortMacAddress; shortMacAddress << ESP.getEfuseMac();
      String macAddress = shortMacAddress.str().c_str();
      shortMacAddress.str(""); shortMacAddress  << macAddress[macAddress.length()-4] << macAddress[macAddress.length()-3]  
                                                << macAddress[macAddress.length()-2] << macAddress[macAddress.length()-1];
      cmd2dev << "cmd2dev" << shortMacAddress.str().c_str();
      devans <<"dev" << shortMacAddress.str().c_str() << "ans";
      devstream <<"dev" << shortMacAddress.str().c_str() << "ss";
      connectionStatus << "dev" << shortMacAddress.str().c_str() << "status";
      lastWillTopic = connectionStatus.str().c_str();
      mqttClient.setWill(lastWillTopic.c_str(),0,true,"Offline",7U);
      //Serial.println(lastWillTopic);
      return false;
  
  } else {blinkTime = 1000; blinkMe(StaticJsonDocument<sizejson>()); createWIFIAP();  return true;}
  
  // addFunctions("renameTopics",RENAMETOPICS_PARAMETERS,renameTopics);
}

#endif