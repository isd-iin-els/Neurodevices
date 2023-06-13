#ifndef tsPCSLoopfes_h
#define tsPCSLoopfes_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
//#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
//#include "sendIMUData.h"

namespace tsPCS{

volatile bool Loop_flag = false;


uint8_t modPin[8]    = {27,19,12,18,23,14,26,25},//ca
        levelPin[4]  = {13,4,2,33};

esp_timer_create_args_t DataLoop_periodic_timer_args;
esp_timer_handle_t DataLoop_periodic_timer = nullptr;
volatile uint64_t DataLoop_counter = 0;
volatile bool DataLoop_flag = false, state = false;

volatile uint16_t freq_boost;
volatile uint16_t freq_high;
volatile uint16_t freq_low;
volatile uint16_t pulse_width;
volatile uint16_t count = 0;
volatile uint16_t time_max = 0;

String openLoopUpdate(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {

  String answer;
  if (operation == DURINGCICLEUPDATE_MSG){
    const char *msg = doc["m"];
    LinAlg::Matrix<double> code = msg;

    // for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
    ledcWrite(0, code(0,0));  
    // if(dispositivo.stopLoopFlag){
    //   openLoopFesInit(doc["t"],doc["p"]);
    // }
    // if (doc.containsKey("f")) //f is for fade
    //   for(uint8_t i = 0; i < code.getNumberOfColumns(); ++i)
    //     dispositivo.fes[i].setFadeTime(doc["f"]); 
 
    answer += "1";
  }
  else
    answer += "";
  return answer;
}

static void DataLoop(void *param){

  if(count < pulse_width){
    if(state){
      digitalWrite(modPin[0],0);
      //Verificar a necessidade de um delay
      digitalWrite(modPin[1],1);
      state = !state;
    }else{
      digitalWrite(modPin[1],0);
      digitalWrite(modPin[0],1); 
      state = !state;
    }
  }else{
    digitalWrite(modPin[1],0);
    digitalWrite(modPin[0],0);
  }
  count++;
  if(count > time_max){
    count = 0;
  }

  // #ifdef WiFistaTCP_h
  //   client->write(ss.str().c_str());
  // #endif
  // #ifdef WiFistaMQTT_h
  //   mqttClient.publish(devstream.str().c_str(), 0, false, ss.str().c_str());
  // #endif 
    
  //std::cout << IMUDataLoop_counter << ss.str().c_str() << std::endl;

  if(DataLoop_counter>=(int)param)
  {
    ESP_ERROR_CHECK(esp_timer_stop(DataLoop_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(DataLoop_periodic_timer)); //Timer delete
    DataLoop_periodic_timer = nullptr;
    DataLoop_flag = false;
    std::cout << "Finalizou\n";
  }
}

String sendtsPCSInit(const StaticJsonDocument<sizejson> &doc, const uint8_t &operation) {
  String answer;
   std::cout << "Tudo Inicializado\n";
  if (operation == TSPCSSENDINIT_MSG && !DataLoop_flag){

    freq_boost = (uint16_t)doc["freq_boost"]; //high
    freq_high = 2*(uint16_t)doc["freq_high"]; //high
    freq_low = (uint16_t)doc["freq_low"]; //low
    pulse_width = (uint16_t)((uint16_t)doc["p_width"])/(1000000.0/freq_high); //low
    count = 0;
    time_max = freq_high/freq_low;

    pinMode(modPin[0], OUTPUT); //Channel 3
    pinMode(modPin[1], OUTPUT); //Channel 3

    ledcSetup(0, freq_boost, LEDC_TIMER_12_BIT);
    ledcAttachPin(levelPin[0], 0);



    DataLoop_flag = true;
    DataLoop_periodic_timer_args.callback = &DataLoop;
    DataLoop_periodic_timer_args.name = "tspcsSendInit";
    DataLoop_periodic_timer_args.arg = (void*)((int64_t)(freq_low));
    ESP_ERROR_CHECK(esp_timer_create(&DataLoop_periodic_timer_args, &DataLoop_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(DataLoop_periodic_timer, 1000000.0/freq_high));
    
    std::cout << "Tudo Inicializado\n";
    answer += "Loop para aquisicao e envio de dados criado a taxa de 1ms\r\n";
  }
  else
    answer += "";
  return answer;
}
}
#endif