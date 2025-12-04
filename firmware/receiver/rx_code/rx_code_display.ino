#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>
#include <hd44780.h> 
#include <hd44780ioClass/hd44780_I2Cexp.h>




#define RX_PIN 27  RH_ASK driver(2000, RX_PIN, -1, -1);
hd44780_I2Cexp lcd;
#define LCD_COLS 16
#define LCD_ROWS 2


void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando receptor RF 433MHz...");


  if (!driver.init()) {
    Serial.println("Error al iniciar el receptor RF.");
  } else {
    Serial.println("Receptor RF iniciado correctamente.");
  }


  int status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) {
    Serial.print("Error LCD: ");
    Serial.println(status);
  }


  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Receptor RF listo");
  delay(2000);
  lcd.clear();
}


void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);


  if (driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';
    String mensaje = String((char*)buf);


    Serial.print("Mensaje recibido: ");
    Serial.println(mensaje);


    float temperatura = 0, humedad = 0;


    int tIndex = mensaje.indexOf("T:");
    int cIndex = mensaje.indexOf("C");
    int hIndex = mensaje.indexOf("H:");
    int pIndex = mensaje.indexOf("%");


    if (tIndex != -1 && cIndex != -1 && hIndex != -1 && pIndex != -1) {
      temperatura = mensaje.substring(tIndex + 2, cIndex).toFloat();
      humedad = mensaje.substring(hIndex + 2, pIndex).toFloat();
    }


    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");


    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura, 1);
    lcd.print((char)223);
    lcd.print("C");


    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.print(humedad, 1);
    lcd.print("%");
  }
}
