#ifndef WiFiMQTT_h
  #define WiFiMQTT_h

   #ifdef ESP32DEV
      #include <wifistaMQTT.h>
  #elif ESP32C3DEV
      #include <wifiAMQTTC3.h>
  #elif ESP32S2DEV
      #include <wifiAMQTTC3.h>
  #endif

#endif