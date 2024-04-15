import { Router } from 'express'
import multer, { diskStorage } from 'multer'
import {
  BookController,
  UserController,
  AuthorController,
  AuthorBookController,
  CategoryController
} from '../controllers/index.js'
import authToken from '../middleware/auth.js'
import path from 'path'
import {
  CategoryValidation,
  CreateAuthorValidation,
  CreateBookValidation,
  GetAuthorsValidation,
  GetBooksValidation,
  LoginValidation,
  UpdateAuthorValidation,
  UpdateBookValidation,
  validate
} from '../common/validation.js'

const router = Router()
const upload_destination = 'uploads'

const storage = diskStorage({
  destination: upload_destination,
  filename: (_req, file, cb) => {
    cb(null, Date.now() + path.extname(file.originalname))
  }
})

const uploads = multer({
  storage: storage,
  limits: {
    fileSize: 2 * 1024 * 1024 // 2MiB
  },
  fileFilter: (_req, file, callback) => {
    const ext = path.extname(file.originalname)
    if (ext !== '.jpg' && ext !== '.png') {
      return callback(new Error('Only images are allowed'))
    }
    callback(null, true)
  }
})

/* Users */
router.post('/register', validate(LoginValidation), UserController.register)
router.post('/login', validate(LoginValidation), UserController.login)

/* Books */
router.post(
  '/books',
  authToken,
  uploads.single('cover'),
  validate(CreateBookValidation),
  BookController.createBook
)
router.get(
  '/books',
  authToken,
  validate(GetBooksValidation),
  BookController.getBooks
)
router.get('/books/:id', authToken, BookController.getBookById)
router.put(
  '/books/:id',
  authToken,
  uploads.single('cover'),
  validate(UpdateBookValidation),
  BookController.updateBookById
)
router.delete('/books/:id', authToken, BookController.deleteBookById)

/* Authors */
router.post(
  '/authors',
  authToken,
  uploads.single('avatar'),
  validate(CreateAuthorValidation),
  AuthorController.createAuthor
)
router.get('/authors/:id', authToken, AuthorController.getAuthorById)
router.get(
  '/authors',
  authToken,
  validate(GetAuthorsValidation),
  AuthorController.getAuthors
)
router.put(
  '/authors/:id',
  authToken,
  uploads.single('avatar'),
  validate(UpdateAuthorValidation),
  AuthorController.updateAuthorById
)
router.delete('/authors/:id', authToken, AuthorController.deleteAuthorById)

/* Categories */
router.post(
  '/categories',
  authToken,
  validate(CategoryValidation),
  CategoryController.createCategory
)

router.get('/categories', authToken, CategoryController.getCategories)
router.get('/categories/:id', authToken, CategoryController.getCategoryById)
router.put('/categories/:id', authToken, CategoryController.updateCategoryById)
router.delete(
  '/categories/:id',
  authToken,
  CategoryController.deleteCategoryById
)

/* Relationship between authors and books */
router.post(
  '/authors/:authorid/books/:bookid',
  authToken,
  AuthorBookController.createRelation
)
router.delete(
  '/authors/:authorid/books/:bookid',
  authToken,
  AuthorBookController.deleteRelation
)

export default router
