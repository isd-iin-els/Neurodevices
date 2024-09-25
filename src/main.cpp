
#include <wifiMQTT.h>
#include <sendIMUData.h>
#include "openLoopFes.h"
#include "asyncSerialServices.h"
#include "sendInsoleData.h"
#include "Stimperturb.h"
#include "adc1115_mqtt.h"
#include "controlStim.h"


bool captivPortal;

void setup() {

  sleep(2);
  captivPortal = wifiSTAMQTTInit();
  startSerialService(); // Não é compactível com C3

  addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate,2);
  addFunctions("restart",RESTART_PARAMETERS,restart,7);
  addFunctions("whoAmI",WHOAMI_PARAMETERS,whoAmI,9);
  addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,1);  //sensors.init();
  addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,22);
  addFunctions("INSOLESTREAM",INSOLESTREAM_PARAMETERS,insoleStream,28); 
  addFunctions("STOPINSOLESTREAM",STOPINSOLESTREAM_PARAMETERS,stopInsoleStream,29);
  addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes,8);
  addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate,18);
  addFunctions("StimPerturb",SENDINSOLE_PARAMETERS, protpertubation::StimPerturb,38);
  addFunctions("adc1115SendInit", ADC1115SENDINIT_PARAMETERS, adc1115MQTT::sendAdc1115Init,36);
  addFunctions("setStim", setStim_PARAMETERS, ControlStim::setStim,41); 
}

void loop() {
  if (captivPortal) dnsServer.processNextRequest();
}


