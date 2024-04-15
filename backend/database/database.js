import { createPool } from 'mysql2'

const pool = createPool({
  host: process.env.MYSQL_HOST,
  user: 'root',
  password: process.env.MYSQL_ROOT_PASSWORD,
  database: 'qlibrarydb'
}).promise()

export default pool
