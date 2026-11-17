#ifndef WiFiMQTT_h
  #define WiFiMQTT_h

   #ifdef ESP32DEV
    #include "network/wifiAMQTTC3.h"
    //   #include <wifistaMQTT.h>
  #elif ESP32C3DEV
      #include "network/wifiAMQTTC3.h"
  #elif ESP32S2DEV
      #include "network/wifiAMQTTS2.h"
  #elif ESP32DEVC6
      #include "network/wifiAMQTTC3.h"
  #elif ESP32DEVC5
      #include "network/wifiAMQTTC5.h"
  #endif

#endif