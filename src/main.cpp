
// // #define DEVICEID uint64_t chipid = ESP.getEfuseMac();

// #include <Arduino.h>
// #include <AsyncTCP.h>
// #include "WiFi.h"
// #include <sstream>
// #include "wifistaTCP.h"
// // #include "sendIMUData.h"
// // #include "openLoopFes.h"
// // #include "closedLoopFes.h"
// // #include "blinkled.h"
// // #include "fesBike.h"
// // #include "NBStimulator.h"
// // #include "mp_closedLoopFes.h"
// // #include "estimuladorBN.h"

// void setup() {
//   Serial.begin(115200); //initDMP6(gpio_num_t(23));
//   // serverIP = IPAddress(192,168,137,101);
//   wifiSTATCPInit();
//   // addFunctions("imuSendInit",imuSendInit);  //sensors.init();
//   // addFunctions("stopOpenLoopFes",stopOpenLoopFes);
//   // addFunctions("openLoopFesUpdate",openLoopFesUpdate);
//   // addFunctions("TwoDOFLimbFesControl",TwoDOFLimbFesControl);
//   // addFunctions("closedLoopFesReferenceUpdate",closedLoopFesReferenceUpdate); //Fazer isso para o caso geral
//   // addFunctions("PIDsParametersUpdate",PIDsParametersUpdate);//Fazer isso para o caso geral]
  
//   // // addFunctions("fesBikeStart",fesBikeStart);
//   addFunctions("whoAmI",whoAmI);
//   // addFunctions("blinkMe",blinkMe);
//   // // addFunctions("neurogenic_bladder_init",neurogenic_bladder_init);
//   // addFunctions("MP_PIDSclosedLoopFesmpreferenceUpdate",MP_PIDSFesReferenceUpdate);
//   // addFunctions("TwoDOFLimbMP_PIDSFes",MP_PIDSTwoDOFLimbFes);
//   // addFunctions("MP_PIDSParametersUpdate",MP_PIDSParametersUpdate);

//   // addFunctions("estimuladorBN",neuromoduladoBNUpdate); initialisateNeuromoduladoBNControl();
//   // analogSetAttenuation((adc_attenuation_t) ADC_ATTEN_0db);
//   WiFiClient client;
 
//   if (!client.connect("192.168.137.1", 5000)) {

//     Serial.println("Connection to host failed");

//     delay(1000);
//     return;
//   }
//   std::stringstream ss;
//   ss << "{\"api_command\":\"registerClient\"," << "\"Device IP\":\"" << WiFi.localIP().toString().c_str()<< "\"}"; 
//   Serial.println("Connected to server successful!");

//   client.print(ss.str().c_str());
//   delay(1000);
//   Serial.println("Disconnecting...");
//   client.stop();
// }
// // float x = 0, h = 0.01; uint8_t contador = 0;
// void loop() {
//   // delay(10);
//   // x = x + h*(analogReadMilliVolts(36)-x);
//   // if(contador==0)
//   //   Serial.println(x);
//   // contador++;
// }

#include <wifistaMQTT.h>
#include <sendIMUData.h>
#include "IMUController.h"
#include "openLoopFes.h"
// #include "closedLoopFes.h"
// #include "blinkled.h"
// #include "fesBike.h"
// #include "NBStimulator.h"
//#include "bpm_MQTT.h"
// #include <IRremote.hpp>
// #include "sendInsoleData.h"

EEPROMClass  accelM("eeprom0");
EEPROMClass  gyrM("eeprom1");
EEPROMClass  magM("eeprom2");
bool captivPortal;

void setup() {
  
  sleep(2);
  captivPortal = wifiSTAMQTTInit();
  // IrReceiver.begin(32, DISABLE_LED_FEEDBACK);
  
  addFunctions("OTAMQTT::updateFirmware",UPDATE_FIRMWARE_PARAMETERS,OTAMQTT::updateFirmware);
  addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate);
  addFunctions("restart",RESTART_PARAMETERS,restart);
  addFunctions("alive",ALIVE_PARAMETERS,alive);
  addFunctions("whoAmI",WHOAMI_PARAMETERS,whoAmI);
  //addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit);  //sensors.init();
  //addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop);
  addFunctions("IMUControllerInit",IMUControllerINIT_PARAMETERS,IMUControllerInit);  //sensors.init();
  addFunctions("IMUControllerStops",IMUControllerStop_PARAMETERS,IMUControllerStop);

  // addFunctions("imuAccelerometerCalibration",IMUACCELEROMETERCALIBRATION_PARAMETERS,imuAccelerometerCalibration);  //sensors.init();
  // addFunctions("imuGiroscopeCalibration",IMUGIROSCOPECALIBRATION_PARAMETERS,imuGiroscopeCalibration);  //sensors.init();
  // addFunctions("imuMagnetometerCalibration",IMUMAGNETOMETERCALIBRATION_PARAMETERS,imuMagnetometerCalibration);  //sensors.init();
  addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes);
  addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate);
  // addFunctions("fesBikeStart",FESBIKESTART_PARAMETERS,fesBikeStart);
  //addFunctions("adcStream",ADCSTREAM_PARAMETERS,adcStream); 
  //addFunctions("stopadcStream",STOPADCSTREAM_PARAMETERS,stopAdcStream); 
  //addFunctions("bpmStream",BPMSTREAM_PARAMETERS,bpmStream); 
  //addFunctions("stopBpmStream",STOPBPMSTREAM_PARAMETERS,stopBpmStream); 
  // addFunctions("insoleStream",INSOLESTREAM_PARAMETERS,insoleStream); 
  // addFunctions("stopInsoleStream",STOPINSOLESTREAM_PARAMETERS,stopInsoleStream); 


  // addFunctions("TwoDOFLimbFesControl",CLOSEDLOOPFESCONTROL_PARAMETERS,TwoDOFLimbFesControl);
  // addFunctions("closedLoopFesReferenceUpdate",CLOSEDLOOPFESREFERENCEUPDATE_PARAMETERS,closedLoopFesReferenceUpdate); //Fazer isso para o caso geral
  // addFunctions("PIDsParametersUpdate",PIDSPARAMETERSUPDATE_PARAMETERS,PIDsParametersUpdate);//Fazer isso para o caso geral]

  // addFunctions("blinkMe",BLINKME_PARAMETERS,blinkMe);

  
  
  // // addFunctions("neurogenic_bladder_init",neurogenic_bladder_init);
  // addFunctions("MP_PIDSclosedLoopFesmpreferenceUpdate",MP_PIDSFesReferenceUpdate);
  // addFunctions("TwoDOFLimbMP_PIDSFes",MP_PIDSTwoDOFLimbFes);
  // addFunctions("MP_PIDSParametersUpdate",MP_PIDSParametersUpdate);

  // addFunctions("estimuladorBN",neuromoduladoBNUpdate); initialisateNeuromoduladoBNControl();
}

void loop() {
  if (captivPortal) dnsServer.processNextRequest();
  // if (IrReceiver.decode()) {  // Grab an IR code
  //     if (IrReceiver.decodedIRData.numberOfBits > 0 && IrReceiver.decodedIRData.protocol == 18) {
  //         Serial.println();  
  //         Serial.println(IrReceiver.decodedIRData.address, HEX);
  //         Serial.println(IrReceiver.decodedIRData.command, HEX);
  //         Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
  //         Serial.println(IrReceiver.decodedIRData.protocol);  
  //         String toSend = String(IrReceiver.decodedIRData.address)+","+
  //                         String(IrReceiver.decodedIRData.command)+","+
  //                         String(IrReceiver.decodedIRData.decodedRawData)+","+
  //                         String(IrReceiver.decodedIRData.protocol)+"\n";
  //         mqttClient.publish("emergencyButton", 0, true, toSend.c_str());
  //     }
  // }
  // IrReceiver.resume();   // Prepare for the next value
  // int potValue = analogRead(36);
  // Serial.println(potValue);
  //delay(100);
  //Serial.println("EU usei OTA");
}


// //Batimento Cardiaco
// //Batimento Cardiaco
// #include <wifistaMQTT.h>
// #include <sendIMUData.h>
// #include "driver/adc.h"
// #include "esp_adc_cal.h"

// // #define loMais 4
// // #define loMenos 2
// double filt = 0;
// double time, atime, afilt = 0,bpm=0;


// void setup() {
//   Serial.begin(115200);
//   // pinMode(loMais, INPUT); // Setup for leads off detection LO +
//   // pinMode(loMenos, INPUT); // Setup for leads off detection LO -
//   adc1_config_width(ADC_WIDTH_BIT_12);
//   adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_11db);
//   atime = millis();
// }

// void loop() {
  
//   // filt += 0.1*(analogRead(36)-filt);
//   // Serial.println(filt);
//   // if(filt-afilt > 50){
//   //   time = millis();
//   //   double bpmTemp = (time-atime)*60;
//   //   bpm += 0.1*(bpmTemp-bpm);
//   //   Serial.print("BPM = ");Serial.println(bpm);
//   // }
//   // delay(20);
// }
