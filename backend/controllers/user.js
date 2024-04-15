import pool from '../database/database.js'
import { genSaltSync, hashSync, compareSync } from 'bcrypt'

import jwt from 'jsonwebtoken'
import { handleError } from '../common/error.js'

const UserController = {
  register: async (req, res) => {
    const { username, password } = req.body

    const q = 'SELECT * FROM user WHERE username = ?'

    pool
      .query(q, [username])
      .then(async data => {
        const [[user]] = data

        if (user) {
          return res.status(400).json({ error: 'User already exists' })
        }

        const saltRounds = 10
        const salt = genSaltSync(saltRounds)
        const hashedPassword = hashSync(password, salt)

        const insertQuery =
          'INSERT INTO user (username, password) VALUES (?, ?)'

        await pool.query(insertQuery, [username, hashedPassword])

        res.status(200).json({ ok: 'User successfully created' })
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  login: async (req, res) => {
    const { username, password } = req.body

    const q = 'SELECT password FROM user WHERE username = ?'

    pool
      .query(q, [username])
      .then(async data => {
        const [[user]] = data

        if (!user) {
          return res.status(400).json({ error: 'Invalid username or password' })
        }

        const valid = compareSync(password, user.password)
        if (!valid) {
          return res.status(400).json({ error: 'Invalid username or password' })
        }

        const token = jwt.sign({ userId: user.id }, process.env.SECRET_KEY)

        res.status(200).json({ token: token })
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  updateUser: async (req, res) => {
    const { id } = req.params
    const { username } = req.body.username

    const userId = req.user.userId

    if (id !== userId) {
      return res.status(403).json({ error: 'No access' })
    }

    const q = `UPDATE user
       SET username = ?
       WHERE user_id = ?`

    pool
      .query(q, [username, id])
      .then(data => {
        const [result] = data

        if (result.affectedRows) {
          res.status(200).json({ ok: 'Updated' })
        } else {
          res.status(404).json({ error: `User with id=${id} does not exists` })
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  }
}

export default UserController
