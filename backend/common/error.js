export function internalError(res) {
  return res.status(500).json({ error: 'Internal server error' })
}

export function handleError(err, res) {
  console.error(err)
  internalError(res)
}
