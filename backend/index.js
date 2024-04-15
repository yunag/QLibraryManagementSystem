import express from 'express'
import fs from 'fs'

import 'dotenv/config'

import router from './routes/route.js'

const app = express()

app.use(express.urlencoded({ extended: true }))
app.use(express.json())
app.use('/uploads', express.static('uploads'))
app.use('/api', router)

app.use((err, _req, res, _next) => {
  console.error(err)
  res
    .status(err.status || 500)
    .json({ error: err.message || 'Something went wrong' })
})

if (!fs.existsSync('uploads')) {
  fs.mkdirSync('uploads')
}

const port = process.env.PORT || 3000

app.listen(port, () => {
  console.log(`Server listening on port ${port}`)
})
