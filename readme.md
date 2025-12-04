https://docs.google.com/document/d/1NSgX56SJ-lAVZMjQqvptkzBY-jhHcA9PcGTwaJtmWNI/edit?tab=t.0

🧩 Lista de materiales (versión con ESP32)
Componente
Cantidad
Función principal
ESP32 DevKit V1 (o similar)
1
Microcontrolador principal. Integra Wi-Fi, Bluetooth y UART/SPI para RF.
Sensor DHT11 o DHT22
1
Sensor de temperatura y humedad ambiente.
Módulo RF 433 MHz TX (transmisor)
1
Envía los datos medidos de temperatura y humedad.
Módulo RF 433 MHz RX (receptor)
1
Recibe los datos transmitidos y los envía al servidor o notebook.
Antenas 433 MHz
2
Mejoran el alcance y estabilidad de transmisión.
Protoboard
1
Para las conexiones iniciales y pruebas.
Cables Dupont (M-M, M-H)
—
Conexiones entre módulos y ESP32.
Fuente de alimentación 5 V (puede ser por USB)
1
Alimenta el ESP32 y los módulos.
Notebook o PC
1
Recibe datos desde el ESP32 (por Wi-Fi, Bluetooth o serial).
Software necesario:
—
Arduino IDE / VSCode (PlatformIO), Node.js (para servidor), MongoDB, navegador web.

💡 Si preferís enviar los datos por Wi-Fi, no necesitarás el módulo RF.
 Pero si querés cumplir con los conceptos de modulación y transmisión digital, el uso del par TX/RX 433 MHz es excelente para mostrarlo como parte de Comunicaciones Digitales.

Requisitos que cumple el proyecto para la asignatura de Comunicaciones Digitales
Aspecto
Descripción
Transmisión digital de datos
El sensor genera datos analógicos (temperatura y humedad) que se digitalizan y se transmiten por un canal RF de 433 MHz.
Codificación / decodificación
El ESP32 codifica los datos (por ejemplo, en formato binario o ASCII) y el receptor los decodifica para interpretarlos.
Comunicación unidireccional (o bidireccional si se agrega feedback)
El sistema muestra un flujo TX → RX con una portadora de RF modulada digitalmente.
Interfaz con sistemas digitales
El ESP32 procesa, formatea y muestra la información en una aplicación web o en la PC, evidenciando integración entre hardware y software.
Aplicación práctica real
La estación meteorológica es un ejemplo concreto de IoT y comunicación inalámbrica de sensores.
Visualización de datos recibidos
Los datos transmitidos se muestran en una interfaz HTML/JS (dashboard), demostrando el paso completo: captura → transmisión → recepción → visualización.
Conectividad moderna (Bluetooth / Wi-Fi)
Permite ampliar el concepto hacia IoT y sistemas conectados, con potencial para almacenamiento en la nube o control remoto.


1) Resumen arquitectónico (visión general)
Nodo sensor (en campo)
ESP32 + DHT11 → lee T/H → arma trama digital → transmite por RF 433 MHz (TX).


Nodo base (cerca del servidor / notebook)
Módulo RF 433 MHz (RX) conectado a un ESP32 receptor → recibe tramas, valida checksum, reenvía por Wi-Fi (HTTP POST / WebSocket) al servidor Node.js que guarda en MongoDB.


Desde el servidor el frontend (HTML/CSS/JS) muestra datos en tiempo real (WebSocket) y persistidos (MongoDB).


Así obtienes: medición → transmisión RF (capas físicas y enlace) → recepción → entrega a red IP → almacenamiento y visualización.

2) Bloques hardware (qué donde)
Nodo sensor (TX):


ESP32 DevKit


DHT11 (o DHT22)


Módulo RF TX 433 MHz (ASK/ASK-TX)


Antena 17 cm


Fuente USB / batería powerbank


Nodo base (RX):


ESP32 DevKit


Módulo RF RX 433 MHz


Antena 17 cm


Conexión Wi-Fi local (router)


Fuente de alimentación USB


PC / Notebook (servidor de desarrollo):


Node.js + MongoDB (puede ser local) + navegador


Opcional:
Pantalla OLED en nodo base para diagnóstico local


RTC o SD para log local en caso de desconexión



3) Conexiones/pines recomendados (ESP32 estándar)
Nodo Sensor (ESP32) — ejemplo de wiring:
DHT11 DATA → GPIO 4 (con pull-up 4.7k recomendado; VCC 3.3V o 5V según modelo)


DHT11 VCC → 3.3V (o 5V si tu DHT lo soporta)


DHT11 GND → GND


RF TX DATA → GPIO 17 (salida digital)


RF TX VCC → 5V (si requiere) o 3.3V según módulo


RF TX GND → GND


Antena TX → hilo ~17 cm al pin ANT del módulo (o directamente al módulo si no hay pin)


Nodo Base (ESP32 receptor):
RF RX DATA → GPIO 16 (entrada digital)


RF RX VCC → 5V/3.3V según módulo


RF RX GND → GND


OLED I²C SDA → GPIO 21, SCL → GPIO 22 (opcional)


Wi-Fi: conectado a la red local (no pines)


Nota: verifica tensión tolerada del módulo RF que uses; muchos TX funcionan mejor a 5V, los RX suelen tolerar 5V o 3.3V. ESP32 trabaja a 3.3V; si alimentas módulo a 5V, conecta sólo la línea de datos con un divisor si es entrada al ESP32.

4) Formato de la trama y protocolo (nivel enlace/aplicación)
Objetivos: formato simple, delimitado, con checksum para detectar errores. También incluimos un campo de secuencia para detectar pérdidas.
Trama ASCII (ejemplo)
<STX>VER,ID,SEQ,TEMP,HUM,CHK<ETX>

Ejemplo:
<STX>01,01,005,24.5,52,137<ETX>

Campos:
VER = versión protocolo (01)


ID = ID del nodo (01)


SEQ = número de secuencia (3 dígitos, 000..255)


TEMP = temperatura con 1 dec (ej. 24.5)


HUM = humedad entera (ej. 52)


CHK = checksum: suma de bytes de payload modulo 256 (o CRC8 si quieres más robustez)


<STX> y <ETX> son delimitadores (< y > en ASCII o caracteres especiales 0x02/0x03)


Razonamiento:
ASCII facilita depuración por puerto serie.


Checksum detecta corrupción por el canal RF.


SEQ permite detectar pérdidas y duplicados.



5) Codificación física y temporización
Modulación: ASK (módulos RF 433 típicos).


Baud/rate: los módulos simples no usan "baud" estándar; la librería o esquema de envío define la velocidad. En entornos Arduino/ESP suele usarse ~2000 bps o codificaciones por bit-banging.


Trama guard time: añadir guard interval (p. ej. 50 ms antes y después) para evitar colisiones.


Reintentos: el TX envía N veces cada lectura (p. ej. 3 repeticiones con 100 ms entre envíos) para aumentar probabilidad de entrega.


Intervalo de muestreo: por defecto 30 s a 60 s (para práctica 10 s está bien).


ACK: opcional; si quieres demostrar protocolo de enlace con ACK, implementa respuesta breve desde el receptor (RX → TX) para confirmar recepción. Esto complica por tener que usar dos radios y sincronización TX/RX alternado; para el alcance de la materia puedes explicar teoría y dejarlo como mejora.



6) Firmware — Flujos lógicos (pseudocódigo)
Nodo Sensor (ESP32) — loop principal
Leer DHT (T, H). Si error, saltar.


Incrementar SEQ.


Crear payload ASCII VER,ID,SEQ,TEMP,HUM


Calcular checksum (suma bytes % 256).


Armar frame <STX>payload,CHK<ETX>.


Repetir N veces: transmitir frame por pin RF_TX (con encoding definido).


Esperar INTERVAL (ej. 30 s).


Nodo Base (ESP32) — loop principal
Escuchar datos desde pin RF_RX.


Cuando recibe posibles bytes, formar buffer hasta <ETX>.


Verificar <STX> y <ETX>, extraer payload.


Calcular checksum y comparar.


Si OK: parsear campos, almacenar en memoria temporal.


Si FAIL: descartar y opcionalmente incrementar contador de errores.


Enviar HTTP POST al servidor (URL /api/readings) con JSON:


{ "node":"01","seq":5,"temp":24.5,"hum":52,"ts":"2025-10-29T12:34:00Z" }

Si POST OK, opcional mostrar OK en OLED y log.


Si POST falla (no hay red), cachear en SPIFFS/SD y reintentar periódicamente.



7) Arquitectura software (servidor + BD + frontend)
Backend (Node.js + Express)
Endpoint POST /api/readings — recibe JSON de receptor y lo inserta en MongoDB.


Endpoint GET /api/readings?node=01&limit=100 — leer últimos datos.


WebSocket (socket.io) — emite lectura en tiempo real a frontend conectado.


MongoDB (colección readings)
Documento ejemplo:
{
  "_id": "...",
  "node": "01",
  "seq": 5,
  "temp": 24.5,
  "hum": 52,
  "received_at": "2025-10-29T12:34:02Z",
  "rssi": -72,          // opcional si mide señal
  "checksum_ok": true
}

Frontend (HTML/CSS/JS)
Página con panel que muestra:


Lectura actual (temperatura, humedad, timestamp).


Gráfica histórica (Chart.js) con últimos N puntos.


Log de eventos (errores de checksum, reconexiones).


Cliente WebSocket para recibir actualizaciones en tiempo real.

-------------------------------------

#include <DHT.h>

#define DHTPIN 7      // Pin de datos
#define DHTTYPE DHT11 // Tipo de sensor
#define N 10           // Número de muestras para analizar estabilidad
#define TOL_TEMP 0.5  // Tolerancia de temperatura (°C)
#define TOL_HUM 2.0   // Tolerancia de humedad (%)

DHT dht(DHTPIN, DHTTYPE);

// Arrays para almacenar las últimas N lecturas
float tempHist[N] = {0};
float humHist[N] = {0};
int index = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Probando sensor DHT11 con análisis de estabilidad...");
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Verificar si la lectura es válida
  if (isnan(t) || isnan(h)) {
    Serial.println("Error al leer el sensor DHT11");
    delay(1000);
    return;
  }

  // Guardar la lectura en el historial
  tempHist[index] = t;
  humHist[index] = h;
  index = (index + 1) % N;

  // Analizar estabilidad si tenemos N lecturas
  bool tempEstable = true;
  bool humEstable = true;
  for (int i = 0; i < N; i++) {
    if (abs(tempHist[i] - t) > TOL_TEMP) tempEstable = false;
    if (abs(humHist[i] - h) > TOL_HUM) humEstable = false;
  }

  // Mostrar resultados
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" °C");
  Serial.print(tempEstable ? " (estable)" : " (inestable)");

  Serial.print("\tHumedad: ");
  Serial.print(h);
  Serial.print(" %");
  Serial.println(humEstable ? " (estable)" : " (inestable)");

  delay(1000); // Intervalo entre lecturas
}

----------------------------------------

int ThermistorPin = A0;
int Vo;
float R1 = 10000; // value of R1 on board
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; //steinhart-hart coeficients for thermistor
void setup() {
  Serial.begin(9600);
}
void loop() {
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
  T = T - 273.15; //convert Kelvin to Celcius
 // T = (T * 9.0)/ 5.0 + 32.0; //convert Celcius to Farenheit
  Serial.print("Temperature: "); 
  Serial.print(T);
  Serial.println(" C"); 
  delay(500);
}


--------------------------------


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ----- Configuración del sensor DHT -----
#define DHTPIN 7        // Pin de datos
#define DHTTYPE DHT11   // Tipo de sensor
#define N 10            // Número de muestras para analizar estabilidad
#define TOL_TEMP 0.5    // Tolerancia de temperatura (°C)
#define TOL_HUM 2.0     // Tolerancia de humedad (%)

DHT dht(DHTPIN, DHTTYPE);

// ----- Configuración de la pantalla LCD -----
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C 0x27 o 0x3F según el módulo

// Arrays para almacenar las últimas N lecturas
float tempHist[N] = {0};
float humHist[N] = {0};
int index = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Probando sensor DHT11 con análisis de estabilidad...");
  dht.begin();

  // Inicializar pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando DHT11...");
  delay(1000);
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Verificar si la lectura es válida
  if (isnan(t) || isnan(h)) {
    Serial.println("Error al leer el sensor DHT11");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error sensor DHT");
    delay(1000);
    return;
  }

  // Guardar la lectura en el historial
  tempHist[index] = t;
  humHist[index] = h;
  index = (index + 1) % N;

  // Analizar estabilidad si tenemos N lecturas
  bool tempEstable = true;
  bool humEstable = true;
  for (int i = 0; i < N; i++) {
    if (abs(tempHist[i] - t) > TOL_TEMP) tempEstable = false;
    if (abs(humHist[i] - h) > TOL_HUM) humEstable = false;
  }

  // Mostrar resultados en Serial
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" °C");
  Serial.print(tempEstable ? " (E)" : " (I)");
  Serial.print("\tHumedad: ");
  Serial.print(h);
  Serial.print(" %");
  Serial.println(humEstable ? " (E)" : " (I)");

  // Mostrar resultados en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print("C ");
  lcd.print(tempEstable ? "E" : "I"); // E=estable, I=inestable

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(h, 1);
  lcd.print("% ");
  lcd.print(humEstable ? "E" : "I");

  delay(1000); // Intervalo entre lecturas
}
