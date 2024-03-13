#ifndef wifiAMQTTC3_h
#define wifiAMQTTC3_h

#include "Arduino.h"
#include "WiFi.h"
#include <webConfiguration.h>
#include "Arduino.h"
#include <servicesDef.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include "iostream"
#include "sstream"
#include <WiFi.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include <map>
// #include "blinkled.h"

class MqttClient {
	public:
		esp_mqtt_client_handle_t mqtt_client;
		uint16_t subscribe(const char* topic, uint8_t qos){
			return esp_mqtt_client_subscribe (mqtt_client, topic, qos);
		}
		uint16_t publish(const char* topic, uint8_t qos, bool retain, String payload, size_t length = 0, bool dup = false, uint16_t message_id = 0){
			return esp_mqtt_client_publish (mqtt_client, topic, payload.c_str(), payload.length(), qos, false);
		}
};

esp_mqtt_client_config_t mqtt_cfg;
MqttClient mqttClient;

// const char* WIFI_SSID = "CAMPUS";
// const char* WIFI_PASSWD = "IINELS_educacional";
// const char* MQTT_HOST = "10.1.0.18";
// const uint32_t MQTT_PORT = 1883;
const uint16_t sizejson = 512;
std::stringstream cmd2dev,devans,devstream,connectionStatus;
String lastWillTopic;

void WiFiEvent(WiFiEvent_t event);

static esp_err_t mqtt_event_handler (esp_mqtt_event_handle_t event);
void onMqttMessage(char* topic, char* payload);
void onMqttConnect();

typedef String (*SDLEventFunction)(const StaticJsonDocument<512> &doc);
std::map<int,SDLEventFunction> functions;
std::map<String,String> functionalitiesParameters;

void addFunctions(String functioname, String parameters, SDLEventFunction function, uint16_t operation) {
    functions[operation] = function;
    functionalitiesParameters[functioname] = parameters;
}

void removeFunctions(String functioname) {
    functionalitiesParameters.erase(functioname);
}

String restart(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
	String answer;
	ESP.restart();
	return answer;
}

String whoAmI(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;

    std::stringstream ss, functionalities;
    ss << "{";
    ss <<  "\"Device\":\"DOIT Esp32 DevKit v1\",";
    ss << "\"Device Function\":\"" << devFunction.c_str() << "\",";
    ss << "\"Device ID\":" << ESP.getEfuseMac() << ","; 
    ss << "\"Device IP\":\"" << WiFi.localIP().toString().c_str() << "\","; 
    ss << "\"Topics\":[\"" << cmd2dev.str().c_str() << "\",\"" << devans.str().c_str() << "\",\"" << devstream.str().c_str()<< "\",\"" << connectionStatus.str().c_str()<< "\"],"; 
    ss << "\"Implemented Functionalities\":";
    // functionalities << globaljson.as<String>().c_str();
    // ss << functionalities.str() << " }\0";
	functionalities << " {" ;
    for(std::map<String,String>::iterator iter = functionalitiesParameters.begin(); iter != functionalitiesParameters.end(); ++iter)
        functionalities << "\"" << iter->first.c_str() << "\":" << functionalitiesParameters[iter->first.c_str()].c_str() << ",";
    functionalities.seekp(-1, std::ios_base::end);
    functionalities.seekp(-1, std::ios_base::end);
    functionalities << "}\0";
	ss << functionalities.str() << " }\0";

    answer = ss.str().c_str();
    return answer;
}

void connectToWifi() {
	Serial.println("Connecting to Wi-Fi...");
	WiFi.onEvent(WiFiEvent);
	WiFi.mode (WIFI_MODE_STA);
	WiFi.setSleep(false);
	WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
	while (!WiFi.isConnected ()) {
		Serial.print ('.');
		delay (100);
	}
}

void createTopics(){
	std::stringstream shortMacAddress; shortMacAddress << ESP.getEfuseMac();
	String macAddress = shortMacAddress.str().c_str();
	shortMacAddress.str(""); shortMacAddress  << macAddress[macAddress.length()-4] << macAddress[macAddress.length()-3]  
											<< macAddress[macAddress.length()-2] << macAddress[macAddress.length()-1];
	cmd2dev << "cmd2dev" << shortMacAddress.str().c_str();
	devans <<"dev" << shortMacAddress.str().c_str() << "ans";
	devstream <<"dev" << shortMacAddress.str().c_str() << "ss";
	connectionStatus << "dev" << shortMacAddress.str().c_str() << "status";
	lastWillTopic = connectionStatus.str().c_str();
}

void connectToMqtt() {
	mqtt_cfg.host = MQTT_HOST.c_str();
	mqtt_cfg.port = MQTT_PORT;
	mqtt_cfg.keepalive = 15;

	mqtt_cfg.transport = MQTT_TRANSPORT_OVER_TCP;

	mqtt_cfg.event_handle = mqtt_event_handler;
	mqtt_cfg.lwt_topic = lastWillTopic.c_str();
	mqtt_cfg.lwt_msg = "Offline";
	mqtt_cfg.lwt_msg_len = 7;


	Serial.println("Connecting to MQTT...");
	mqttClient.mqtt_client = esp_mqtt_client_init (&mqtt_cfg);
	esp_err_t err = esp_mqtt_client_start (mqttClient.mqtt_client);
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:
  
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP()); 
			
        // blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
            connectToMqtt();
        break;


        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("WiFi lost connection");
            connectToWifi();
        break;
    }
}

static esp_err_t mqtt_event_handler (esp_mqtt_event_handle_t event) {

	if (event->event_id == MQTT_EVENT_CONNECTED) {
    	Serial.println("Evento Conectado");
		onMqttConnect();
	} 
	else if (event->event_id == MQTT_EVENT_DISCONNECTED) {
    	Serial.println("Evento Desconectado");
	} else  if (event->event_id == MQTT_EVENT_SUBSCRIBED) {
    	Serial.println("Evento Inscrito");
	} else  if (event->event_id == MQTT_EVENT_UNSUBSCRIBED) {
    	Serial.println("Evento Desinscrito");
	} else  if (event->event_id == MQTT_EVENT_PUBLISHED) {
    	Serial.println("Evento publicou");
	} else  if (event->event_id == MQTT_EVENT_DATA) {
		Serial.println("Evento Recebeu Dado");
		onMqttMessage(event->topic, event->data);
	} else  if (event->event_id == MQTT_EVENT_BEFORE_CONNECT) {
    	Serial.println("Antes de conectar");
	}
	return ESP_OK;
}

void onMqttConnect() {
	Serial.println("MQTT Connected");
	Serial.println(cmd2dev.str().c_str());
	Serial.println(devans.str().c_str());
	Serial.println(devstream.str().c_str());
	Serial.println(connectionStatus.str().c_str());
  
	mqttClient.publish("newdev", 1, false, whoAmI(StaticJsonDocument<sizejson>()).c_str());
	mqttClient.publish(connectionStatus.str().c_str(), 0, true, "Online");
	mqttClient.subscribe(cmd2dev.str().c_str(), 0);
}

void onMqttMessage(char* topic, char* payload){
	String topico = topic;
	StaticJsonDocument<sizejson> doc;
	DeserializationError error = deserializeJson(doc, payload);

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
}

bool wifiSTAMQTTInit(){ 
	Serial.begin (115200);
	// delay(1000); return false;
	if(loadEEPROM()){createTopics();connectToWifi();resetEEPROMValuesRoutine();return false;
	}else{createWIFIAP();  return true;}
}








// #include "Arduino.h"
// #include <AsyncMQTT_ESP32.h>
// // #include "iostream"
// // #include "sstream"
// #include <ESPAsyncWebServer.h>
// // #include <globals.h>
// // #include <webConfiguration.h>
// String WIFI_SSID = "CAMPUS",WIFI_PASSWORD = "IINELS_educacional";
// String MQTT_HOST = "10.1.0.18";
// int MQTT_PORT = 1883;

// // // extern "C" {
// // // 	#include "freertos/FreeRTOS.h"
// // // 	#include "freertos/timers.h"
// // // }

// // // #include "otaMqtt.h"



// AsyncMqttClient mqttClient;
// // TimerHandle_t mqttReconnectTimer;
// // TimerHandle_t wifiReconnectTimer;
// // const uint16_t sizejson = 512;
// // std::stringstream cmd2dev,devans,devstream,connectionStatus;
// // String lastWillTopic;


// // //-------------------------------------------------------------------------------
// // void addFunctions(String functioname, String parameters, SDLEventFunction function, uint16_t operation) {
// //     functions[operation] = function;
// //     functionalitiesParameters[functioname] = parameters;
// //     // Serial.println(parameters);
// // }

// // void removeFunctions(String functioname) {
// //     // functions.erase(functioname);
// //     functionalitiesParameters.erase(functioname);
// // }

// // String restart(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
// //   String answer;
// //   // if (operation == RESTART_MSG)
// //     ESP.restart();
// //   return answer;
// // }

// // String resetDevice(const StaticJsonDocument<sizejson> &doc)  {
// //   String answer;
// //   // if (operation == ALIVE_MSG)
// //   wifiSSID.writeString(0,"");
// //   wifiSSID.commit();
// //   //Serial.println("Reiniciando para alterar configurações de rede!\n");
// //   ESP.restart();
// //   return "1";

// // }

// // String alive(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
// //   String answer;
// //   // if (operation == ALIVE_MSG)
// //     return "1";
// // }

// // String whoAmI(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
// //   String answer;
// //   // if (operation == WHOAMI_MSG){
// //     std::stringstream ss, functionalities;
// //     ss << "{";
// //     ss <<  "\"Device\":\"DOIT Esp32 DevKit v1\",";
// //     ss << "\"Device Function\":\"" << devFunction.c_str() << "\",";
// //     ss << "\"Device ID\":" << ESP.getEfuseMac() << ","; 
// //     ss << "\"Device IP\":\"" << WiFi.localIP().toString().c_str() << "\","; 
// //     ss << "\"Topics\":[\"" << cmd2dev.str().c_str() << "\",\"" << devans.str().c_str() << "\",\"" << devstream.str().c_str()<< "\",\"" << connectionStatus.str().c_str()<< "\"],"; 
// //     ss << "\"Implemented Functionalities\":";
// //     functionalities << globaljson.as<String>().c_str();
// //     ss << functionalities.str() << " }\0";

// //     answer = ss.str().c_str();
// //     return answer;
// //   // }
// //   // answer = "";
// //   return answer;
// // }

// // #include "blinkled.h"

// void connectToWifi() {
//   Serial.println("Connecting to Wi-Fi...");
//   WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
//   return;
// }

// void connectToMqtt() {
//   Serial.println("Connecting to MQTT...");
//   mqttClient.connect();
// }


// void WiFiEvent(WiFiEvent_t event) {
//     Serial.printf("[WiFi-event] event: %d\n", event);
//     switch(event) {
//         case SYSTEM_EVENT_STA_GOT_IP:
  
//             Serial.println("WiFi connected");
//             Serial.println("IP address: ");
//             Serial.println(WiFi.localIP()); 
//         // blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
//             connectToMqtt();
//         break;


//         case SYSTEM_EVENT_STA_DISCONNECTED:
//             Serial.println("WiFi lost connection");
//             connectToWifi();
//         break;
//     }
// }

// void onMqttConnect(bool sessionPresent) {
// //   std::cout << "MQTT Connected\n";
// //   std::cout << cmd2dev.str().c_str() <<"\n";
// //   std::cout << devans.str().c_str() << "\n";
// //   std::cout << devstream.str().c_str() << "\n";
// //   std::cout << connectionStatus.str().c_str() << "\n";
  
// //   //adicionar tópico broadcast para play, pause e stop, adicionar tópico de log e erros
// //   mqttClient.publish("newdev", 0, true, whoAmI(StaticJsonDocument<sizejson>()).c_str());
// //   mqttClient.publish(connectionStatus.str().c_str(), 0, true, "Online");
// //   mqttClient.subscribe(cmd2dev.str().c_str(), 0);
//   mqttClient.subscribe("OTAMQTT", 0);
//   //blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
// }

// void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//   String text; 
//   switch( reason) {
//   case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
//      text = "TCP_DISCONNECTED"; 

//      break; 
//   case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
//      text = "MQTT_UNACCEPTABLE_PROTOCOL_VERSION"; 

//      break; 
//   case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
//      text = "MQTT_IDENTIFIER_REJECTED";  

//      break;
//   case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE: 
//      text = "MQTT_SERVER_UNAVAILABLE"; 

//      break;
//   case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
//      text = "MQTT_MALFORMED_CREDENTIALS"; 

//      break;
//   case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
//      text = "MQTT_NOT_AUTHORIZED"; 

//      break;
  
//   }
//   Serial.printf(" [%8u] Disconnected from the broker reason = %s\n", millis(), text.c_str());
//   Serial.printf(" [%8u] Reconnecting to MQTT..\n", millis());
//   if (WiFi.isConnected()) {
//           connectToMqtt();
//    }
// }

// void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
//   Serial.println("Subscribe acknowledged.");
//   Serial.print("  packetId: ");
//   Serial.println(packetId);
//   Serial.print("  qos: ");
//   Serial.println(qos);
// }

// // void onMqttUnsubscribe(uint16_t packetId) {
// //   Serial.println("Unsubscribe acknowledged.");
// //   Serial.print("  packetId: ");
// //   Serial.println(packetId);
// // }

// void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
//   String topico = topic;
// //   StaticJsonDocument<sizejson> doc;
// //   DeserializationError error = deserializeJson(doc, payload);

// //   if (error) {
// //     Serial.print(F("deserializeJson() failed: "));
// //     Serial.println(error.f_str());
// //     return;
// //   }
  
// //   updateLastSectionIfDifferent(doc); //Criar função de configuração e colocar isso dentro... usa o dado configurado... se as keys não existirem, não importa, é só devolver o globaljson no doc com a configuração pré-determinada... Devolver nessa função os parâmetros de globaljson adaptado, exceto o que precisar mudar... isso implica que só altera no global js quando a função for de configuração... aqui na on mqttmessage o globaljso deveria apenas ser usado para completar as configurações e evitar que isso seja enviado o tempo todo.
// //   std::cout << globaljson.as<String>().c_str() << "\n";

// //     int op = uint16_t(doc["op"]); 
// //   if(functions.count(op) != 0){
// //     std::cout << "Entrou aqui1\n";
// //     String ans = functions[op](doc);
// //     std::cout << "Entrou aqui2\n";
// //     std::cout << ans;
// //     if( ans != "")//{
// //         mqttClient.publish(devans.str().c_str(), 0, true, ans.c_str());
// //   }
// }

// void onMqttPublish(uint16_t packetId) {
//   // Serial.println("Publish acknowledged.");
//   // Serial.print("  packetId: ");
//   // Serial.println(packetId);
// }

// bool wifiSTAMQTTInit(){
//   Serial.begin(115200);
//   delay(1000);
// //   Serial.println("Serial Inicializada");
// //   Serial.println();
// //   return 0;
//   if(true){
// //   if(loadEEPROM()){
//       // wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

//       WiFi.onEvent(WiFiEvent);
//       connectToWifi();
      
//       mqttClient.onConnect(onMqttConnect);
//       mqttClient.onDisconnect(onMqttDisconnect);
// //       // mqttClient.onSubscribe(onMqttSubscribe);
// //       // mqttClient.onUnsubscribe(onMqttUnsubscribe);
//       mqttClient.onMessage(onMqttMessage);
// //       // mqttClient.onPublish(onMqttPublish);
//       mqttClient.setServer(MQTT_HOST.c_str(), MQTT_PORT);//teste MQTT_HOST.c_str()
      
//       mqttClient.setKeepAlive(60);
      
// //       // mqttClient.setCredentials("neurogenicbladder","PajmuJM0imwyIxve");
// //       //mqttClient.setCredentials(login.c_str(),pass.c_str());
      
// //       resetEEPROMValuesRoutine();

// //       std::stringstream shortMacAddress; shortMacAddress << ESP.getEfuseMac();
// //       String macAddress = shortMacAddress.str().c_str();
// //       shortMacAddress.str(""); shortMacAddress  << macAddress[macAddress.length()-4] << macAddress[macAddress.length()-3]  
// //                                                 << macAddress[macAddress.length()-2] << macAddress[macAddress.length()-1];
// //       cmd2dev << "cmd2dev" << shortMacAddress.str().c_str();
// //       devans <<"dev" << shortMacAddress.str().c_str() << "ans";
// //       devstream <<"dev" << shortMacAddress.str().c_str() << "ss";
// //       connectionStatus << "dev" << shortMacAddress.str().c_str() << "status";
// //       lastWillTopic = connectionStatus.str().c_str();
// //       mqttClient.setWill(lastWillTopic.c_str(),0,true,"Offline",7U);
// //       //Serial.println(lastWillTopic);
//       return false;
  
//   } else {/*blinkTime = 1000; blinkMe(StaticJsonDocument<sizejson>()); createWIFIAP();*/  return true;}
  
//   // addFunctions("renameTopics",RENAMETOPICS_PARAMETERS,renameTopics);
// }

#endif