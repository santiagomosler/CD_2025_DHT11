import mongoose from "mongoose";

const readingSchema = new mongoose.Schema({
  deviceId: {
    type: String,
    default: "unknown",
    trim: true
  },
  sensor: {
    type: String,
    default: "DHT11",
    trim: true
  },
  temperature: {
    type: Number,
    required: [true, "Temperature is required"],
    min: -50,
    max: 150
  },
  humidity: {
    type: Number,
    required: [true, "Humidity is required"],
    min: 0,
    max: 100
  },
  stable: {
    type: Boolean,
    default: true
  },
  timestamp: {
    type: Date,
    default: Date.now
  }
}, {
  versionKey: false
});

readingSchema.index({ timestamp: 1 }, { expireAfterSeconds: 3600 });

export default mongoose.model("Reading", readingSchema);
