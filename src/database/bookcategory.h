#ifndef BOOKCATEGORY_H
#define BOOKCATEGORY_H

#include "schema.h"

class BookCategory : public Schema {
  Q_GADGET

public:
  BookCategory(quint32 book_id, quint32 category_id)
      : book_id(book_id), category_id(category_id) {}

  BookCategory() {}

  SCHEMA_NAME(book_category);
  SCHEMA_KEY(quint32, book_id);
  SCHEMA_FIELD(quint32, category_id);
};

#endif /* !BOOKCATEGORY_H */
