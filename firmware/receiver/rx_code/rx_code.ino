#include <RH_ASK.h>
#include <SPI.h> 
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
RH_ASK rf_driver(2000, 27, 26, 0);

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_RF_Receptor");
  
  if (!rf_driver.init()) {
    Serial.println("Error iniciando RF driver");
  } else {
    Serial.println("RF driver iniciado correctamente");
  }

  Serial.println("Receptor listo...");
}

void loop() {
  uint8_t buf[32];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    String data = String((char*)buf);
    data.trim();

    float temperature = 0.0;
    float humidity = 0.0;

    int tIndex = data.indexOf("T:");
    int hIndex = data.indexOf("H:");

    if (tIndex != -1 && hIndex != -1) {
      int tEnd = data.indexOf("C", tIndex);
      int hEnd = data.indexOf("%", hIndex);

      if (tEnd != -1 && hEnd != -1) {
        temperature = data.substring(tIndex + 2, tEnd).toFloat();
        humidity = data.substring(hIndex + 2, hEnd).toFloat();

        String jsonData = "{\"temperature\":" + String(temperature, 1) + 
                          ",\"humidity\":" + String(humidity, 1) + "}";

        Serial.println(jsonData);
        SerialBT.println(jsonData);
      }
    }
  }
}