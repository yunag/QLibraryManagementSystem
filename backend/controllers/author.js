import { handleError } from '../common/error.js'
import { addWhereClause } from '../common/query.js'
import pool from '../database/database.js'

const baseQuery = `
SELECT
  author_id AS id,
  first_name,
  last_name,
  image_url
FROM
  author
`

async function getAuthorById(id) {
  const q = baseQuery + ' WHERE author_id = ?'

  const [[author]] = await pool.query(q, [id])

  return author
}

function notExistError(res, id) {
  return res.status(404).json({
    error: `Author with id=${id} does not exist`
  })
}

const AuthorController = {
  getAuthorById: async (req, res) => {
    const { id } = req.params

    getAuthorById(id)
      .then(author => {
        if (author) {
          res.status(200).json(author)
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  deleteAuthorById: async (req, res) => {
    const { id } = req.params

    const q = `DELETE FROM author WHERE author_id = ?`

    pool
      .query(q, [id])
      .then(data => {
        const [result] = data

        if (result.affectedRows) {
          res.status(204).json()
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  updateAuthorById: async (req, res) => {
    const { id } = req.params

    if (req.file && req.file.path) {
      req.body.image_url = req.file.path
    }

    const q = `UPDATE author SET ? WHERE author_id = ?`

    pool
      .query(q, [req.body, id])
      .then(async data => {
        const [result] = data

        if (result.affectedRows) {
          const author = await getAuthorById(id)

          res.status(200).json(author)
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  createAuthor: async (req, res) => {
    const q = `INSERT INTO author SET ?`

    if (req.file && req.file.path) {
      req.body.image_url = req.file.path
    }

    pool
      .query(q, [req.body])
      .then(async data => {
        const [result] = data

        if (result.affectedRows) {
          const author = await getAuthorById(result.insertId)

          res.status(201).json(author)
        } else {
          res.status(400).json({ error: 'Failed to insert author' })
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  getAuthors: async (req, res) => {
    const userQuery = req.query

    const conditions = []
    const values = []

    if (userQuery.firstname) {
      conditions.push('first_name = ?')
      values.push(userQuery.firstname)
    }
    if (userQuery.lastname) {
      conditions.push('last_name = ?')
      values.push(userQuery.lastname)
    }

    let q = addWhereClause(baseQuery, conditions)

    if (userQuery.orderby) {
      const orderBy = userQuery.orderby.split('-')
      const mapToField = {
        firstname: 'first_name',
        lastname: 'last_name'
      }

      q = `${q} ORDER BY ${mapToField[orderBy[0]]} ${orderBy[1] || 'DESC'}`
    }

    q = `${q} LIMIT ? OFFSET ?`

    values.push(userQuery.limit, userQuery.offset)

    pool
      .query(q, values)
      .then(data => {
        const [results] = data

        res.status(200).json(results)
      })
      .catch(err => {
        handleError(err, res)
      })
  }
}

export default AuthorController
