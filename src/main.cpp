
#include <wifiMQTT.h>
#include <sendIMUData.h>
#include "openLoopFes.h"
#include "asyncSerialServices.h"
#include "Stimperturb.h"

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
  addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes,8);
  addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate,18);
  addFunctions("StimPerturb",SENDINSOLE_PARAMETERS,protpertubation::StimPerturb,38);
}

void loop() {
  if (captivPortal) dnsServer.processNextRequest();
}


