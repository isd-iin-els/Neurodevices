#ifndef WiFistaTCP_h
#define WiFistaTCP_h

#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include <map>
#include <ArduinoJson.h>
#include "servicesDef.h"


bool noAnswer = true;

const char *ssid = "ESP";
const char *password = "12345678";
const uint16_t sizejson = 512;
AsyncClient *client;
AsyncServer server(4000);

// IPAddress serverIP(192,168,137,100);
// IPAddress gateway(192,168,137,1);
// IPAddress subnet(255,255,255,0);

typedef String (*SDLEventFunction)(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation);
std::map<String,SDLEventFunction> functions;

void addFunctions(String functioname, SDLEventFunction function) {
    functions[functioname] = function;
}

void removeFunctions(String functioname) {
    functions.erase(functioname);
}

void onData(void* arg, AsyncClient* c, void* data, size_t len) {
  StaticJsonDocument<sizejson> doc;
  DeserializationError error = deserializeJson(doc, data);
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  uint8_t operation = doc["op"]; 
  for (std::map<String,SDLEventFunction>::iterator it = functions.begin() ; it != functions.end(); ++it)
      c->write(((it->second)(doc,operation)).c_str());
  
}

void onConnect(void* arg, AsyncClient* c) {
  Serial.print("We're connected!\n");
}

void onDisconnect(void* arg, AsyncClient* c) {
  Serial.print("We're disconnected!\n");
}

void onConnectServer(void* arg, AsyncClient* c){
  Serial.print("We're connected!\n");
  client = c;
  //c->write("hello from server");
  client->onConnect(onConnect);
  client->onDisconnect(onDisconnect);
  client->onData(onData);
}

String restart(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  String answer;
  if (operation == RESTART_MSG)
    ESP.restart();
  return answer;
}

String whoAmI(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  // char* d = reinterpret_cast<char*>(data); String msg,answer;
  // for (size_t i = 0; i < len; ++i) msg += d[i];
  // uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  // msg = msg.substring(index+1,msg.length());
  String answer;
  if (operation == WHOAMI_MSG){
    std::stringstream ss;
    ss << "{";
    ss <<  "\"Device\":\"DOIT Esp32 DevKit v1\",";
    ss << "\"Device Attribute\":\"4-Channel FES\",";
    ss << "\"Device ID\":" << ESP.getEfuseMac() << ","; 
    ss << "\"Device IP\":\"" << WiFi.localIP().toString().c_str() << "\","; 
    ss << "\"Implemented Functionalities\": [" ;
    for(std::map<String,SDLEventFunction>::iterator iter = functions.begin(); iter != functions.end(); ++iter)
      ss << "\"" << iter->first.c_str() << "\",";
    ss.seekp(-1, std::ios_base::end);
    ss.seekp(-1, std::ios_base::end);
    ss << "] }\0";
    answer = ss.str().c_str();
    return answer;
  }
  answer = "";
  return answer;
}

void wifiSTATCPInit(){
    WiFi.mode(WIFI_STA);
    // WiFi.config(serverIP,gateway,subnet);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        delay(1000);
        Serial.println("Trying to Connect");
        WiFi.reconnect();
    }
    Serial.println("Wifi connected!");
    Serial.println("Openning the server port: 4000");
    server.onClient(onConnectServer, NULL);
    server.begin();
    addFunctions("restart",restart);
}

#endif