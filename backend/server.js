import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import path from "path";
import { fileURLToPath } from "url";
import cors from "cors";
import readingsRouter from "./routes/readings.js";
import { startBluetoothListener } from "./bluetoothListener.js";

dotenv.config();

const app = express();
const PORT = process.env.PORT || 3000;

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

app.use(express.json());
app.use(cors());

mongoose
  .connect(process.env.MONGODB_URI, {
    dbName: "CD_UNMDP_2025",
  })
  .then(() => {
    startBluetoothListener("COM5");
  })
  .catch((err) => console.error("Error al conectar a MongoDB:", err));

app.use("/api/readings", readingsRouter);

const frontendPath = path.join(__dirname, "../frontend");
app.use(express.static(frontendPath));

app.get(/^(?!\/api).*/, (req, res) => {
  res.sendFile(path.join(frontendPath, "index.html"));
});

app.listen(PORT, () => {
  console.log(`Servidor corriendo en http://localhost:${PORT}`);
});
