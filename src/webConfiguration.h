#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>

DNSServer dnsServer;
AsyncWebServer server(80);

EEPROMClass mqttLogin ("login");
EEPROMClass mqttPass  ("pass");
EEPROMClass mqttHost  ("host");
EEPROMClass mqttPort  ("port");
EEPROMClass wifiSSID  ("ssid");
EEPROMClass wifiPassword  ("wifipass");
String login = "",pass = "",MQTT_HOST = "", WIFI_SSID = "", WIFI_PASSWORD = "";
uint16_t MQTT_PORT = 0;


bool loadEEPROM(){
  if (mqttLogin.begin(0x50) && mqttPass.begin(0x50)  && 
      mqttHost.begin(0x50)  && mqttPort.begin(0x10)  && 
      wifiSSID.begin(0x50)  && wifiPassword.begin(0x50)) {
        login = mqttLogin.readString(0);
        pass = mqttPass.readString(0);
        MQTT_HOST = mqttHost.readString(0);
        MQTT_PORT = mqttPort.readUInt(0);
        WIFI_SSID = wifiSSID.readString(0);
        WIFI_PASSWORD =  wifiPassword.readString(0);
        std::cout << login.c_str()         << "\n" << pass.c_str() <<"\n"
                  << MQTT_HOST.c_str()     << "\n" << MQTT_PORT    <<"\n"
                  << WIFI_SSID.c_str()     << "\n" << WIFI_PASSWORD.c_str() <<"\n";
        if (MQTT_HOST==""||WIFI_SSID==""||WIFI_PASSWORD=="")
          return false;
        return true;
    }
    else{
        return false;
    }
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

  <head>
    <title>ESP Input Form</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html,

      body {
        /* background:blue; */
        text-align: center;
      }
      form {
        margin-top: 200px;
        margin-left: 20%;
        margin-right: 20%;
      }
      label {
        color: black;
        font-weight: bold;
        float:left;
      }
      input {
        width: 100%;
        height: 30px;
        margin: 5px;
      }
      input[type=submit] {
        color: white;
        background-color: blue;
        border: 2px solid blue;
      }
      p {
        color:blue;
      }
    </style>
  </head>
  <body>
    <form action="/get">
      <label>WiFi SSID :</label>
      <input type="text" name="wifiSSID">
      <br>
      <label>WiFi Password:</label>
      <input type="text" name="wifiPassword">
      <br>
      <label>MQTT Server Login:</label>
      <input type="text" name="mqttLogin">
      <br>
      <label>MQTT Server Password:</label>
      <input type="text" name="mqttPass">
      <br>
      <label>MQTT Server Host Name:</label>
      <input type="text" name="mqttHost">
      <br>
      <label>MQTT Server port:</label>
      <input type="text" name="mqttPort">
      <br>
      <input type="submit" value="Submit">
    </form><br>
  </body>
</html>)rawliteral";



boolean captivePortal(AsyncWebServerRequest *request) {
  // !isIp(server.hostHeader())
  if (true) {
    Serial.println("Request redirected to captive portal");
    AsyncWebServerResponse *response = request->beginResponse(302, "text/html", index_html);
    response->addHeader("Location", String("http://") + "8.8.8.8");
    request->send(response);
    //server.send(302, "text/plain", "");   
    //server.client().stop(); 
    return true;
  }
  return false;
}

void handleRoot(AsyncWebServerRequest *request) {
  // if (captivePortal(request)) { 
  //   return;
  // }
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send(response);
}

void initWebServer(){
    server.on("/", handleRoot);
    server.on("/generate_204", handleRoot);
    server.onNotFound([](AsyncWebServerRequest *request){
      request->send(404);
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        
        //request->send_P(200, "text/html", index_html);
    });
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam("mqttLogin") && request->hasParam("mqttPass")&&request->hasParam("mqttHost") && request->hasParam("mqttPort")) {
      login = request->getParam("mqttLogin")->value();
      pass = request->getParam("mqttPass")->value();
      MQTT_HOST = request->getParam("mqttHost")->value();
      MQTT_PORT = request->getParam("mqttPort")->value().toInt();
      WIFI_SSID = request->getParam("wifiSSID")->value();
      WIFI_PASSWORD =  request->getParam("wifiPassword")->value();
    //   writeString(login,mqttLogin);
      if(login.length()>0){
        mqttLogin.writeString(0,login.c_str());
        mqttLogin.commit();
      }
      if(pass.length()>0){
        mqttPass.writeString(0,pass.c_str());
        mqttPass.commit();
      }
      if(MQTT_HOST.length()>0){
        mqttHost.writeString(0,MQTT_HOST.c_str());
        mqttHost.commit();
      }
      if(MQTT_PORT>0){
        mqttPort.writeUInt(0,MQTT_PORT);
        mqttPort.commit();
      }
      if(WIFI_SSID.length()>0){
        wifiSSID.writeString(0,WIFI_SSID.c_str());
        wifiSSID.commit();
      }
      if(WIFI_PASSWORD.length()>0){
        wifiPassword.writeString(0,WIFI_PASSWORD.c_str());
        wifiPassword.commit();
      }

      
      std::cout << login.c_str()<<"\n"<<pass.c_str()<<"\n"<<MQTT_HOST.c_str()<<"\n"<<MQTT_PORT<<"\n";
      request->send(200, "text/plain", "Data Updated");
      delay(1000);
      ESP.restart();
    }
    else {
      request->send(200, "text/plain", "Data Not Updated");
    }
    
    
  });
//   std::cout << "Aqui5\n";
  server.begin();
//   std::cout << "Aqui6\n";
}

#define INTERRUPT_PIN 0
//portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; 
bool counter_reinit = false;
uint64_t millisRestart = 0;
void IRAM_ATTR resetEEPROMValues(){
  //portENTER_CRITICAL_ISR(&mux);  
  counter_reinit = digitalRead(INTERRUPT_PIN);
  uint64_t mill = millis();
  if (!counter_reinit)
  {
    millisRestart = mill;
    //counter_reinit = true;
    //Serial.print("Entrou... Milis = ");
    //Serial.println(millisRestart);
  }
  else if(counter_reinit && mill-millisRestart > 6000){
    wifiSSID.writeString(0,"");
    wifiSSID.commit();
    //Serial.println("Reiniciando para alterar configurações de rede!\n");
    ESP.restart();
  }
  // else if(counter_reinit){
  //   //counter_reinit = false;
  //   //Serial.print("Nao reiniciou... Milis = ");
  //   Serial.println(mill-millisRestart);
  // }
  //portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR zeroResetFlag(){
  counter_reinit = 0;
}

void resetEEPROMValuesRoutine(){
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(INTERRUPT_PIN, resetEEPROMValues, CHANGE);
}


void createWIFIAP(){
  WiFi.softAPConfig(IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
  Serial.println(WiFi.softAP("BNConfig", "12345678") ? "Ready" : "Failed!");
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError); 
  dnsServer.start(53, "*", IPAddress(8,8,8,8));
  delay(2000);
  initWebServer();
}
