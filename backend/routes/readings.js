import express from 'express';
import Reading from '../models/reading.js';
import { validateReading } from '../middleware/validateReading.js';

const router = express.Router();

router.post('/', validateReading, async (req, res) => {
  try {
    const reading = new Reading(req.body);
    await reading.save();
    res.status(201).json(reading);
  } catch (err) {
    res.status(500).json({ error: 'Error al guardar la lectura' });
  }
});

router.get('/', async (req, res) => {
  try {
    const readings = await Reading.find().sort({ timestamp: -1 });
    res.json(readings);
  } catch (err) {
    res.status(500).json({ error: 'Error al obtener lecturas' });
  }
});

export default router;
