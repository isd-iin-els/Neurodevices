
// #define DEVICEID uint64_t chipid = ESP.getEfuseMac();

#include <Arduino.h>
#include <AsyncTCP.h>
#include "WiFi.h"
#include <sstream>
#include "wifistaTCP.h"
#include "sendIMUData.h"
#include "openLoopFes.h"
#include "closedLoopFes.h"
#include "blinkled.h"
// #include "fesBike.h"
// #include "NBStimulator.h"
// #include "mp_closedLoopFes.h"
// #include "estimuladorBN.h"

void setup() {
  Serial.begin(115200); //initDMP6(gpio_num_t(23));
  // serverIP = IPAddress(192,168,137,101);
  wifiSTATCPInit();
  addFunctions("imuSendInit",imuSendInit);  //sensors.init();
  addFunctions("stopOpenLoopFes",stopOpenLoopFes);
  addFunctions("openLoopFesUpdate",openLoopFesUpdate);
  addFunctions("TwoDOFLimbFesControl",TwoDOFLimbFesControl);
  addFunctions("closedLoopFesReferenceUpdate",closedLoopFesReferenceUpdate); //Fazer isso para o caso geral
  addFunctions("PIDsParametersUpdate",PIDsParametersUpdate);//Fazer isso para o caso geral]
  
  // // addFunctions("fesBikeStart",fesBikeStart);
  addFunctions("whoAmI",whoAmI);
  addFunctions("blinkMe",blinkMe);
  // addFunctions("neurogenic_bladder_init",neurogenic_bladder_init);
  // addFunctions("MP_PIDSclosedLoopFesmpreferenceUpdate",MP_PIDSclosedLoopFesmpreferenceUpdate);
  // addFunctions("TwoDOFLimbMP_PIDSFes",TwoDOFLimbMP_PIDSFes);
  // addFunctions("TwoDOFLimbMP_PIDSFes",MP_PIDSParametersUpdate);

  // addFunctions("estimuladorBN",neuromoduladoBNUpdate); initialisateNeuromoduladoBNControl();
  // analogSetAttenuation((adc_attenuation_t) ADC_ATTEN_0db);
  WiFiClient client;
 
  if (!client.connect("192.168.137.1", 5000)) {

    Serial.println("Connection to host failed");

    delay(1000);
    return;
  }
  std::stringstream ss;
  ss << "{\"command\":\"registerClient\"," << "\"Device IP\":\"" << WiFi.localIP().toString().c_str()<< "\"}"; 
  Serial.println("Connected to server successful!");

  client.print(ss.str().c_str());
  delay(1000);
  Serial.println("Disconnecting...");
  client.stop();
}
// float x = 0, h = 0.01; uint8_t contador = 0;
void loop() {
  // delay(10);
  // x = x + h*(analogReadMilliVolts(36)-x);
  // if(contador==0)
  //   Serial.println(x);
  // contador++;
}


// #include <WiFi.h>
// extern "C" {
// 	#include "freertos/FreeRTOS.h"
// 	#include "freertos/timers.h"
// }
// #include <AsyncMqttClient.h>

// #define WIFI_SSID "ESP"
// #define WIFI_PASSWORD "12345678"

// #define MQTT_HOST IPAddress(15, 236, 203, 194) //broker.emqx.io
// #define MQTT_PORT 1883

// AsyncMqttClient mqttClient;
// TimerHandle_t mqttReconnectTimer;
// TimerHandle_t wifiReconnectTimer;

// void connectToWifi() {
//   Serial.println("Connecting to Wi-Fi...");
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
// }

// void connectToMqtt() {
//   Serial.println("Connecting to MQTT...");
//   mqttClient.connect();
// }

// void WiFiEvent(WiFiEvent_t event) {
//     Serial.printf("[WiFi-event] event: %d\n", event);
//     switch(event) {
//     case SYSTEM_EVENT_STA_GOT_IP:
//         Serial.println("WiFi connected");
//         Serial.println("IP address: ");
//         Serial.println(WiFi.localIP());
//         connectToMqtt();
//         break;
//     case SYSTEM_EVENT_STA_DISCONNECTED:
//         Serial.println("WiFi lost connection");
//         xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
//         xTimerStart(wifiReconnectTimer, 0);
//         break;
//     }
// }

// void onMqttConnect(bool sessionPresent) {
//   Serial.println("Connected to MQTT.");
//   Serial.print("Session present: ");
//   Serial.println(sessionPresent);
//   uint16_t packetIdSub = mqttClient.subscribe("andre.dantas@isd.org.br/test/lol", 2);
//   Serial.print("Subscribing at QoS 2, packetId: ");
//   Serial.println(packetIdSub);
//   mqttClient.publish("andre.dantas@isd.org.br/test/lol", 0, true, "test 1");
//   Serial.println("Publishing at QoS 0");
//   uint16_t packetIdPub1 = mqttClient.publish("andre.dantas@isd.org.br/test/lol", 1, true, "test 2");
//   Serial.print("Publishing at QoS 1, packetId: ");
//   Serial.println(packetIdPub1);
//   uint16_t packetIdPub2 = mqttClient.publish("andre.dantas@isd.org.br/test/lol", 2, true, "test 3");
//   Serial.print("Publishing at QoS 2, packetId: ");
//   Serial.println(packetIdPub2);
// }

// void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//   Serial.println("Disconnected from MQTT.");

//   if (WiFi.isConnected()) {
//     xTimerStart(mqttReconnectTimer, 0);
//   }
// }

// void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//   Serial.println("Subscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
//   Serial.print("  qos: ");
//   Serial.println(qos);
// }

// void onMqttUnsubscribe(uint16_t packetId) {
//   Serial.println("Unsubscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
// }

// void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
//   Serial.println("Publish received.");
//   Serial.print("  topic: ");
//   Serial.println(topic);
//   Serial.print("  qos: ");
//   Serial.println(properties.qos);
//   Serial.print("  dup: ");
//   Serial.println(properties.dup);
//   Serial.print("  retain: ");
//   Serial.println(properties.retain);
//   Serial.print("  len: ");
//   Serial.println(len);
//   Serial.print("  index: ");
//   Serial.println(index);
//   Serial.print("  total: ");
//   Serial.println(total);
// }

// void onMqttPublish(uint16_t packetId) {
//   Serial.println("Publish acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
// }

// void setup() {
//   Serial.begin(115200);
//   Serial.println();
//   Serial.println();

//   mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
//   wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

//   WiFi.onEvent(WiFiEvent);

//   mqttClient.onConnect(onMqttConnect);
//   mqttClient.onDisconnect(onMqttDisconnect);
//   mqttClient.onSubscribe(onMqttSubscribe);
//   mqttClient.onUnsubscribe(onMqttUnsubscribe);
//   mqttClient.onMessage(onMqttMessage);
//   mqttClient.onPublish(onMqttPublish);
//   mqttClient.setServer("maqiatto.com", MQTT_PORT);
//   mqttClient.setCredentials("andre.dantas@isd.org.br", "12345678");

//   connectToWifi();
// }

// void loop() {
// }
