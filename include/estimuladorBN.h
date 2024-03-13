#ifndef ESTIMULADORBN_h
#define ESTIMULADORBN_h

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
#include "SistemasdeControle/embeddedTools/signalAnalysis/systemLoop.h"
#include "wifistaTCP.h"
#include <EEPROM.h>
#define EEPROM_SIZE 512

esp_timer_create_args_t neuromoduladoBN_periodic_timer_args;
esp_timer_handle_t neuromoduladoBN_periodic_timer = nullptr, IMUClosedLoop_periodic_timer = nullptr;
volatile bool neuromoduladoBN_flag = false;
volatile uint64_t neuromoduladoBN_Counter;

uint8_t  modPin[4]    = {12,27},
         levelPin[4]  = {13},
         tOn = 200, tempoSessao = 30*60,
         stimulationSetedLevel = 0;
uint16_t period = 20000;
float tempoUsoSegundos = 0, estimulacaoMedia = 0;

void memoryInitialisation(){
    EEPROM.begin(EEPROM_SIZE);
    tOn = EEPROM.read(0);
    period = EEPROM.read(1);
    tempoUsoSegundos = EEPROM.read(2);
    estimulacaoMedia = EEPROM.read(3);
}

void IRAM_ATTR neuromoduladoBNLoop(void *param){
    neuromoduladoBN_Counter++;
    float analogValue = analogRead(36);
    float outputControl = dispositivo.getPID(0).OutputControl(analogValue,stimulationSetedLevel);
    dispositivo.fes[0].setPowerLevel(outputControl); 
    
    tempoUsoSegundos += 0.1;
    estimulacaoMedia += analogValue;
    EEPROM.write(2,tempoUsoSegundos); // tempoUsoSegundos
    EEPROM.write(3,estimulacaoMedia); // estimulacaoMedia

  if(neuromoduladoBN_Counter==(int)param)
  {
    dispositivo.fes[0].setPowerLevel(0); 
    printf("stop\r\n"); //Print information
    ESP_ERROR_CHECK(esp_timer_stop(neuromoduladoBN_periodic_timer)); //Timer pause
    ESP_ERROR_CHECK(esp_timer_delete(neuromoduladoBN_periodic_timer)); //Timer delete
    neuromoduladoBN_periodic_timer = nullptr;
    neuromoduladoBN_flag = false;
    neuromoduladoBNStop();
  }
}
uint8_t button_1 = 25,button_2 = 26,interrupt_time_1 = 0,last_interrupt_time_1 = 0,
        interrupt_time_2 = 0,last_interrupt_time_2 = 0;

void debounce_1(){
  unsigned long interrupt_time_1 = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time_1 - last_interrupt_time_1 > 200) {
      
  }
  last_interrupt_time_1 = interrupt_time_1;
}

void initialisateNeuromoduladoBNControl(){
    pinMode(button_1,INPUT);
    pinMode(button_2,INPUT);
    attachInterrupt(digitalPinToInterrupt(button_1), debounce_1, CHANGE);

    memoryInitialisation();
    dispositivo.fes[0].setPowerLevel(0); 
    delay(10000);

    if(WiFi.waitForConnectResult() != WL_CONNECTED){
        dispositivo.getPID(0).setParams(1,0.1,0); dispositivo.getPID(0).setSampleTime(1); dispositivo.getPID(0).setLimits(1.1,1.5);
        // A seleção entre modo wifi e modo de operação será escolhida com base no apertar do botão também. 
        // Devo fazer um loop para previamente regular a corrente, quando o usuário mantiver o botão por 2s, 
        // começar a contar o tempo de estimulação

        neuromoduladoBN_periodic_timer_args.callback = &neuromoduladoBNLoop;
        neuromoduladoBN_periodic_timer_args.name = "neuromoduladoBNLoop";
        neuromoduladoBN_periodic_timer_args.arg = (void*)(int (tempoSessao/0.1));
        ESP_ERROR_CHECK(esp_timer_create(&neuromoduladoBN_periodic_timer_args, &neuromoduladoBN_periodic_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(neuromoduladoBN_periodic_timer, tempoSessao*1000000.0));
        neuromoduladoBN_Counter = 0;
    }
}

Devices::fes4channels dispositivo(levelPin, modPin, 1, 18000, tOn, period, true);

void neuromoduladoBNInit(uint16_t ton, uint16_t period){
    neuromoduladoBN_flag = true;
    if(dispositivo.stopLoopFlag){
      dispositivo.stopLoopFlag = false;
      dispositivo.timeOnAndPeriodUpdate(ton,period);
      dispositivo.startLoop();
    }
}

void neuromoduladoBNStop(){
    for(uint8_t i = 0; i < 4; ++i)
      dispositivo.fes[i].setPowerLevel(0); 
    dispositivo.stopLoopFlag = true;
}

String neuromoduladoBNUpdate(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 9){
    Serial.print("Operation 9, received data: "); Serial.println(msg);
    tOn = code(0,0);
    EEPROM.write(0,tOn);
    period = code(0,1);
    EEPROM.write(1,period);
    tempoSessao = code(0,2);
    EEPROM.write(1,tempoSessao);
    // for(uint8_t i = 0; i < code.getNumberOfColumns()-2; ++i)
    //   dispositivo.fes[i].setPowerLevel(code(0,i));  
    // neuromoduladoBNInit(code(0,code.getNumberOfColumns()-2),code(0,code.getNumberOfColumns()-1));
    
    if(!noAnswer)
      answer += "Valores de estimulacao alterados\r\n";
  }
  else
    answer += "";
  return answer;
}

String stopNeuromoduladoBN(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 10){
    Serial.print("Operation 10, received data: "); Serial.println(msg);
    neuromoduladoBNStop();
    if(!noAnswer)
      answer += "Valores de estimulacao alterados\r\n";
  }
  else
    answer += "";
  return answer;
}

String getNeuromoduladoBNData(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 11){
    Serial.print("Operation 11, received data: "); Serial.println(msg);
    std::stringstream ss;
    ss << "Tempo de Uso em Segundos: " << tempoUsoSegundos << std::endl;
    ss << "Valor medio de estimulacao: " << estimulacaoMedia << std::endl;
    tempoUsoSegundos = EEPROM.read(2);
    estimulacaoMedia = EEPROM.read(3);
    if(!noAnswer)
      answer += ss.str().c_str();
  }
  else
    answer += "";
  return answer;
}

String clearNeuromoduladoBNData(void* data, size_t len) {
  char* d = reinterpret_cast<char*>(data); String msg,answer;
  for (size_t i = 0; i < len; ++i) msg += d[i];
  uint16_t index = msg.indexOf('?'); String op = msg.substring(0,index);
  msg = msg.substring(index+1,msg.length());
  LinAlg::Matrix<double> code = msg.c_str();

  if (op.toInt() == 11){
    Serial.print("Operation 11, received data: "); Serial.println(msg);
    EEPROM.write(2,0); // tempoUsoSegundos
    EEPROM.write(3,0); // estimulacaoMedia
    if(!noAnswer)
      answer += "Dados apagados com sucesso!\r\n";
  }
  else
    answer += "";
  return answer;
}

#endif