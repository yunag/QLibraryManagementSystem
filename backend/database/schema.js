import { z } from 'zod'

export const UserSchema = z
  .object({
    username: z.string().min(1),
    password: z.string().min(1)
  })
  .strict()

export const BookSchema = z
  .object({
    title: z.string().trim().min(1).max(255),
    description: z.string().trim().max(65535).optional(),
    cover_url: z.string().trim().url().max(255).optional(),
    publication_date: z.coerce.date().optional(),
    copies_owned: z.coerce.number().optional()
  })
  .strict()

export const AuthorSchema = z
  .object({
    first_name: z.string().trim().min(1).max(255),
    last_name: z.string().trim().min(1).max(255),
    image_url: z.string().trim().url().max(255).optional()
  })
  .strict()

export const CategorySchema = z
  .object({
    name: z.string().trim().min(1).max(255)
  })
  .strict()
