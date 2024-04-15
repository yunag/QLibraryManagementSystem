import { z } from 'zod'
import {
  AuthorSchema,
  BookSchema,
  CategorySchema,
  UserSchema
} from '../database/schema.js'

export const validate = schema => (req, res, next) => {
  try {
    const res = z.object(schema).parse({
      body: req.body,
      query: req.query,
      params: req.params
    })

    req.body = res.body || req.body
    req.query = res.query || req.query
    req.params = res.params || req.params

    next()
  } catch (err) {
    return res.status(400).send(err.issues)
  }
}

export const LoginValidation = {
  body: UserSchema
}

export const CreateBookValidation = {
  body: BookSchema.omit({ cover_url: true })
}

export const UpdateBookValidation = {
  body: BookSchema.omit({ cover_url: true }).partial()
}

export const CategoryValidation = {
  body: CategorySchema
}

export const CreateAuthorValidation = {
  body: AuthorSchema.omit({ image_url: true })
}

export const UpdateAuthorValidation = {
  body: AuthorSchema.omit({ image_url: true }).partial()
}

export const GetBooksValidation = {
  query: z
    .object({
      limit: z.coerce.number().min(1).max(5000).default(200),
      offset: z.coerce.number().min(0).default(0),
      title: z.string().trim().optional(),
      publicationdatestart: z.coerce.date().optional(),
      publicationdateend: z.coerce.date().optional(),
      orderby: z
        .enum([
          'publicationdate',
          'publicationdate-asc',
          'rating',
          'rating-asc',
          'title',
          'title-asc'
        ])
        .optional()
    })
    .strict()
}

export const GetAuthorsValidation = {
  query: z
    .object({
      limit: z.coerce.number().min(1).max(5000).default(200),
      offset: z.coerce.number().min(0).default(0),
      orderby: z
        .enum(['firstname', 'firstname-asc', 'lastname', 'lastname-asc'])
        .optional()
    })
    .strict()
}
