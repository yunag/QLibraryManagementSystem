import { handleError } from '../common/error.js'
import { addWhereClause } from '../common/query.js'
import pool from '../database/database.js'

const baseQuery = `
SELECT
  book_id AS id,
  title,
  description,
  cover_url,
  publication_date,
  copies_owned,
  COALESCE(
    (
      SELECT
        JSON_ARRAYAGG(
          JSON_OBJECT(
            'first_name',
            a.first_name,
            'last_name',
            a.last_name,
            'id',
            a.author_id
          )
        )
      FROM
        author AS a
        INNER JOIN book_author ba ON ba.author_id = a.author_id
      WHERE
        ba.book_id = b.book_id
      ORDER BY
        a.first_name
    ),
    JSON_ARRAY()
  ) AS authors,
  COALESCE(
    (
      SELECT
        JSON_ARRAYAGG(JSON_OBJECT('name', c.name, 'id', c.category_id))
      FROM
        category AS c
        INNER JOIN book_category bc ON bc.category_id = c.category_id
      WHERE
        bc.book_id = b.book_id
      ORDER BY
        c.name
    ),
    JSON_ARRAY()
  ) AS categories,
  (
    SELECT
      avg(br.rating)
    FROM
      user u
      INNER JOIN book_rating br ON br.user_id = u.user_id
    WHERE
      br.book_id = b.book_id
  ) AS rating
FROM
  book b
`

async function getBookById(id) {
  const q = baseQuery + ' WHERE book_id = ?'

  const [[book]] = await pool.query(q, [id])

  if (book) {
    book.categories = JSON.parse(book.categories)
    book.authors = JSON.parse(book.authors)
  }

  return book
}

function notExistError(res, id) {
  return res.status(404).json({
    error: `Author with id=${id} does not exist`
  })
}

const BookController = {
  updateBookById: async (req, res) => {
    const { id } = req.params

    if (req.file && req.file.path) {
      req.body.cover_url = req.file.path
    }

    const q = `UPDATE book SET ? WHERE book_id = ?`

    pool
      .query(q, [req.body, id])
      .then(async data => {
        const [results] = data

        if (results.affectedRows) {
          const book = await getBookById(id)

          res.status(200).json(book)
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  createBook: async (req, res) => {
    const q = `INSERT INTO book SET ?`

    if (req.file && req.file.path) {
      req.body.cover_url = req.file.path
    }

    pool
      .query(q, [req.body])
      .then(async data => {
        const [result] = data

        const book = await getBookById(result.insertId)

        res.status(201).json(book)
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  getBookById: async (req, res) => {
    const { id } = req.params

    getBookById(id)
      .then(book => {
        if (!book) {
          return notExistError(res, id)
        }

        res.status(200).json(book)
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  deleteBookById: async (req, res) => {
    const { id } = req.params

    const q = `DELETE FROM book WHERE book_id = ?`

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

  getBooks: async (req, res) => {
    const userQuery = req.query

    const conditions = []
    const values = []

    if (userQuery.title) {
      conditions.push('title = ?')
      values.push(userQuery.title)
    }
    if (userQuery.publicationdatestart) {
      conditions.push('publication_date > ?')
      values.push(userQuery.publicationdatestart)
    }
    if (userQuery.publicationdateend) {
      conditions.push('publication_date < ?')
      values.push(userQuery.publicationdateend)
    }

    let q = addWhereClause(baseQuery, conditions)

    if (userQuery.orderby) {
      const orderBy = userQuery.orderby.split('-')
      const mapToField = {
        publicationdate: 'publication_date'
      }

      q = `${q} ORDER BY ${mapToField[orderBy[0]] || orderBy[0]} ${orderBy[1] || 'DESC'}`
    }

    q = `${q} LIMIT ? OFFSET ?`

    values.push(userQuery.limit, userQuery.offset)

    pool
      .query(q, values)
      .then(data => {
        const [results] = data

        for (const res of results) {
          res.authors = JSON.parse(res.authors)
          res.categories = JSON.parse(res.categories)
        }

        res.status(200).json(results)
      })
      .catch(err => {
        handleError(err, res)
      })
  }
}

export default BookController
