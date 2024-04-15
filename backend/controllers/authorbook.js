import { handleError } from '../common/error.js'
import pool from '../database/database.js'

const AuthorBookController = {
  createRelation: async (req, res) => {
    const { bookid, authorid } = req.params

    const q = 'INSERT INTO book_author (author_id, book_id) VALUES (?, ?)'

    pool
      .query(q, [authorid, bookid])
      .then(data => {
        const [result] = data

        if (result.affectedRows) {
          res.status(201).json({ ok: 'Relationship created' })
        } else {
          res.status(409).json({
            error: `Relation authorid=${authorid}, bookid=${bookid} already exists`
          })
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  deleteRelation: async (req, res) => {
    const { bookid, authorid } = req.params

    const q = 'DELETE FROM book_author WHERE author_id = ? AND book_id = ?'

    pool
      .query(q, [authorid, bookid])
      .then(data => {
        const [result] = data

        if (result.affectedRows) {
          res.status(204).json()
        } else {
          res.status(404).json({
            error: `Relation authorid=${authorid}, bookid=${bookid} does not exists`
          })
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  }
}

export default AuthorBookController
