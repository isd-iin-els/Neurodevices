#ifndef WiFistaTCP_h
#define WiFistaTCP_h

#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include <map>

bool noAnswer = true;

const char *ssid = "ESP";
const char *password = "12345678";

AsyncClient *client;
AsyncServer server(4000);

// IPAddress serverIP(192,168,137,100);
// IPAddress gateway(192,168,137,1);
// IPAddress subnet(255,255,255,0);

typedef String (*SDLEventFunction)(void*,size_t);
std::map<String,SDLEventFunction> functions;

void addFunctions(String functioname, SDLEventFunction function) {
    functions[functioname] = function;
}

void removeFunctions(String functioname) {
    functions.erase(functioname);
}

void onData(void* arg, AsyncClient* c, void* data, size_t len) {
  for (std::map<String,SDLEventFunction>::iterator it = functions.begin() ; it != functions.end(); ++it)
    c->write(((it->second)(data,len)).c_str());
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

String restart(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  if (op.toInt() == 7)
    ESP.restart();
  return answer;
}

String whoAmI(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  if (op.toInt() == 9){
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

volatile uint64_t blink_Counter; volatile bool blink_flag = false;
esp_timer_create_args_t blink_periodic_timer_args;
esp_timer_handle_t blink_periodic_timer = nullptr;
volatile bool level = false;
void IRAM_ATTR blink(void *param){
  blink_Counter++;
  gpio_set_level(gpio_num_t (2), level);
  level = !level;
  if(blink_Counter==(int)param)
  {
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(blink_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(blink_periodic_timer)); //Timer delete
    blink_periodic_timer = nullptr;
    blink_flag = false;
    gpio_set_level(gpio_num_t (2), false);
  }
}

String blinkMe(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  if (op.toInt() == 10){
    gpio_pad_select_gpio((gpio_num_t)2);
    gpio_set_direction((gpio_num_t)2, GPIO_MODE_OUTPUT); 
    blink_periodic_timer_args.callback = &blink;
    blink_periodic_timer_args.name = "blink";
    blink_periodic_timer_args.arg = (void*)(int (20));
    ESP_ERROR_CHECK(esp_timer_create(&blink_periodic_timer_args, &blink_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(blink_periodic_timer, 500000.0));
    blink_Counter = 0;
    std::stringstream ss;
    ss << "start blinking\n";
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