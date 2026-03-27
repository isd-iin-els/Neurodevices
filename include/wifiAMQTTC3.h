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
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include <map>
#include "esp_wifi.h"
// #include "blinkled.h"

class MqttClient {
	public:
		esp_mqtt_client_handle_t mqtt_client ;
		uint16_t subscribe(const char* topic, uint8_t qos){
			return esp_mqtt_client_subscribe (mqtt_client, topic, qos);
		}
		uint16_t publish(const char* topic, uint8_t qos, bool retain, String payload, size_t length = 0, bool dup = false, uint16_t message_id = 0){
			return esp_mqtt_client_publish (mqtt_client, topic, payload.c_str(), payload.length(), qos, false);
		}
};

esp_mqtt_client_config_t mqtt_cfg = {};
MqttClient mqttClient;

// const char* WIFI_SSID = "CAMPUS";
// const char* WIFI_PASSWD = "IINELS_educacional";
// const char* MQTT_HOST = "10.1.0.18";
// const uint32_t MQTT_PORT = 1883;
const uint16_t sizejson = 512;
std::stringstream cmd2dev,devans,devstream,connectionStatus;
String lastWillTopic, stream_callback_topic = "\0";

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

String subscribe(const StaticJsonDocument<sizejson> &doc)  {
	String answer;
	String topic = doc["topic"];
	stream_callback_topic = topic.c_str();
	int qos = doc["qos"];
	Serial.print("Subscribing to topic: "); Serial.println(topic.c_str());
	int msg_id = mqttClient.subscribe(topic.c_str(), qos);
	return answer;
}

String who_am_i(const StaticJsonDocument<sizejson> &doc/*, const uint8_t &operation*/)  {
  String answer;

	std::stringstream ss, functionalities;
	ss << "{";
	ss << "\"service_id\":\"" << ESP.getEfuseMac() << "\",";   // entre aspas para ser sempre string
	ss << "\"flow\": {\"in\": [\"float\"],\"out\": [\"float\"]}" << ",";
	ss << "\"service_name\":\"DOIT Esp32 DevKit v1\",";
	ss << "\"service_description\":\"" << devFunction.c_str() << "\",";
	ss << "\"service_type\":\"default\",";
	ss << "\"topics\":[\"" << cmd2dev.str() << "\",\"" 
	<< devans.str() << "\",\"" 
	<< devstream.str() << "\"],";
	ss << "\"commands\":[";

	// Monta os comandos
	for (auto iter = functionalitiesParameters.begin(); iter != functionalitiesParameters.end(); ++iter) {
		StaticJsonDocument<sizejson> doc;
		DeserializationError error = deserializeJson(doc, functionalitiesParameters[iter->first].c_str());

		if (error) continue; // ignora erro de parsing

		ss << "{";
		ss << "\"op\":" << (int)doc["op"] << ",";
		doc.remove("op");

		String jsonString;
		serializeJson(doc, jsonString);

		ss << "\"name\":\"" << iter->first.c_str() << "\",";
		ss << "\"parameters\":" << jsonString.c_str();
		ss << "}";

		if (std::next(iter) != functionalitiesParameters.end())
			ss << ",";
	}

	ss << "]}";


    answer = ss.str().c_str();
    return answer;
}

#include "blinkled.h"

void connectToWifi() {
	Serial.println("Connecting to Wi-Fi...");
	WiFi.onEvent(WiFiEvent);
	WiFi.mode (WIFI_MODE_STA);
	WiFi.setSleep(false);
	esp_wifi_set_ps (WIFI_PS_NONE);
	WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
	while (!WiFi.isConnected ()) {
		Serial.print ('.');
		delay (100);
	}
}

void createTopics(){
	std::stringstream shortMacAddress; shortMacAddress << ESP.getEfuseMac();
	// String macAddress = shortMacAddress.str().c_str();
	// shortMacAddress.str(""); shortMacAddress  << macAddress[macAddress.length()-4] << macAddress[macAddress.length()-3]  
	// 										<< macAddress[macAddress.length()-2] << macAddress[macAddress.length()-1];
	cmd2dev << "cmd/" << shortMacAddress.str().c_str();
	devans <<"status/" << shortMacAddress.str().c_str() ;
	devstream <<"stream/" << shortMacAddress.str().c_str();
	// connectionStatus << "dev" << shortMacAddress.str().c_str() << "status";
	// lastWillTopic = connectionStatus.str().c_str();
}

void connectToMqtt() {
	mqtt_cfg.host = MQTT_HOST.c_str();
	mqtt_cfg.port = MQTT_PORT;
	mqtt_cfg.keepalive = 15;

	mqtt_cfg.transport = MQTT_TRANSPORT_OVER_TCP;

	mqtt_cfg.event_handle = mqtt_event_handler;
	// mqtt_cfg.lwt_topic = lastWillTopic.c_str();
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
			
        	blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
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
		// Serial.println("Evento Recebeu Dado");
		onMqttMessage(event->topic, event->data);
	} else  if (event->event_id == MQTT_EVENT_BEFORE_CONNECT) {
    	Serial.println("Antes de conectar");
	}
	return ESP_OK;
}

void onMqttConnect() {
	// blinkTime = 5; blinkMe(StaticJsonDocument<sizejson>());
	std::stringstream shortMacAddress, cmd2devMac; shortMacAddress << ESP.getEfuseMac();
    String macAddress = shortMacAddress.str().c_str();
    shortMacAddress.str(""); shortMacAddress  << macAddress[macAddress.length()-4] << macAddress[macAddress.length()-3]  
                                            << macAddress[macAddress.length()-2] << macAddress[macAddress.length()-1];
    cmd2devMac << "cmd2dev" << shortMacAddress.str().c_str();
	Serial.println("MQTT Connected");
	Serial.println(cmd2dev.str().c_str());
	Serial.println(devans.str().c_str());
	Serial.println(devstream.str().c_str());

	Serial.println(cmd2devMac.str().c_str());
	// Serial.println(connectionStatus.str().c_str());
  
	mqttClient.publish("newservice", 1, false, who_am_i(StaticJsonDocument<sizejson>()).c_str());
	// mqttClient.publish(connectionStatus.str().c_str(), 0, true, "Online");
	mqttClient.subscribe(cmd2dev.str().c_str(), 0);
	mqttClient.subscribe(cmd2devMac.str().c_str(), 0);
	mqttClient.subscribe("broadcast/get_active_services", 0);
}

double saida = 0;
void onMqttMessage(char* topic, char* payload){
	String topico = topic;
	// Serial.println(topico);
	// Serial.println(payload);
	// Serial.println(stream_callback_topic);
	// // Serial.println(1);
	if(topico.indexOf("broadcast/get_active_services") != -1){
		mqttClient.publish("newservice", 1, false, who_am_i(StaticJsonDocument<sizejson>()).c_str());
		// Serial.println("newdev");
		// Serial.println( who_am_i(StaticJsonDocument<sizejson>()).c_str());
		return;
	}
	// Serial.println(2);
	StaticJsonDocument<sizejson> doc;
	DeserializationError error = deserializeJson(doc, payload);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}
	// Serial.println(3);
	if(topico.substring(0, 15) == stream_callback_topic.substring(0, 15)){
			Serial.println("Topic stream: ");
			double control = doc["control"];
			Serial.println(control);
			saida = 0.9 * saida + 0.004 * control;
			// Serial.print("Saida: "); Serial.println(0.9 * saida + 0.004 * control);
			// Serial.print("Saida: "); Serial.println(0.9 * saida + 0.004 * control);
			Serial.println(saida);
			std::ostringstream ss;
			ss << "{\"output\": " << saida << "}";

			std::string topic = devstream.str();
			std::string payload = ss.str();
			// Serial.println(saida);
			mqttClient.publish(topic.c_str(), 0, false, payload.c_str());

		return;
	}

	int op = uint16_t(doc["op"]); 
	Serial.println(op);
	Serial.println(doc["parameters"].as<String>().c_str());
	
	if (doc.containsKey("parameters")) {
		
		StaticJsonDocument<sizejson> parameters;
		error = deserializeJson(parameters, doc["parameters"].as<String>().c_str());
		if (error) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.f_str());
			return;
		}
		if(functions.count(op) != 0){
			String ans = functions[op](parameters);
			std::cout << ans;
			if( ans != "")//{
				mqttClient.publish(devans.str().c_str(), 0, true, ans.c_str());
		}
	}else{
		Serial.println(op);
		Serial.println(doc.as<String>().c_str());
		if(functions.count(op) != 0){
			Serial.println(doc.as<String>().c_str());
			String ans = functions[op](doc);
			std::cout << ans;
			if( ans != "")//{
				mqttClient.publish(devans.str().c_str(), 0, true, ans.c_str());
		}
	}
}

bool wifiSTAMQTTInit(){ 
	Serial.begin (115200);
	// delay(1000); return false;
	if(loadEEPROM()){resetEEPROMValuesRoutine();createTopics();connectToWifi();return false;
	}else{blinkTime = 1000; blinkMe(StaticJsonDocument<sizejson>());createWIFIAP();return true;}
}

#endif