export default function (req, res, next) {
  const apiKey = process.env.API_KEY;
  if (!apiKey) return next();
  const auth = req.headers['authorization'] || '';
  if (auth === `Bearer ${apiKey}`) return next();
  return res.status(401).json({ ok: false, error: "Unauthorized" });
}
