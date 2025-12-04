#include <RH_ASK.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define TX_PIN 17
RH_ASK driver(2000, -1, TX_PIN, -1);

float tempAnterior = NAN;
float humAnterior = NAN;
const float UMBRAL_PORCENTAJE = 2.0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  Serial.println("Inicializando transmisor RF 433MHz...");
  if (!driver.init()) {
    Serial.println("Error al iniciar el transmisor RF.");
  } else {
    Serial.println("Transmisor RF iniciado correctamente.");
  }

  Serial.println("Sensor DHT11 listo.");
}

void loop() {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Error al leer DHT11");
    delay(2000);
    return;
  }

  bool tempEstable = true;
  bool humEstable = true;

  if (!isnan(tempAnterior)) {
    float variacionTemp = fabs((temperatura - tempAnterior) / tempAnterior) * 100.0;
    if (variacionTemp > UMBRAL_PORCENTAJE) tempEstable = false;
  }

  if (!isnan(humAnterior)) {
    float variacionHum = fabs((humedad - humAnterior) / humAnterior) * 100.0;
    if (variacionHum > UMBRAL_PORCENTAJE) humEstable = false;
  }

  String mensaje = "T:" + String(temperatura, 1) + "C " + (tempEstable ? "E" : "I") +
                   " H:" + String(humedad, 1) + "% " + (humEstable ? "E" : "I");

  const char *msg = mensaje.c_str();
  driver.send((uint8_t*)msg, strlen(msg));
  driver.waitPacketSent();

  Serial.print("Enviado: ");
  Serial.println(mensaje);

  tempAnterior = temperatura;
  humAnterior = humedad;

  delay(2000);
}
