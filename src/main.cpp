
// // #include "Arduino.h"
// #include "network/wifiMQTT.h"
// #include "imuSensor/sendIMUData.h"
// // #include "openLoopFes.h"
// #include "stim/tsPCS.h"
// // #include "asyncSerialServices.h"
// // #include "sendInsoleData.h"
// // #include "Stimperturb.h"
// // #include "adc1115_mqtt.h"
// // #include "controlStim.h"


// bool captivPortal;

// void setup() {
//   sleep(2);
//   // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
//   captivPortal = wifiSTAMQTTInit();
//   // // startSerialService(); // Não é compactível com C3

//   addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate,2);
//   // // addFunctions("restart",RESTART_PARAMETERS,restart,7);
//   // addFunctions("subscribe",subscribe_PARAMETERS,subscribe,0);
//   // addFunctions("who_am_i",WHOAMI_PARAMETERS,who_am_i,9);
//   // addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,1);  //sensors.init();
//   // addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,22);
//   // // addFunctions("INSOLESTREAM",INSOLESTREAM_PARAMETERS,insoleStream,28); 
//   // // addFunctions("STOPINSOLESTREAM",STOPINSOLESTREAM_PARAMETERS,stopInsoleStream,29);
//   // // addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes,8);
//   // //addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate,18);
//   // // addFunctions("StimPerturb",SENDINSOLE_PARAMETERS, protpertubation::StimPerturb,38);
//   // // addFunctions("adc1115SendInit", ADC1115SENDINIT_PARAMETERS, adc1115MQTT::sendAdc1115Init,36);
//   // // addFunctions("setStim", setStim_PARAMETERS, ControlStim::setStim,41); 

//   // addFunctions("TSPCS_PARAMETERS",TSPCS_PARAMETERS,tsPCS::openLoopTspcsUpdate,2);
//   // addFunctions("INIT_TSPCS_PARAMETERS",INIT_TSPCS_PARAMETERS,tsPCS::sendtsPCSInit,3);

//   addFunctions("who_am_i",WHOAMI_PARAMETERS,who_am_i,0);
//   addFunctions("subscribe",subscribe_PARAMETERS,subscribe,1);
//   // addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,2);  //sensors.init();
//   // addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,3);
// }

// void loop() {
//   if (captivPortal) dnsServer.processNextRequest();
// }


#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "network/wifiMQTT.h"
#include "imuSensor/sendIMUData.h"
// #include "openLoopFes.h"
#include "stim/tsPCS.h"
// #include "asyncSerialServices.h"
// #include "sendInsoleData.h"
// #include "Stimperturb.h"
// #include "adc1115_mqtt.h"
// #include "controlStim.h"


bool captivPortal;
// Função opcional de loop contínuo rodando em background (Task do FreeRTOS)
void loop(void *pvParameters);

// O ESP-IDF exige o app_main com extern "C" em arquivos C++ (.cpp)
extern "C" void app_main(void) {
    // 1. Inicializa obrigatoriamente o framework do Arduino
  initArduino();

    
    
  sleep(2);
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  captivPortal = wifiSTAMQTTInit();
  // // startSerialService(); // Não é compactível com C3

//   addFunctions("openLoopFesUpdate",OPENLOOPFESUPDATE_PARAMETERS,openLoopFesUpdate,2);
  // // addFunctions("restart",RESTART_PARAMETERS,restart,7);
  // addFunctions("subscribe",subscribe_PARAMETERS,subscribe,0);
  // addFunctions("who_am_i",WHOAMI_PARAMETERS,who_am_i,9);
  // addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,1);  //sensors.init();
  // addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,22);
  // // addFunctions("INSOLESTREAM",INSOLESTREAM_PARAMETERS,insoleStream,28); 
  // // addFunctions("STOPINSOLESTREAM",STOPINSOLESTREAM_PARAMETERS,stopInsoleStream,29);
  // // addFunctions("stopOpenLoopFes",STOPOPENLOOPFES_PARAMETERS,stopOpenLoopFes,8);
  // //addFunctions("openLoopTonFreqUpdate",OPENLOOPTONFREQUPDATE_PARAMETERS,openLoopTonFreqUpdate,18);
  // // addFunctions("StimPerturb",SENDINSOLE_PARAMETERS, protpertubation::StimPerturb,38);
  // // addFunctions("adc1115SendInit", ADC1115SENDINIT_PARAMETERS, adc1115MQTT::sendAdc1115Init,36);
  // // addFunctions("setStim", setStim_PARAMETERS, ControlStim::setStim,41); 

  addFunctions("TSPCS_PARAMETERS",TSPCS_PARAMETERS,tsPCS::openLoopTspcsUpdate,2);
  addFunctions("INIT_TSPCS_PARAMETERS",INIT_TSPCS_PARAMETERS,tsPCS::sendtsPCSInit,3);

  addFunctions("who_am_i",WHOAMI_PARAMETERS,who_am_i,0);
  addFunctions("subscribe",subscribe_PARAMETERS,subscribe,1);
  // addFunctions("imuSendInit",IMUSENDINIT_PARAMETERS,imuSendInit,2);  //sensors.init();
  // addFunctions("imuSendStop",IMUSENDSTOP_PARAMETERS,imuSendStop,3);




    // 3. Cria a thread/task para rodar sua rotina constantemente
    xTaskCreatePinnedToCore(
        loop,        // Ponteiro para a função do loop
        "LoopTask",           // Nome da task
        1024,                 // Tamanho da pilha (Stack)
        NULL,                 // Parâmetro
        1,                    // Prioridade
        NULL,                 // Handle
        1                     // Núcleo (Core 1)
    );

    // O app_main pode retornar ou ficar vazio aqui, 
    // pois a task criada continuará rodando infinitamente em background.
}

void loop(void *pvParameters) {
    while (true) {
        if (captivPortal) dnsServer.processNextRequest();
        delay(100);
    }
}