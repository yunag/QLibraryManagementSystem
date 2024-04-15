import { handleError } from '../common/error.js'
import pool from '../database/database.js'

async function getCategoryById(id) {
  const q = 'SELECT category_id, name FROM category WHERE category_id = ?'

  const [[category]] = await pool.query(q, [id])

  return category
}

function notExistError(res, id) {
  return res.status(404).json({
    error: `Book with id=${id} does not exist`
  })
}

const CategoryController = {
  createCategory: async (req, res) => {
    const { name } = req.body

    const q = 'INSERT INTO category (name) VALUES (?)'

    pool
      .query(q, [name])
      .then(async data => {
        const [result] = data

        if (result.affectedRows) {
          const category = await getCategoryById(result.insertId)

          res.status(201).json(category)
        } else {
          res.status(409).json({ error: 'Category already exists' })
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  getCategoryById: async (req, res) => {
    const { id } = req.params

    getCategoryById(id)
      .then(category => {
        if (category) {
          res.status(200).json(category)
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  getCategories: async (req, res) => {
    const q = 'SELECT category_id as id, name FROM category'

    pool
      .query(q)
      .then(data => {
        const [result] = data

        res.status(200).json(result)
      })
      .catch(err => {
        handleError(err, res)
      })
  },

  deleteCategoryById: async (req, res) => {
    const { id } = req.params

    const q = 'DELETE FROM category WHERE category_id = ?'

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

  updateCategoryById: async (req, res) => {
    const { id } = req.params

    const q = 'UPDATE category SET ? WHERE category_id = ?'

    pool
      .query(q, [req.body, id])
      .then(async data => {
        const [result] = data

        if (result.affectedRows) {
          const category = await getCategoryById(id)

          res.status(200).json(category)
        } else {
          notExistError(res, id)
        }
      })
      .catch(err => {
        handleError(err, res)
      })
  }
}

export default CategoryController
