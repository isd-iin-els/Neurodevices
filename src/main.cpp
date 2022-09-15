
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
#include "openLoopFes.h"
#include "closedLoopFes.h"
#include "blinkled.h"
#include "fesBike.h"
#include "NBStimulator.h"
EEPROMClass  accelM("eeprom0");
EEPROMClass  gyrM("eeprom1");
EEPROMClass  magM("eeprom2");

void setup() {

  sleep(2);
  wifiSTAMQTTInit();
  

  addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate);
  addFunctions("restart",RESTART_PARAMETERS,restart);
  addFunctions("alive",ALIVE_PARAMETERS,alive);
  addFunctions("whoAmI",WHOAMI_PARAMETERS,whoAmI);
  addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit);  //sensors.init();
  addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop);
  addFunctions("imuAccelerometerCalibration",IMUACCELEROMETERCALIBRATION_PARAMETERS,imuAccelerometerCalibration);  //sensors.init();
  addFunctions("imuGiroscopeCalibration",IMUGIROSCOPECALIBRATION_PARAMETERS,imuGiroscopeCalibration);  //sensors.init();
  addFunctions("imuMagnetometerCalibration",IMUMAGNETOMETERCALIBRATION_PARAMETERS,imuMagnetometerCalibration);  //sensors.init();
  addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes);
  addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate);
  addFunctions("fesBikeStart",FESBIKESTART_PARAMETERS,fesBikeStart);
  addFunctions("adcStream",ADCSTREAM_PARAMETERS,adcStream); 


  addFunctions("TwoDOFLimbFesControl",CLOSEDLOOPFESCONTROL_PARAMETERS,TwoDOFLimbFesControl);
  addFunctions("closedLoopFesReferenceUpdate",CLOSEDLOOPFESREFERENCEUPDATE_PARAMETERS,closedLoopFesReferenceUpdate); //Fazer isso para o caso geral
  addFunctions("PIDsParametersUpdate",PIDSPARAMETERSUPDATE_PARAMETERS,PIDsParametersUpdate);//Fazer isso para o caso geral]
  // addFunctions("blinkMe",BLINKME_PARAMETERS,blinkMe);

  
  
  // // addFunctions("neurogenic_bladder_init",neurogenic_bladder_init);
  // addFunctions("MP_PIDSclosedLoopFesmpreferenceUpdate",MP_PIDSFesReferenceUpdate);
  // addFunctions("TwoDOFLimbMP_PIDSFes",MP_PIDSTwoDOFLimbFes);
  // addFunctions("MP_PIDSParametersUpdate",MP_PIDSParametersUpdate);

  // addFunctions("estimuladorBN",neuromoduladoBNUpdate); initialisateNeuromoduladoBNControl();
}

void loop() {
  // int potValue = analogRead(36);
  // Serial.println(potValue);
  // delay(500);
}
