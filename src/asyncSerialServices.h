#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Buffer to store received data
#define SERIAL_BUFFER_SIZE 512
char serialBuffer[SERIAL_BUFFER_SIZE];
int serialBufferIndex = 0;

// Task handles
TaskHandle_t serialTaskHandle;

void onDataReceivedTask(void *pvParameters) {
  for (;;) {
    if (Serial.available() > 0) {
        String receivedLine = Serial.readStringUntil('\n');
        StaticJsonDocument<sizejson> doc;
        DeserializationError error = deserializeJson(doc, receivedLine);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        int op = uint16_t(doc["op"]); 
        if(functions.count(op) != 0){
            std::cout << "Entrou aqui1\n";
            String ans = functions[op](doc);
            std::cout << "Entrou aqui2\n";
            std::cout << ans;
            if( ans != "")//{
                Serial.println(ans.c_str());
        }
    }

    // Add more logic as needed

    vTaskDelay(pdMS_TO_TICKS(10)); // Delay to avoid high CPU usage
  }
}

void startSerialService() {
  Serial.begin(115200);

  // Create a task to handle serial communication
  xTaskCreatePinnedToCore(
      onDataReceivedTask,   // Task function
      "SerialTask",         // Task name
      4096,                 // Stack size (words)
      NULL,                 // Task parameters
      1,                    // Priority
      &serialTaskHandle,    // Task handle
      0);                   // Task runs on core 0
}

