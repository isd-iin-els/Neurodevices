
#include <wifiMQTT.h>
#include <sendIMUData.h>
//#include "IMUController.h"
#include "openLoopFes.h"
#include "wheelChair.h"
#include "asyncSerialServices.h"

// #include "adc1115_mqtt.h"
// #include "closedLoopFes.h"
// #include "blinkled.h"
// #include "fesBike.h"
// #include "NBStimulator.h"
//#include "bpm_MQTT.h"
// #include <IRremote.hpp>
// #include "sendInsoleData.h"
// #include "tsPCS.h"

// EEPROMClass  accelM("eeprom0");
// EEPROMClass  gyrM("eeprom1");
// EEPROMClass  magM("eeprom2");
bool captivPortal;

void setup() {

  sleep(2);
  // Serial.begin (115200);
  captivPortal = wifiSTAMQTTInit();
  startSerialService(); // Não é compactível com C3
  // // IrReceiver.begin(32, DISABLE_LED_FEEDBACK);
  
  // // addFunctions("OTAMQTT::updateFirmware",UPDATE_FIRMWARE_PARAMETERS,OTAMQTT::updateFirmware,25);
  addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate,2);
  addFunctions("restart",RESTART_PARAMETERS,restart,7);
  // addFunctions("reset",RESET_PARAMETERS,resetDevice,35);
  // // addFunctions("alive",ALIVE_PARAMETERS,alive);
  addFunctions("whoAmI",WHOAMI_PARAMETERS,whoAmI,9);
  addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,1);  //sensors.init();
  addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,22);
  addFunctions("sendWheelChairInit",SENDWHEELCHAIRINIT_PARAMETERS,wheelChair::sendWheelChairInit,37);
  // // addFunctions("tsPCS::sendInit",TSPCSSENDINIT_PARAMETERS,tsPCS::sendtsPCSInit,33);  
  // // addFunctions("tsPCS::openLoopUpdate",DURINGCICLEUPDATE_PARAMETERS,tsPCS::openLoopUpdate,34);
  // // addFunctions("IMUControllerInit",IMUControllerINIT_PARAMETERS,IMUControllerInit);  //sensors.init();
  // // addFunctions("IMUControllerStops",IMUControllerStop_PARAMETERS,IMUControllerStop);

  // // addFunctions("imuAccelerometerCalibration",IMUACCELEROMETERCALIBRATION_PARAMETERS,imuAccelerometerCalibration);  //sensors.init();
  // // addFunctions("imuGiroscopeCalibration",IMUGIROSCOPECALIBRATION_PARAMETERS,imuGiroscopeCalibration);  //sensors.init();
  // // addFunctions("imuMagnetometerCalibration",IMUMAGNETOMETERCALIBRATION_PARAMETERS,imuMagnetometerCalibration);  //sensors.init();
  addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes,8);
  addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate,18);
  // // addFunctions("fesBikeStart",FESBIKESTART_PARAMETERS,fesBikeStart);
  // //addFunctions("adcStream",ADCSTREAM_PARAMETERS,adcStream); 
  // //addFunctions("stopadcStream",STOPADCSTREAM_PARAMETERS,stopAdcStream); 
  // //addFunctions("bpmStream",BPMSTREAM_PARAMETERS,bpmStream); 
  // //addFunctions("stopBpmStream",STOPBPMSTREAM_PARAMETERS,stopBpmStream); 
  // // addFunctions("insoleStream",INSOLESTREAM_PARAMETERS,insoleStream,28); 
  // // addFunctions("stopInsoleStream",STOPINSOLESTREAM_PARAMETERS,stopInsoleStream,29); 


  // // addFunctions("TwoDOFLimbFesControl",CLOSEDLOOPFESCONTROL_PARAMETERS,TwoDOFLimbFesControl);
  // // addFunctions("closedLoopFesReferenceUpdate",CLOSEDLOOPFESREFERENCEUPDATE_PARAMETERS,closedLoopFesReferenceUpdate); //Fazer isso para o caso geral
  // // addFunctions("PIDsParametersUpdate",PIDSPARAMETERSUPDATE_PARAMETERS,PIDsParametersUpdate);//Fazer isso para o caso geral]

  // // addFunctions("blinkMe",BLINKME_PARAMETERS,blinkMe);

  
  
  // // // addFunctions("neurogenic_bladder_init",neurogenic_bladder_init);
  // // addFunctions("MP_PIDSclosedLoopFesmpreferenceUpdate",MP_PIDSFesReferenceUpdate);
  // // addFunctions("TwoDOFLimbMP_PIDSFes",MP_PIDSTwoDOFLimbFes);
  // // addFunctions("MP_PIDSParametersUpdate",MP_PIDSParametersUpdate);

  // // addFunctions("estimuladorBN",neuromoduladoBNUpdate); initialisateNeuromoduladoBNControl();
  // #ifdef ESP32DEV
  // addFunctions("sendAdc1115Init",ADC1115SENDINIT_PARAMETERS,adc1115MQTT::sendAdc1115Init,35);  //sensors.init();
  // #endif
  // // loadLastSectionConfig();
}

void loop() {
  if (captivPortal) dnsServer.processNextRequest();
  // delay(1000);
  // Serial.println("_");
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

// #include <Arduino.h>
// #include "WiFi.h"

// void printWifiStatus() {
//   Serial.print("SSID: ");
//   Serial.println(WiFi.SSID());

//   IPAddress ip = WiFi.localIP();
//   Serial.print("IP Address: ");
//   Serial.println(ip);

//   long rssi = WiFi.RSSI();
//   Serial.print("signal strength (RSSI):");
//   Serial.print(rssi);
//   Serial.println(" dBm");
// }


// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("lkjbkln");

//   WiFi.useStaticBuffers(true);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin("CAMPUS", "IINELS_educacional");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("Connected to WiFi");
//   printWifiStatus();
// }

// void loop() {
//   Serial.println("lkjbkln");
//   delay(1000);
// }

