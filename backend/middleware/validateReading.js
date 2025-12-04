export function validateReading(req, res, next) {
  const { temperature, humidity } = req.body;
  if (temperature === undefined || humidity === undefined) {
    return res.status(400).json({ error: 'Faltan campos: temperature y/o humidity' });
  }
  if (typeof temperature !== 'number' || typeof humidity !== 'number') {
    return res.status(400).json({ error: 'Los valores deben ser numéricos' });
  }
  if (temperature < -50 || temperature > 100) {
    return res.status(400).json({ error: 'Temperatura fuera de rango (-50°C a 100°C)' });
  }
  if (humidity < 0 || humidity > 100) {
    return res.status(400).json({ error: 'Humedad fuera de rango (0% a 100%)' });
  }
  next();
}
