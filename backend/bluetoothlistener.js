import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import Reading from "./models/reading.js";

function tryParseJSONFromString(line) {
  const trimmed = line.trim();

  const noPrefix = trimmed.replace(/^Enviado:\s*/i, "");

  if (/^[\{\[]/.test(noPrefix)) {
    try {
      return JSON.parse(noPrefix);
    } catch {
      return null;
    }
  }

  const match = noPrefix.match(/(\{.*\}|\[.*\])/);
  if (match) {
    try {
      return JSON.parse(match[0]);
    } catch {
      return null;
    }
  }

  return null;
}

export function startBluetoothListener(portPath) {
  try {
    const port = new SerialPort({ path: portPath, baudRate: 9600 });
    const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

    parser.on("data", async (data) => {
      const json = tryParseJSONFromString(String(data));
      if (!json) {
        return;
      }

      try {
        const newReading = new Reading({
          deviceId: json.deviceId ?? "ESP32_RX_RF",
          temperature: json.temperature,
          humidity: json.humidity,
          stable: json.stable ?? true,
        });

        await newReading.save();
      } catch (err) {
        console.error("Error guardando lectura:", err.message);
      }
    });

    port.on("error", (err) => {
      console.error("Error en el puerto serial:", err.message);
    });
  } catch (error) {
    console.error("No se pudo iniciar el listener Bluetooth:", error.message);
  }
}
