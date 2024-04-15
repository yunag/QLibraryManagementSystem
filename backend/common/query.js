export function addWhereClause(baseQuery, conditions) {
  if (!conditions || conditions.length === 0) {
    return baseQuery
  }

  const whereClause = ` WHERE ${conditions.join(' AND ')}`
  return baseQuery + whereClause
}
